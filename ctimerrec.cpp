#include "ctimerrec.h"
#include "ui_ctimerrec.h"

CTimerRec::CTimerRec(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CTimerRec)
{
    ui->setupUi(this);
    iTimeShift = 0;
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
