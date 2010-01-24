/*********************** Information *************************\
| $HeadURL$
|
| Author: Joerg Neubert
|
| Begin: 24.01.2010 / 15:41:34
|
| Last edited by: $$
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
   sListFile  = QString(INI_DIR).arg(getenv(APPDATA)) + QString("/reclist.xml");
   InitTab();
}

CTimerRec::~CTimerRec()
{
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

void CTimerRec::SetRecInfo(uint uiStart, uint uiEnd, int cid)
{
   QString sStart = QDateTime::fromTime_t(uiStart - TIMER_REC_OFFSET + iTimeShift * 3600)
                    .toString(DEF_TIME_FORMAT);
   QString sEnd   = QDateTime::fromTime_t(uiEnd + TIMER_REC_OFFSET + iTimeShift * 3600)
                    .toString(DEF_TIME_FORMAT);;

   r_ui->edtStart->setText(sStart);

   if (uiEnd > 0)
   {
      r_ui->edtEnd->setText(sEnd);
   }
   else
   {
      r_ui->edtEnd->setText(tr("Insert end time!"));
   }

   r_ui->cbxChannel->setCurrentIndex(r_ui->cbxChannel->findData(QVariant(cid)));

   // set name ...
   QMap<int, rec::SChanEntry>::const_iterator cit = ChanList.find(cid);

   if (cit != ChanList.end())
   {
      r_ui->edtName->setText(QString("%1-%2").arg((*cit).Name).arg(sStart));
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

         ChanList.insert(entry.cid, entry);
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

      for (int i = 0; i < JobList.size(); i++)
      {
         str << "<entry cid=\"" << JobList[i].cid << "\" start=\"" << JobList[i].uiStart
               << "\" end=\"" << JobList[i].uiEnd << "\" name=\"" << JobList[i].sName << "\" />" << endl;
      }

      str << "</timerrec>" << endl;

      fListFile.close();
   }

   return iRV;
}

void CTimerRec::on_btnSet_clicked()
{
   rec::SRecEntry entry;

   uiActId ++;

   entry.cid     = r_ui->cbxChannel->itemData(r_ui->cbxChannel->currentIndex()).toInt();
   entry.uiStart = QDateTime::fromString(r_ui->edtStart->text(), DEF_TIME_FORMAT).toTime_t();
   entry.uiEnd   = QDateTime::fromString(r_ui->edtEnd->text(), DEF_TIME_FORMAT).toTime_t();
   entry.sName   = r_ui->edtName->text();
   entry.id      = uiActId;

   JobList.insert(entry.id, entry);

   AddRow(entry);

   VlcLog.LogInfo(QString("%1 - %2():%3 set set set\n")
                  .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__));
}

void CTimerRec::on_pushOK_clicked()
{
    SaveRecordList();
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
               attrs          = xml.attributes();
               iTimeShift     = attrs.value(QString("timeshift").toUtf8()).toString().toInt();
            }
            else if (xml.name() == "entry")
            {
               uiActId ++;
               attrs          = xml.attributes();
               entry.cid      = attrs.value(QString("cid").toUtf8()).toString().toInt();
               entry.uiStart  = attrs.value(QString("start").toUtf8()).toString().toUInt();
               entry.uiEnd    = attrs.value(QString("end").toUtf8()).toString().toUInt();
               entry.sName    = attrs.value(QString("name").toUtf8()).toString();

               entry.id       = uiActId;

               JobList.insert(entry.id, entry);

               AddRow(entry);

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

   // prepare cell flags ...
   flags = Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled;

   iRows = r_ui->tableRecordEntries->rowCount();

   // one row more ...
   r_ui->tableRecordEntries->setRowCount(iRows + 1);

   // col 1 ...
   when = QDateTime::fromTime_t(entry.uiStart + iTimeShift * 3600);
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
   when = QDateTime::fromTime_t(entry.uiEnd + iTimeShift * 3600);
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
   r_ui->tableRecordEntries->setColumnWidth(0, 70);
   r_ui->tableRecordEntries->setColumnWidth(1, 50);
   r_ui->tableRecordEntries->setColumnWidth(2, 50);
   r_ui->tableRecordEntries->setColumnWidth(3, 40);
}

/************************* History ***************************\
| $Log$
\*************************************************************/
