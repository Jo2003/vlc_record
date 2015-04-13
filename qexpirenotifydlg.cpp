/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/sunduk.tv/qexpirenotifydlg.cpp $
 *
 *  @file     qexpirenotifydlg.cpp
 *
 *  @author   Jo2003
 *
 *  @date     12.02.2014
 *
 *  $Id: qexpirenotifydlg.cpp 1299 2014-02-12 14:00:23Z Olenka.Joerg $
 *
 *///------------------------- (c) 2014 by Jo2003  --------------------------
#include "qexpirenotifydlg.h"

#include "cvlcrecdb.h"

#include <QDateTime>

// db storage class ...
extern CVlcRecDB *pDb;

//---------------------------------------------------------------------------
//
//! \brief   constructs QExpireNotifyDlg
//
//! \author  Jo2003
//! \date    12.02.2014
//
//! \param   parent pointer to parent widget
//
//! \return  --
//---------------------------------------------------------------------------
QExpireNotifyDlg::QExpireNotifyDlg(QWidget *parent) :
   QNotifyDlg(parent)
{
   setWindowTitle(tr("Account Notification"));

   remindCbx()->addItem(tr("Tomorrow"),                1);
   remindCbx()->addItem(tr("In two days"),             2);
   remindCbx()->addItem(tr("In five days"),            5);
}

//---------------------------------------------------------------------------
//
//! \brief   destroys QExpireNotifyDlg
//
//! \author  Jo2003
//! \date    12.02.2014
//
//! \return  --
//---------------------------------------------------------------------------
QExpireNotifyDlg::~QExpireNotifyDlg()
{

}

//---------------------------------------------------------------------------
//
//! \brief   make sure GUI is translated as needed
//
//! \author  Jo2003
//! \date    12.02.2014
//
//! \param   e pointer to event
//
//! \return  --
//---------------------------------------------------------------------------
void QExpireNotifyDlg::changeEvent(QEvent *e)
{
   if (e->type() == QEvent::LanguageChange)
   {
      // get current value ...
      int i = remindCbx()->currentIndex();

      setWindowTitle(tr("Account Notification"));

      remindCbx()->clear();

      remindCbx()->addItem(tr("Tomorrow"),                1);
      remindCbx()->addItem(tr("In two days"),             2);
      remindCbx()->addItem(tr("In five days"),            5);

      // set current index ...
      remindCbx()->setCurrentIndex(i);
   }

   QNotifyDlg::changeEvent(e);
}

//---------------------------------------------------------------------------
//
//! \brief   button remind was pressed
//
//! \author  Jo2003
//! \date    12.02.2014
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QExpireNotifyDlg::on_btnRemind_clicked()
{
   // timestamp ...
   qint64 llOffs = remindCbx()->itemData(remindCbx()->currentIndex()).toInt();

   if (llOffs != -1)
   {
      llOffs = QDateTime::currentDateTime().toTime_t() + (llOffs * 3600 * 24);
   }

   pDb->setValue("ExpNextRemind", llOffs);

   accept();
}
