/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 24.01.2010 / 15:41:34
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#include "ctimerrec.h"
#include "ui_ctimerrec.h"
#include <QFileInfo>

// logging stuff ...
extern CLogFile VlcLog;

// for folders ...
extern CDirStuff *pFolders;

// storage db ...
extern CVlcRecDB *pDb;

// global showinfo class ...
extern CShowInfo showInfo;

/* -----------------------------------------------------------------\
|  Method: CTimerRec / constructor
|  Begin: 26.01.2010 / 16:05:00
|  Author: Jo2003
|  Description: create object, init values
|
|  Parameters: pointer to parent window
|
|  Returns: --
\----------------------------------------------------------------- */
CTimerRec::CTimerRec(QWidget *parent) : QDialog(parent), r_ui(new Ui::CTimerRec)
{
   r_ui->setupUi(this);
   iTimeShift    = 0;
   iReqId        = -1;
   uiActId       = 0;
   uiEdtId       = INVALID_ID;
   pApiClient    = NULL;
   pXmlParser    = NULL;
   pSettings     = NULL;
   itActJob      = NULL;
   pStreamLoader = NULL;
   InitTab();
   connect (&recTimer, SIGNAL(timeout()), this, SLOT(slotRecTimer()));
   connect (this, SIGNAL(accepted()), this, SLOT(slotSaveRecordList()));
}

/* -----------------------------------------------------------------\
|  Method: ~CTimerRec / destructor
|  Begin: 26.01.2010 / 16:05:00
|  Author: Jo2003
|  Description: clean on destruction
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
CTimerRec::~CTimerRec()
{
   delete r_ui;
}

/* -----------------------------------------------------------------\
|  Method: changeEvent
|  Begin: 26.01.2010 / 16:05:00
|  Author: Jo2003
|  Description: catch language change event
|
|  Parameters: event pointer
|
|  Returns: --
\----------------------------------------------------------------- */
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

/* -----------------------------------------------------------------\
|  Method: SetTimeShift
|  Begin: 26.01.2010 / 16:05:00
|  Author: Jo2003
|  Description: set timeshift
|
|  Parameters: new timeshift value
|
|  Returns: --
\----------------------------------------------------------------- */
void CTimerRec::SetTimeShift(int iTs)
{
   iTimeShift = iTs;
}

/* -----------------------------------------------------------------\
|  Method: SetStreamLoader
|  Begin: 20.12.2010 / 10:10
|  Author: Jo2003
|  Description: set stream loader
|
|  Parameters: pointer to stream loader class
|
|  Returns: --
\----------------------------------------------------------------- */
void CTimerRec::SetStreamLoader(CStreamLoader *pLoader)
{
   pStreamLoader = pLoader;

   connect (pStreamLoader, SIGNAL(sigStreamDwnTimer(int,QString)), this,
            SLOT(slotStreamReady(int,QString)));
}

/* -----------------------------------------------------------------\
|  Method: StartTimer
|  Begin: 26.01.2010 / 16:05:00
|  Author: Jo2003
|  Description: start timer loop
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CTimerRec::StartTimer()
{
   if (!recTimer.isActive())
   {
      // check time every 3 second ...
      recTimer.start(3000);
   }
}

/* -----------------------------------------------------------------\
|  Method: SetXmlParser
|  Begin: 26.01.2010 / 16:05:00
|  Author: Jo2003
|  Description: set xml parser
|
|  Parameters: pointer to xml parser
|
|  Returns: --
\----------------------------------------------------------------- */
void CTimerRec::SetXmlParser(ApiParser *pParser)
{
   pXmlParser = pParser;
}

/* -----------------------------------------------------------------\
|  Method: setApiClient
|  Begin: 26.01.2010 / 16:05:00
|  Author: Jo2003
|  Description: set wait trigger
|
|  Parameters: pointer to wait trigger
|
|  Returns: --
\----------------------------------------------------------------- */
void CTimerRec::setApiClient(ApiClient *pClient)
{
   pApiClient = pClient;
}

/* -----------------------------------------------------------------\
|  Method: SetSettings
|  Begin: 26.01.2010 / 16:05:00
|  Author: Jo2003
|  Description: set settings
|
|  Parameters: pointer to settings
|
|  Returns: --
\----------------------------------------------------------------- */
void CTimerRec::SetSettings(CSettingsDlg *pSet)
{
   pSettings = pSet;

   if (pSettings->GetShutdownCmd() == "")
   {
      r_ui->checkShutdown->setDisabled(true);
   }
}

/* -----------------------------------------------------------------\
|  Method: SetVlcCtrl
|  Begin: 01.02.2010 / 11:05:00
|  Author: Jo2003
|  Description: set vlc control
|
|  Parameters: pointer to vlc control class
|
|  Returns: --
\----------------------------------------------------------------- */
void CTimerRec::SetVlcCtrl(CVlcCtrl *pCtrl)
{
   pVlcCtrl = pCtrl;
}

/* -----------------------------------------------------------------\
|  Method: SetRecInfo
|  Begin: 26.01.2010 / 16:05:00
|  Author: Jo2003
|  Description: fill in record info into dialog
|
|  Parameters: start time, end time, channel id, program name
|
|  Returns: --
\----------------------------------------------------------------- */
void CTimerRec::SetRecInfo (uint uiStart, uint uiEnd, int cid, const QString &name)
{
   // we don't update ...
   uiEdtId = INVALID_ID;

   // set timeshift stuff ...
   GmtToTimeShift(uiStart, iTimeShift);
   GmtToTimeShift(uiEnd, iTimeShift);

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
   r_ui->cbxTimeShift->setCurrentIndex(r_ui->cbxTimeShift->findText(QString::number(iTimeShift)));

   if (name != "")
   {
      r_ui->edtName->setText(QString("%1(%2)").arg(name)
                             .arg(dtStart.toString("yyyy-MM-dd__hh-mm")));
   }
   else
   {
      // set name ...
      QMap<int, rec::SChanEntry>::const_iterator cit = ChanList.constFind(cid);

      if (cit != ChanList.constEnd())
      {
         r_ui->edtName->setText(QString("%1-%2").arg((*cit).Name)
                                .arg(dtStart.toString("yyyy-MM-dd__hh-mm")));
      }
      else
      {
         r_ui->edtName->setText(dtStart.toString("yyyy-MM-dd, hh-mm"));
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: SetChanList
|  Begin: 26.01.2010 / 16:05:00
|  Author: Jo2003
|  Description: set channel list
|
|  Parameters: ref. to channel list
|
|  Returns: --
\----------------------------------------------------------------- */
void CTimerRec::SetChanList(const QVector<cparser::SChan> &chanList)
{
   rec::SChanEntry entry;
   QFileInfo       fInfo;

   ChanList.clear();
   r_ui->cbxChannel->clear();
   int iCount = 0;

   for (int i = 0; i < chanList.size(); i++)
   {
      if (!chanList[i].bIsGroup)
      {
         iCount ++;
         entry.cid  = chanList[i].iId;
         entry.Name = chanList[i].sName;

         fInfo.setFile(chanList[i].sIcon);

         r_ui->cbxChannel->insertItem(i,
                                    QIcon(QString("%1/%2").arg(pFolders->getLogoDir()).arg(fInfo.fileName())),
                                    QString("%1. %2").arg(iCount).arg(entry.Name),
                                    QVariant(entry.cid));

         ChanList[entry.cid] = entry;
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: slotSaveRecordList
|  Begin: 26.01.2010 / 16:05:00
|  Author: Jo2003
|  Description: save record list to xml file
|
|  Parameters: --
|
|  Returns: 0 ==> ok
|          -1 ==> any error
\----------------------------------------------------------------- */
int CTimerRec::slotSaveRecordList()
{
   int       iRV = 0;
   QSqlQuery query;
   QString   question;

   // delete all stored entries from database ...
   pDb->ask("DELETE FROM timerrec", query);

   QMap<uint, rec::SRecEntry>::const_iterator cit;

   for (cit = JobList.constBegin(); (cit != JobList.constEnd()) && !iRV; cit++)
   {
      question = QString("INSERT INTO timerrec "
                         "(cid, timeshift, recstart, recend, name)"
                         " VALUES (%1, %2, %3, %4, '%5')")
                         .arg((*cit).cid).arg((*cit).iTimeShift)
                         .arg((*cit).uiStart).arg((*cit).uiEnd)
                         .arg((*cit).sName);

      iRV |= pDb->ask(question, query);

      if (iRV)
      {
         QMessageBox::critical(NULL, tr("Error in %1").arg(__FUNCTION__),
                               tr("SQL Error String: %1").arg(pDb->sqlError()));
      }
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: on_btnSet_clicked
|  Begin: 26.01.2010 / 16:05:00
|  Author: Jo2003
|  Description: insert entry into tab and joblist
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CTimerRec::on_btnSet_clicked()
{
   // sanity check ...
   if (!SanityCheck(r_ui->dtEdtStart->dateTime(), r_ui->dtEdtEnd->dateTime(), uiEdtId))
   {
      if (r_ui->edtName->text() != "")
      {
         // update or new entry ... ?
         if (uiEdtId != INVALID_ID)
         {
            // update ...

            // find entry to update ...
            QMap<uint, rec::SRecEntry>::iterator it = JobList.find(uiEdtId);

            // if found, update ...
            if (it != JobList.end())
            {
               (*it).cid        = r_ui->cbxChannel->itemData(r_ui->cbxChannel->currentIndex()).toInt();
               (*it).iTimeShift = r_ui->cbxTimeShift->currentText().toInt();
               (*it).uiStart    = r_ui->dtEdtStart->dateTime().toTime_t();
               (*it).uiEnd      = r_ui->dtEdtEnd->dateTime().toTime_t();
               (*it).sName      = r_ui->edtName->text();

               // leave id and record state untouched ...

               // we need times in GMT ...
               TimeShiftToGmt((*it).uiStart, (*it).iTimeShift);
               TimeShiftToGmt((*it).uiEnd, (*it).iTimeShift);

               // delete from job tab ...
               DelRow(uiEdtId);

               // add row to job tab ...
               AddRow(*it);
            }

            // we're done ...
            uiEdtId = INVALID_ID;
         }
         else
         {
            // new entry ...
            rec::SRecEntry entry;
            entry.cid        = r_ui->cbxChannel->itemData(r_ui->cbxChannel->currentIndex()).toInt();
            entry.uiStart    = r_ui->dtEdtStart->dateTime().toTime_t();
            entry.uiEnd      = r_ui->dtEdtEnd->dateTime().toTime_t();
            entry.sName      = r_ui->edtName->text();
            entry.iTimeShift = r_ui->cbxTimeShift->currentText().toInt();
            entry.eState     = rec::REC_READY;

            // we need times in GMT ...
            TimeShiftToGmt(entry.uiStart, entry.iTimeShift);
            TimeShiftToGmt(entry.uiEnd, entry.iTimeShift);

            // AddJob also adds the table row ...
            AddJob (entry);

            uiEdtId = INVALID_ID;
         }
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
                           tr("The entry you want to add overlapps with an existing entry\n"
                              "or start time is later than end time!"));
   }
}

/* -----------------------------------------------------------------\
|  Method: ReadRecordList
|  Begin: 26.01.2010 / 16:05:00
|  Author: Jo2003
|  Description: read old joblist from xml file
|
|  Parameters: --
|
|  Returns: -1 --> any error
|            0 --> ok
\----------------------------------------------------------------- */
int CTimerRec::ReadRecordList()
{
   QSqlQuery      query;
   rec::SRecEntry entry;
   int            iRV = 0;
   JobList.clear();

   if (!pDb->ask("SELECT id, cid, timeshift, recstart, recend, name FROM timerrec", query))
   {
      while (query.next())
      {
         entry.dbId       = query.value(0).toUInt();
         entry.cid        = query.value(1).toInt();
         entry.iTimeShift = query.value(2).toInt();
         entry.uiStart    = query.value(3).toUInt();
         entry.uiEnd      = query.value(4).toUInt();
         entry.sName      = query.value(5).toString();
         entry.eState     = rec::REC_READY;

         // AddJob also adds the table row ...
         AddJob(entry);
      }
   }
   else
   {
      QMessageBox::critical(NULL, tr("Error in %1").arg(__FUNCTION__),
                            tr("SQL Error String: %1").arg(pDb->sqlError()));
      iRV = -1;
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: AddRow
|  Begin: 26.01.2010 / 16:05:00
|  Author: Jo2003
|  Description: add one row to job tab
|
|  Parameters: ref. to job entry
|
|  Returns: act table row
\----------------------------------------------------------------- */
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

   GmtToTimeShift(uiStart, entry.iTimeShift);
   GmtToTimeShift(uiEnd, entry.iTimeShift);

   // prepare cell flags ...
   flags = Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;

   iRows = r_ui->tableRecordEntries->rowCount();

   // one row more ...
   r_ui->tableRecordEntries->setRowCount(iRows + 1);

   // col 1: date ...
   when = QDateTime::fromTime_t(uiStart);
   pItem = new QTableWidgetItem (when.toString("dd.MM.yyyy"));
   pItem->setFlags(flags);
   pItem->setData(Qt::UserRole, QVariant(entry.id));
   r_ui->tableRecordEntries->setItem(iRows, 0, pItem);

   // col 2: start time ...
   pItem = new QTableWidgetItem (when.toString("hh:mm"));
   pItem->setFlags(flags);
   pItem->setData(Qt::UserRole, QVariant(entry.id));
   r_ui->tableRecordEntries->setItem(iRows, 1, pItem);

   // col 3: end time ...
   when = QDateTime::fromTime_t(uiEnd);
   pItem = new QTableWidgetItem (when.toString("hh:mm"));
   pItem->setFlags(flags);
   pItem->setData(Qt::UserRole, QVariant(entry.id));
   r_ui->tableRecordEntries->setItem(iRows, 2, pItem);

   // col 4: channel ...
   pItem = new QTableWidgetItem (QIcon(QString("%1/%2.gif").arg(pFolders->getLogoDir()).arg(entry.cid)), "");
   pItem->setFlags(flags);
   pItem->setData(Qt::UserRole, QVariant(entry.id));
   r_ui->tableRecordEntries->setItem(iRows, 3, pItem);

   // col. 5: timeshift ...
   pItem = new QTableWidgetItem (QString::number(entry.iTimeShift));
   pItem->setFlags(flags);
   pItem->setData(Qt::UserRole, QVariant(entry.id));
   r_ui->tableRecordEntries->setItem(iRows, 4, pItem);

   // col 6: name ...
   pItem = new QTableWidgetItem (entry.sName);
   pItem->setFlags(flags);
   pItem->setData(Qt::UserRole, QVariant(entry.id));
   r_ui->tableRecordEntries->setItem(iRows, 5, pItem);

   r_ui->tableRecordEntries->setRowHeight(iRows, 26);

   return iRows + 1;
}

/* -----------------------------------------------------------------\
|  Method: InitTab
|  Begin: 26.01.2010 / 16:05:00
|  Author: Jo2003
|  Description: init table view
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CTimerRec::InitTab()
{
   r_ui->tableRecordEntries->clearContents();
   r_ui->tableRecordEntries->setIconSize(QSize(24, 24));
   r_ui->tableRecordEntries->setColumnWidth(0, 80);
   r_ui->tableRecordEntries->setColumnWidth(1, 70);
   r_ui->tableRecordEntries->setColumnWidth(2, 70);
   r_ui->tableRecordEntries->setColumnWidth(3, 40);
   r_ui->tableRecordEntries->setColumnWidth(4, 25);
}

/* -----------------------------------------------------------------\
|  Method: AddJob
|  Begin: 26.01.2010 / 16:05:00
|  Author: Jo2003
|  Description: add job to joblist
|
|  Parameters: ref. to job entry
|
|  Returns: --
\----------------------------------------------------------------- */
void CTimerRec::AddJob(rec::SRecEntry &entry)
{
   mInfo(tr("Add Job #%1 (%2) to Joblist!").arg(uiActId).arg(entry.sName));

   entry.id = uiActId;

   JobList[entry.id] = entry;

   AddRow(entry);

   uiActId ++;
}

/* -----------------------------------------------------------------\
|  Method: SanityCheck
|  Begin: 26.01.2010 / 16:05:00
|  Author: Jo2003
|  Description: check if we can add this job (check times)
|
|  Parameters: start time, end time
|
|  Returns:  0 ==> check ok
|           -1 ==> check not passed
\----------------------------------------------------------------- */
int CTimerRec::SanityCheck(const QDateTime &start, const QDateTime &end, uint uiUpdId)
{
   int iRV = 0;

   if (start < end)
   {
      uint uiStart, uiEnd;

      for (int i = 0; i < JobList.size(); i++)
      {
         if ((uiUpdId == INVALID_ID) || (uiUpdId != JobList[i].id))
         {
            uiStart = JobList[i].uiStart;
            uiEnd   = JobList[i].uiEnd;

            GmtToTimeShift(uiStart, JobList[i].iTimeShift);
            GmtToTimeShift(uiEnd, JobList[i].iTimeShift);

               // start this between start/end other ...
            if (((start.toTime_t() >= uiStart) && (start.toTime_t() <= uiEnd))
               // end this between start/end other ...
               || ((end.toTime_t() >= uiStart) && (end.toTime_t() <= uiEnd))
               // start this before start other and end this after end other ...
               || ((start.toTime_t() < uiStart) && (end.toTime_t() > uiEnd)))
            {
               iRV = -1;
               break;
            }
         }
         else
         {
            // ignore id we try to update ...
            mInfo(tr("Don't check ID %1 (it will be updated).").arg(uiUpdId));
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

/* -----------------------------------------------------------------\
|  Method: on_tableRecordEntries_cellDoubleClicked
|  Begin: 26.01.2010 / 16:05:00
|  Author: Jo2003
|  Description: insert marked entry into edit fields
|
|  Parameters: row, col
|
|  Returns: --
\----------------------------------------------------------------- */
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
         GmtToTimeShift(when, (*cit).iTimeShift);
         r_ui->dtEdtStart->setDateTime(QDateTime::fromTime_t(when));

         // end ...
         when = (*cit).uiEnd;
         GmtToTimeShift(when, (*cit).iTimeShift);
         r_ui->dtEdtEnd->setDateTime(QDateTime::fromTime_t(when));

         // channel ...
         r_ui->cbxChannel->setCurrentIndex(r_ui->cbxChannel->findData(QVariant((*cit).cid)));

         // timshift ...
         r_ui->cbxTimeShift->setCurrentIndex(r_ui->cbxTimeShift->findText(QString::number((*cit).iTimeShift)));

         // name ...
         r_ui->edtName->setText((*cit).sName);
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: GmtToTimeShift
|  Begin: 26.01.2010 / 16:05:00
|  Author: Jo2003
|  Description: convert gmt into timeshift time
|
|  Parameters: ref. to timestamp, timeshift
|
|  Returns: --
\----------------------------------------------------------------- */
void CTimerRec::GmtToTimeShift (uint &when, int iEntryTimeShift)
{
   when = when + iEntryTimeShift * 3600;
}

/* -----------------------------------------------------------------\
|  Method: TimeShiftToGmt
|  Begin: 26.01.2010 / 16:05:00
|  Author: Jo2003
|  Description: convert timeshift time into gmt
|
|  Parameters: ref. to timestamp, timeshift
|
|  Returns: --
\----------------------------------------------------------------- */
void CTimerRec::TimeShiftToGmt(uint &when, int iEntryTimeShift)
{
   when = when - iEntryTimeShift * 3600;
}

/* -----------------------------------------------------------------\
|  Method: DelRow
|  Begin: 26.01.2010 / 16:05:00
|  Author: Jo2003
|  Description: del row from job tab
|
|  Parameters: entry id
|
|  Returns: --
\----------------------------------------------------------------- */
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

/* -----------------------------------------------------------------\
|  Method: delDbEntry
|  Begin: 13.04.2011 / 10:45
|  Author: Jo2003
|  Description: del row from timerrec table
|
|  Parameters: entry id
|
|  Returns: --
\----------------------------------------------------------------- */
void CTimerRec::delDbEntry(int id)
{
   QSqlQuery query;
   query.prepare("DELETE FROM timerrec WHERE id=?");
   query.addBindValue(id);
   pDb->ask(query);
}

/* -----------------------------------------------------------------\
|  Method: on_btnDel_clicked
|  Begin: 26.01.2010 / 16:05:00
|  Author: Jo2003
|  Description: del current job tab entry and matching joblist entry
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CTimerRec::on_btnDel_clicked()
{
   QTableWidgetItem *pItem = r_ui->tableRecordEntries->currentItem();

   if (pItem)
   {
      uint id = pItem->data(Qt::UserRole).toUInt();

      JobList.remove(id);

      r_ui->tableRecordEntries->removeRow(r_ui->tableRecordEntries->currentRow());
   }
}

/* -----------------------------------------------------------------\
|  Method: slotRecTimer
|  Begin: 26.01.2010 / 16:05:00
|  Author: Jo2003
|  Description: timer signal, check joblist if we should start
|               a record job
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CTimerRec::slotRecTimer()
{
   if (pApiClient)
   {
      if (JobList.isEmpty())
      {
         emit sigRecDone();
      }
      else
      {
         uint now = QDateTime::currentDateTime().toTime_t();
         uint start, end;

         QMap<uint, rec::SRecEntry>::iterator it = JobList.begin();

         while (it != JobList.end())
         {
            start = (*it).uiStart;
            end   = (*it).uiEnd;

            GmtToTimeShift(start, (*it).iTimeShift);
            GmtToTimeShift(end, (*it).iTimeShift);

            if (now >= end)
            {
               // old entry ...

               // was record active ... ?
               if ((*it).eState != rec::REC_RUNNING)
               {
                  // old record ... delete without sending signals ...
                  mInfo(tr("Delete old entry #%1 (%2) from Joblist.").arg((*it).id).arg((*it).sName));
                  DelRow((*it).id);
                  delDbEntry((*it).dbId);
                  it = JobList.erase(it);
               }
               else
               {
                  // record was active ... is now done ...
                  mInfo(tr("Stopping timer record #%1 (%2). End time reached!").arg((*it).id).arg((*it).sName));
                  DelRow((*it).id);
                  it = JobList.erase(it);

                  // stop job (vlc) ...
                  if (pVlcCtrl->IsRunning())
                  {
                     pVlcCtrl->stop();
                  }

                  // if own downloader was used ...
                  if (pVlcCtrl->ownDwnld())
                  {
                     pStreamLoader->stopDownload(iReqId);
                     iReqId = -1;
                  }

                  emit sigRecDone();

                  // shut we shut down the system ... ?
                  if (JobList.isEmpty()                     // all done ...
                     && r_ui->checkShutdown->isChecked())   // we want to shut down ...
                  {
                     ShutDown();
                  }
               }
            }
            else
            {
               // should we set timer to stby ... ?
               if (((start - TIMER_STBY_TIME) <= now) && ((*it).eState == rec::REC_READY))
               {
                  // set timer to stby ...
                  emit sigRecActive((int)IncPlay::PS_TIMER_STBY);
                  (*it).eState = rec::REC_STBY;

                  // stop any running vlc ...
                  if (pVlcCtrl->IsRunning())
                  {
                     pVlcCtrl->stop();
                  }

                  mInfo(tr("Record #%1 (%2) starts soon. Set timer to standby!").arg((*it).id).arg((*it).sName));

                  // set timeshift ...
                  pApiClient->queueRequest(CIptvDefs::REQ_TIMESHIFT, (*it).iTimeShift);
               }
               else if ((start <= now) && ((*it).eState == rec::REC_STBY))
               {
                  // start record ...
                  mInfo(tr("Start record #%1 (%2)!").arg((*it).id).arg((*it).sName));
                  emit sigRecActive((int)IncPlay::PS_TIMER_RECORD);
                  (*it).eState = rec::REC_RUNNING;
                  itActJob     = it;

                  showInfo.cleanShowInfo();
                  showInfo.setChanId((*it).cid);
                  showInfo.setShowName((*it).sName);
                  showInfo.setShowType(ShowInfo::Live);
                  showInfo.setStartTime((*it).uiStart);
                  showInfo.setEndTime((*it).uiEnd);
                  showInfo.setPlayState(IncPlay::PS_TIMER_RECORD);
                  showInfo.setChanName(ChanList[(*it).cid].Name);

                  pApiClient->queueRequest(CIptvDefs::REQ_TIMERREC, (*it).cid);
               }

               it++;
            }
         }
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: slotTimerStreamUrl
|  Begin: 26.01.2010 / 16:05:00
|  Author: Jo2003
|  Description: got stream url, start VLC
|
|  Parameters: stream url
|
|  Returns: --
\----------------------------------------------------------------- */
void CTimerRec::slotTimerStreamUrl(const QString &str)
{
   QString sCmdLine;
   Q_PID   vlcpid = 0;
   QString sUrl, sDst;

   if (!pXmlParser->parseUrl(str, sUrl))
   {
      sDst = QString("%1/%2").arg(pSettings->GetTargetDir()).arg((*itActJob).sName);

      if (pVlcCtrl->ownDwnld())
      {
         // own downloader ...
         pStreamLoader->downloadStream(sUrl, QString("%1.%2").arg(sDst).arg("ts"),
                                       pSettings->GetBufferTime(), true);
      }
      else
      {
         if (r_ui->checkRecMini->isChecked())
         {
            // silent record ...
            sCmdLine = pVlcCtrl->CreateClArgs(vlcctrl::VLC_REC_LIVE_SILENT,
                                              pSettings->GetVLCPath(), sUrl,
                                              pSettings->GetBufferTime(), sDst, "ts");
         }
         else
         {
            // normal record ...
            sCmdLine = pVlcCtrl->CreateClArgs(vlcctrl::VLC_REC_LIVE,
                                              pSettings->GetVLCPath(), sUrl,
                                              pSettings->GetBufferTime(), sDst, "ts");
         }

         vlcpid = pVlcCtrl->start(sCmdLine, -1, false, IncPlay::PS_TIMER_RECORD);

         // successfully started ?
         if (!vlcpid)
         {
            QMessageBox::critical(this, tr("Error!"), tr("Can't start Player!"));
         }
         else
         {
            mInfo(tr("Started player with pid #%1!").arg((uint)vlcpid));
         }
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: ShutDown
|  Begin: 02.02.2010 / 15:05:00
|  Author: Jo2003
|  Description: shutdown system with user set command
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CTimerRec::ShutDown()
{
   mInfo(tr("All records done. Shutdown system using command line:\n  --> %1").arg(pSettings->GetShutdownCmd()));
   QProcess::startDetached(pSettings->GetShutdownCmd());
   emit sigShutdown();
}

/* -----------------------------------------------------------------\
|  Method: slotStreamReady [slot]
|  Begin: 20.12.2010 / 10:12
|  Author: Jo2003
|  Description: stream download started, stream can be used
|
|  Parameters: request id, filename of stream stored in HD
|
|  Returns: --
\----------------------------------------------------------------- */
void CTimerRec::slotStreamReady (int Id, QString sName)
{
   iReqId            = Id;

   // on silent record there is no need to show the video ...
   // so only store the request id here ...


   // if no silent record, create command line and show
   // recorded stream ...
   if (!r_ui->checkRecMini->isChecked())
   {
      Q_PID     vlcpid   = 0;
      QString   sCmdLine, fileName, sExt;
      QFileInfo info(sName);

      fileName  = QString ("%1/%2").arg(info.path()).arg(info.completeBaseName());
      sExt      = info.suffix();

      sCmdLine  = pVlcCtrl->CreateClArgs(vlcctrl::VLC_REC_LIVE, pSettings->GetVLCPath(),
                                         "", pSettings->GetBufferTime(),
                                         fileName, sExt);

      // start player if we have a command line ...
      if (sCmdLine != "")
      {
         vlcpid = pVlcCtrl->start(sCmdLine, -1, false, IncPlay::PS_TIMER_RECORD);
      }

      // successfully started ?
      if (!vlcpid)
      {
         QMessageBox::critical(this, tr("Error!"), tr("Can't start Player!"));
      }
      else
      {
         mInfo(tr("Started player with pid #%1!").arg((uint)vlcpid));
      }
   }
}

/************************* History ***************************\
| $Log$
\*************************************************************/

