/*********************** Information *************************\
| $HeadURL$
|
| Author: Joerg Neubert
|
| Begin: 24.01.2010 / 15:41:34
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#include "ctimerrec.h"
#include "ui_ctimerrec.h"

extern CLogFile VlcLog;

CTimerRec::CTimerRec(QWidget *parent) : QDialog(parent), r_ui(new Ui::CTimerRec)
{
   r_ui->setupUi(this);
   iTimeShift = 0;
   uiActId    = 0;
   uiEdtId    = INVALID_ID;
   sListFile  = QString(INI_DIR).arg(getenv(APPDATA)) + QString("/reclist.xml");
   pTrigger   = NULL;
   pXmlParser = NULL;
   pSettings  = NULL;
   itActJob   = NULL;
   InitTab();
   connect (&recTimer, SIGNAL(timeout()), this, SLOT(slotRecTimer()));
}

CTimerRec::~CTimerRec()
{
   SaveRecordList();
   delete r_ui;
}

void CTimerRec::changeEvent(QEvent *e)
{
   QDialog::changeEvent(e);

   switch (e->type())
   {
   case QEvent::LanguageChange:
       r_ui->retranslateUi(this);
       break;
   default:
       break;
   }
}

void CTimerRec::SetTimeShift(int iTs)
{
   iTimeShift = iTs;
}

void CTimerRec::SetLogoPath(const QString &str)
{
   sLogoPath = str;
   ReadRecordList();
}

void CTimerRec::StartTimer()
{
   // check time every 5 second ...
   recTimer.start(5000);
}

void CTimerRec::SetXmlParser(CKartinaXMLParser *pParser)
{
   pXmlParser = pParser;
}

void CTimerRec::SetKartinaTrigger(CWaitTrigger *pTrig)
{
   pTrigger = pTrig;
}

void CTimerRec::SetSettings(CSettingsDlg *pSet)
{
   pSettings = pSet;
}

void CTimerRec::SetRecInfo(uint uiStart, uint uiEnd, int cid)
{
   // we don't update ...
   uiEdtId = INVALID_ID;

   // set timeshift stuff ...
   GmtToTimeShift(uiStart);
   GmtToTimeShift(uiEnd);

   QDateTime dtStart = QDateTime::fromTime_t(uiStart - TIMER_REC_OFFSET);
   QDateTime dtEnd   = QDateTime::fromTime_t(uiEnd + TIMER_REC_OFFSET);

   r_ui->dtEdtStart->setDateTime(dtStart);

   if (uiEnd > 1000000) // make sure there was a correct value set for end ...
   {
      r_ui->dtEdtEnd->setDateTime(dtEnd);
   }
   else
   {
      // if no end is given, set end time 1 hour later than start ...
      r_ui->dtEdtEnd->setDateTime(dtStart.addSecs(3600));
   }

   r_ui->cbxChannel->setCurrentIndex(r_ui->cbxChannel->findData(QVariant(cid)));

   // set name ...
   QMap<int, rec::SChanEntry>::const_iterator cit = ChanList.constFind(cid);

   if (cit != ChanList.constEnd())
   {
      r_ui->edtName->setText(QString("%1-%2").arg((*cit).Name).arg(dtStart.toString("dd.MM.yyyy-hh.mm")));
   }
}

void CTimerRec::SetChanList(const QVector<cparser::SChan> &chanList)
{
   rec::SChanEntry entry;

   ChanList.clear();
   r_ui->cbxChannel->clear();
   int iCount = 0;

   for (int i = 0; i < chanList.size(); i++)
   {
      if (chanList[i].iId > -1)
      {
         iCount ++;
         entry.cid  = chanList[i].iId;
         entry.Name = chanList[i].sName;

         r_ui->cbxChannel->insertItem(i,
                                    QIcon(QString("%1/%2.gif").arg(sLogoPath).arg(entry.cid)),
                                    QString("%1. %2").arg(iCount).arg(entry.Name),
                                    QVariant(entry.cid));

         // ChanList.insert(entry.cid, entry);
         ChanList[entry.cid] = entry;
      }
   }
}

int CTimerRec::SaveRecordList()
{
   int iRV = -1;
   QFile fListFile(sListFile);

   if (fListFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
   {
      iRV = 0;
      QTextStream str(&fListFile);
      str.setCodec ("UTF-8");

      str << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>" << endl;
      str << "<timerrec timeshift=\"" << iTimeShift << "\">" << endl;

      QMap<uint, rec::SRecEntry>::const_iterator cit;

      for (cit = JobList.constBegin(); cit != JobList.constEnd(); cit++)
      {
         str << "<entry cid=\"" << (*cit).cid << "\" start=\"" << (*cit).uiStart
               << "\" end=\"" << (*cit).uiEnd << "\" name=\"" << (*cit).sName << "\" />" << endl;
      }

      str << "</timerrec>" << endl;

      fListFile.close();
   }

   return iRV;
}

void CTimerRec::on_btnSet_clicked()
{
   if (uiEdtId != INVALID_ID)
   {
      JobList.remove(uiEdtId);
      DelRow(uiEdtId);
      uiEdtId = INVALID_ID;
   }

   // sanity check ...
   if (!SanityCheck(r_ui->dtEdtStart->dateTime(), r_ui->dtEdtEnd->dateTime()))
   {
      if (r_ui->edtName->text() != "")
      {
         rec::SRecEntry entry;
         entry.cid        = r_ui->cbxChannel->itemData(r_ui->cbxChannel->currentIndex()).toInt();
         entry.uiStart    = r_ui->dtEdtStart->dateTime().toTime_t();
         entry.uiEnd      = r_ui->dtEdtEnd->dateTime().toTime_t();
         entry.sName      = r_ui->edtName->text();
         entry.bIsStarted = false;

         // we need times in GMT ...
         TimeShiftToGmt(entry.uiStart);
         TimeShiftToGmt(entry.uiEnd);

         // AddJob also adds the table row ...
         AddJob (entry);
      }
      else
      {
         QMessageBox::warning(this, tr("Please Correct!"),
                              tr("Please insert a name!"));
      }
   }
   else
   {
      QMessageBox::warning(this, tr("Please Correct!"),
                           tr("The entry you want to add overlapps with an existing entry!\r\n"
                              "Or start time is later than end time!"));
   }
}

int CTimerRec::ReadRecordList()
{
   QXmlStreamReader     xml;
   QXmlStreamAttributes attrs;
   rec::SRecEntry       entry;
   int                  iRV = -1;
   JobList.clear();

   QFile fListFile(sListFile);

   if (fListFile.open(QIODevice::ReadOnly | QIODevice::Text))
   {
      QTextStream str(&fListFile);
      str.setCodec("UTF-8");
      xml.addData(str.readAll());

      while(!xml.atEnd() && !xml.hasError())
      {
         switch (xml.readNext())
         {
         // we aren't interested in ...
         case QXmlStreamReader::StartDocument:
         case QXmlStreamReader::EndElement:
         case QXmlStreamReader::EndDocument:
            break;

         case QXmlStreamReader::StartElement:
            if (xml.name() == "timerrec")
            {
               attrs            = xml.attributes();
               iTimeShift       = attrs.value(QString("timeshift").toUtf8()).toString().toInt();
            }
            else if (xml.name() == "entry")
            {
               attrs            = xml.attributes();
               entry.cid        = attrs.value(QString("cid").toUtf8()).toString().toInt();
               entry.uiStart    = attrs.value(QString("start").toUtf8()).toString().toUInt();
               entry.uiEnd      = attrs.value(QString("end").toUtf8()).toString().toUInt();
               entry.sName      = attrs.value(QString("name").toUtf8()).toString();
               entry.bIsStarted = false;

               // AddJob also adds the table row ...
               AddJob (entry);

               iRV = 0;
            }
            break;

         default:
            break;
         }
      }

      xml.clear();

      /* Error handling. */
      if(xml.hasError())
      {
         QMessageBox::critical(NULL, tr("Error in %1").arg(__FUNCTION__),
                               tr("XML Error String: %1").arg(xml.errorString()));
      }

      fListFile.close();
   }

   return iRV;
}

int CTimerRec::AddRow(const rec::SRecEntry &entry)
{
   QTableWidgetItem *pItem;
   int               iRows;
   QDateTime         when;
   Qt::ItemFlags     flags;
   uint              uiStart, uiEnd;

   // convert times from GMT ...
   uiStart = entry.uiStart;
   uiEnd   = entry.uiEnd;

   GmtToTimeShift(uiStart);
   GmtToTimeShift(uiEnd);

   // prepare cell flags ...
   flags = Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;

   iRows = r_ui->tableRecordEntries->rowCount();

   // one row more ...
   r_ui->tableRecordEntries->setRowCount(iRows + 1);

   // col 1 ...
   when = QDateTime::fromTime_t(uiStart);
   pItem = new QTableWidgetItem (when.toString("dd.MM.yyyy"));
   pItem->setFlags(flags);
   pItem->setData(Qt::UserRole, QVariant(entry.id));
   r_ui->tableRecordEntries->setItem(iRows, 0, pItem);

   // col 2 ...
   pItem = new QTableWidgetItem (when.toString("hh:mm"));
   pItem->setFlags(flags);
   pItem->setData(Qt::UserRole, QVariant(entry.id));
   r_ui->tableRecordEntries->setItem(iRows, 1, pItem);

   // col 3 ...
   when = QDateTime::fromTime_t(uiEnd);
   pItem = new QTableWidgetItem (when.toString("hh:mm"));
   pItem->setFlags(flags);
   pItem->setData(Qt::UserRole, QVariant(entry.id));
   r_ui->tableRecordEntries->setItem(iRows, 2, pItem);

   // col 4 ...
   pItem = new QTableWidgetItem (QIcon(QString("%1/%2.gif").arg(sLogoPath).arg(entry.cid)), "");
   pItem->setFlags(flags);
   pItem->setData(Qt::UserRole, QVariant(entry.id));
   r_ui->tableRecordEntries->setItem(iRows, 3, pItem);

   // col 5 ...
   pItem = new QTableWidgetItem (entry.sName);
   pItem->setFlags(flags);
   pItem->setData(Qt::UserRole, QVariant(entry.id));
   r_ui->tableRecordEntries->setItem(iRows, 4, pItem);

   r_ui->tableRecordEntries->setRowHeight(iRows, 26);

   return iRows + 1;
}

void CTimerRec::InitTab()
{
   r_ui->tableRecordEntries->clearContents();
   r_ui->tableRecordEntries->setIconSize(QSize(24, 24));
   r_ui->tableRecordEntries->setColumnWidth(0, 80);
   r_ui->tableRecordEntries->setColumnWidth(1, 60);
   r_ui->tableRecordEntries->setColumnWidth(2, 60);
   r_ui->tableRecordEntries->setColumnWidth(3, 40);
}

void CTimerRec::AddJob(rec::SRecEntry &entry)
{
   VlcLog.LogInfo(QString("%1 - %2():%3 Add Job #%4\n")
                  .arg(__FILE__).arg(__FUNCTION__)
                  .arg(__LINE__).arg(uiActId));

   entry.id = uiActId;

   JobList[entry.id] = entry;

   AddRow(entry);

   uiActId ++;
}

int CTimerRec::SanityCheck(const QDateTime &start, const QDateTime &end)
{
   int iRV = 0;

   if (start < end)
   {
      uint uiStart = start.toTime_t();
      uint uiEnd   = end.toTime_t();

      TimeShiftToGmt(uiStart);
      TimeShiftToGmt(uiEnd);

      for (int i = 0; i < JobList.size(); i++)
      {
            /* start this between start/end other ...                         */
         if (((uiStart >= JobList[i].uiStart) && (uiStart <= JobList[i].uiEnd))
            /* end this between start/end other ...                           */
            || ((uiEnd >= JobList[i].uiStart) && (uiEnd <= JobList[i].uiEnd))
            /* start this before start other and end this after end other ... */
            || ((uiStart < JobList[i].uiStart) && (uiEnd > JobList[i].uiEnd)))
         {
            iRV = -1;
            break;
         }
      }
   }
   else
   {
      // start later or equal end ...
      iRV = -1;
   }

   return iRV;
}

void CTimerRec::on_tableRecordEntries_cellDoubleClicked(int row, int column)
{
   QTableWidgetItem *pItem = r_ui->tableRecordEntries->item(row, column);

   if (pItem)
   {
      uint id = pItem->data(Qt::UserRole).toUInt();
      uiEdtId = id;

      QMap<uint, rec::SRecEntry>::const_iterator cit;

      cit = JobList.constFind(id);

      if (cit != JobList.constEnd())
      {
         uint when;

         // start ...
         when = (*cit).uiStart;
         GmtToTimeShift(when);
         r_ui->dtEdtStart->setDateTime(QDateTime::fromTime_t(when));

         // end ...
         when = (*cit).uiEnd;
         GmtToTimeShift(when);
         r_ui->dtEdtEnd->setDateTime(QDateTime::fromTime_t(when));

         // channel ...
         r_ui->cbxChannel->setCurrentIndex(r_ui->cbxChannel->findData(QVariant((*cit).cid)));

         // name ...
         r_ui->edtName->setText((*cit).sName);
      }
   }
}

void CTimerRec::GmtToTimeShift(uint &when)
{
   when = when + iTimeShift * 3600;
}

void CTimerRec::TimeShiftToGmt(uint &when)
{
   when = when - iTimeShift * 3600;
}

void CTimerRec::DelRow(uint uiId)
{
   QTableWidgetItem *pItem;
   for (int i = 0; i < r_ui->tableRecordEntries->rowCount(); i++)
   {
      pItem = r_ui->tableRecordEntries->item(i, 0);

      if (pItem)
      {
         if (pItem->data(Qt::UserRole).toUInt() == uiId)
         {
            r_ui->tableRecordEntries->removeRow(i);
            break;
         }
      }
   }
}

void CTimerRec::on_btnDel_clicked()
{
   QTableWidgetItem *pItem = r_ui->tableRecordEntries->currentItem();
   if (pItem)
   {
      uint id = pItem->data(Qt::UserRole).toUInt();
      int iRV = JobList.remove(id);

      r_ui->tableRecordEntries->removeRow(r_ui->tableRecordEntries->currentRow());

      VlcLog.LogInfo(tr("We tried to remove entry #%1 from map. We removed %2 item(s).\n"
                        "No there are %3 entries in map!\n"
                        "Map sayd I'm %4!\n")
                     .arg(id).arg(iRV).arg(JobList.count())
                     .arg((JobList.isEmpty()) ? "empty" : "not empty"));
   }
}

void CTimerRec::slotRecTimer()
{
   if (pTrigger)
   {
      uint now = QDateTime::currentDateTime().toTime_t();
      uint start, end;

      QMap<uint, rec::SRecEntry>::iterator it = JobList.begin();

      while (it != JobList.end())
      {
         start = (*it).uiStart;
         end   = (*it).uiEnd;

         GmtToTimeShift(start);
         GmtToTimeShift(end);

         if (now >= end)
         {
            // old entry --> delete from tab and JobList!
            DelRow((*it).id);
            it = JobList.erase(it);

            if (JobList.isEmpty())
            {
               emit sigAllDone();
            }
            continue;
         }

         if ((start <= now) && !(*it).bIsStarted)
         {
            // start record ...
            (*it).bIsStarted = true;
            itActJob         = it;
            pTrigger->TriggerRequest(Kartina::REQ_TIMERREC, (*it).cid);
         }

         it++;
      }
   }
}

void CTimerRec::slotTimerStreamUrl(QString str)
{
   pXmlParser->SetByteArray(str.toUtf8());

   QString sUrl   = pXmlParser->ParseURL();

   uint uiRunTime = (*itActJob).uiEnd;
   GmtToTimeShift(uiRunTime);
   uiRunTime      = uiRunTime - QDateTime::currentDateTime().toTime_t();

   QString sCmdLine = VLC_REC_TEMPL;

   sCmdLine.replace(TMPL_VLC, pSettings->GetVLCPath());
   sCmdLine.replace(TMPL_URL, sUrl);
   sCmdLine.replace(TMPL_MUX, QString("ts"));
   sCmdLine.replace(TMPL_DST, QString("%1/%2.ts").arg(pSettings->GetTargetDir())
                                                 .arg((*itActJob).sName));

   sCmdLine += QString(" --http-caching=%1 --no-http-reconnect --run-time=%2")
               .arg(pSettings->GetBufferTime())
               .arg(uiRunTime);

   VlcLog.LogInfo(tr("Start VLC using following command line:\n") + sCmdLine + "\n");

   QProcess::startDetached(sCmdLine);

   itActJob = NULL;
}

bool CTimerRec::PendingRecords()
{
   return (JobList.isEmpty()) ? false : true;
}

int CTimerRec::GetTimerRecTimeShift()
{
   return iTimeShift;
}

/************************* History ***************************\
| $Log$
\*************************************************************/

