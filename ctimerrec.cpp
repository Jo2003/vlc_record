#include "ctimerrec.h"
#include "ui_ctimerrec.h"

CTimerRec::CTimerRec(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CTimerRec)
{
    ui->setupUi(this);
    iTimeShift = 0;
    sListFile  = QString(INI_DIR).arg(getenv(APPDATA)) + QString("/reclist.xml");
}

CTimerRec::~CTimerRec()
{
    delete ui;
}

void CTimerRec::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void CTimerRec::SetTimeShift(int iTs)
{
   iTimeShift = iTs;
}

void CTimerRec::SetRecInfo(uint uiStart, uint uiEnd, int cid)
{
   QString sStart = QDateTime::fromTime_t(uiStart - TIMER_REC_OFFSET + iTimeShift * 3600)
                    .toString(DEF_TIME_FORMAT);
   QString sEnd   = QDateTime::fromTime_t(uiEnd + TIMER_REC_OFFSET + iTimeShift * 3600)
                    .toString(DEF_TIME_FORMAT);;

   ui->edtStart->setText(sStart);

   if (uiEnd > 0)
   {
      ui->edtEnd->setText(sEnd);
   }
   else
   {
      ui->edtEnd->setText(tr("Insert end time!"));
   }

   ui->cbxChannel->setCurrentIndex(ui->cbxChannel->findData(QVariant(cid)));

   // set name ...
   QMap<int, Ui::SChanEntry>::const_iterator cit = ChanList.find(cid);

   if (cit != ChanList.end())
   {
      ui->edtName->setText(QString("%1-%2").arg((*cit).Name).arg(sStart));
   }
}

void CTimerRec::SetChanList(const QVector<cparser::SChan> &chanList)
{
   Ui::SChanEntry entry;

   ChanList.clear();
   ui->cbxChannel->clear();
   int iCount = 0;

   for (int i = 0; i < chanList.size(); i++)
   {
      if (chanList[i].iId > -1)
      {
         iCount ++;
         entry.cid  = chanList[i].iId;
         entry.Name = chanList[i].sName;

         ui->cbxChannel->insertItem(i,
                                    QIcon(QString("%1/%2.gif").arg(sLogoPath).arg(entry.cid)),
                                    QString("%1. %2").arg(iCount).arg(entry.Name),
                                    QVariant(entry.cid));

         ChanList.insert(entry.cid, entry);
      }
   }
}

void CTimerRec::SetLogoPath(const QString &str)
{
   sLogoPath = str;
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
   Ui::SRecEntry entry;

   entry.cid     = ui->cbxChannel->itemData(ui->cbxChannel->currentIndex()).toInt();
   entry.uiStart = QDateTime::fromString(ui->edtStart->text(), DEF_TIME_FORMAT).toTime_t();
   entry.uiEnd   = QDateTime::fromString(ui->edtEnd->text(), DEF_TIME_FORMAT).toTime_t();
   entry.sName   = ui->edtName->text();
   entry.id      = JobList.size();

   JobList.insert(entry.id, entry);
}

void CTimerRec::on_pushOK_clicked()
{
    SaveRecordList();
}

int CTimerRec::ReadRecordList()
{
   QXmlStreamReader     xml;
   QXmlStreamAttributes attrs;
   QByteArray           content;
   Ui::SRecEntry        entry;
   int                  iRV = -1;
   JobList.clear();

   QFile fListFile(sListFile);

   if (fListFile.open(QIODevice::ReadOnly | QIODevice::Text))
   {
      content = fListFile.readAll();
      xml.addData(content);

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
               attrs          = xml.attributes();
               entry.cid      = attrs.value(QString("cid").toUtf8()).toString().toInt();
               entry.uiStart  = attrs.value(QString("start").toUtf8()).toString().toUInt();
               entry.uiEnd    = attrs.value(QString("end").toUtf8()).toString().toUInt();
               entry.sName    = attrs.value(QString("name").toUtf8()).toString();

               entry.id       = JobList.size();

               JobList.insert(entry.id, entry);

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
