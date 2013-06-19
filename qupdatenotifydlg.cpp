/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qupdatenotifydlg.cpp
 *
 *  @author   Jo2003
 *
 *  @date     19.06.2013
 *
 *  $Id$
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#include "qupdatenotifydlg.h"
#include "ui_qupdatenotifydlg.h"
#include "cvlcrecdb.h"

#include <QDateTime>
#include <QDesktopServices>

// db storage class ...
extern CVlcRecDB *pDb;

//---------------------------------------------------------------------------
//
//! \brief   constructs QUpdateNotifyDlg
//
//! \author  Jo2003
//! \date    19.06.2013
//
//! \param   parent pointer to parent widget
//
//! \return  --
//---------------------------------------------------------------------------
QUpdateNotifyDlg::QUpdateNotifyDlg(QWidget *parent) :
   QDialog(parent),
   ui(new Ui::QUpdateNotifyDlg), _iMinor(0), _iMajor(0)
{
   ui->setupUi(this);

   ui->cbxRemind->addItem(tr("Tomorrow"),                1);
   ui->cbxRemind->addItem(tr("In one week"),             7);
   ui->cbxRemind->addItem(tr("In one month"),           30);
#ifndef _TASTE_IPTV_RECORD
   ui->cbxRemind->addItem(tr("Never for this release"), -1);
#endif // _TASTE_IPTV_RECORD

   connect (ui->txtUpdAvailable, SIGNAL(anchorClicked(QUrl)), this, SLOT(slotUpdlinkClicked(QUrl)));
}

//---------------------------------------------------------------------------
//
//! \brief   destroys QUpdateNotifyDlg
//
//! \author  Jo2003
//! \date    19.06.2013
//
//! \return  --
//---------------------------------------------------------------------------
QUpdateNotifyDlg::~QUpdateNotifyDlg()
{
   delete ui;
}

//---------------------------------------------------------------------------
//
//! \brief   make sure GUI is translated as needed
//
//! \author  Jo2003
//! \date    29.11.2012
//
//! \param   e pointer to event
//
//! \return  --
//---------------------------------------------------------------------------
void QUpdateNotifyDlg::changeEvent(QEvent *e)
{
   if (e->type() == QEvent::LanguageChange)
   {
      // get current value ...
      int i = ui->cbxRemind->currentIndex();

      // translate ...
      ui->retranslateUi(this);

      ui->cbxRemind->clear();

      ui->cbxRemind->addItem(tr("Tomorrow"),                1);
      ui->cbxRemind->addItem(tr("In one week"),             7);
      ui->cbxRemind->addItem(tr("In one month"),           30);
#ifndef _TASTE_IPTV_RECORD
      ui->cbxRemind->addItem(tr("Never for this release"), -1);
#endif // _TASTE_IPTV_RECORD

      // set current index ...
      ui->cbxRemind->setCurrentIndex(i);
   }

   QWidget::changeEvent(e);
}

//---------------------------------------------------------------------------
//
//! \brief   button remind was pressed
//
//! \author  Jo2003
//! \date    19.06.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QUpdateNotifyDlg::on_btnRemind_clicked()
{
   // timestamp ...
   qint64 llOffs = ui->cbxRemind->itemData(ui->cbxRemind->currentIndex()).toInt();

   if (llOffs != -1)
   {
      llOffs = QDateTime::currentDateTime().toTime_t() + (llOffs * 3600 * 24);
   }

   pDb->setValue("UpdMinor"    , _iMinor);
   pDb->setValue("UpdMajor"    , _iMajor);
   pDb->setValue("UpdNextCheck", llOffs);

   accept();
}

//---------------------------------------------------------------------------
//
//! \brief   set data to display
//
//! \author  Jo2003
//! \date    19.06.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QUpdateNotifyDlg::setUpdateData(const QString &str, int minor, int major)
{
   ui->txtUpdAvailable->setHtml(str);
   _iMinor = minor;
   _iMajor = major;
}

//---------------------------------------------------------------------------
//
//! \brief   link in text browser clicked -> open link, close dialog
//
//! \author  Jo2003
//! \date    19.06.2013
//
//! \param   url (QUrl) url of clicked link
//
//! \return  --
//---------------------------------------------------------------------------
void QUpdateNotifyDlg::slotUpdlinkClicked(QUrl url)
{
   // remove update stuff from DB ...
   pDb->removeSetting("UpdMinor");
   pDb->removeSetting("UpdMajor");
   pDb->removeSetting("UpdNextCheck");

   QDesktopServices::openUrl(url);
   accept();
}
