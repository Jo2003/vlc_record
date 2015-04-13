/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/qseccodedlg.cpp $
 *
 *  @file     qseccodedlg.cpp
 *
 *  @author   Jo2003
 *
 *  @date     30.05.2012
 *
 *  $Id: qseccodedlg.cpp 752 2012-06-06 13:23:58Z Olenka.Joerg $
 *
 *///------------------------- (c) 2012 by Jo2003  --------------------------
#include "qseccodedlg.h"
#include "ui_qseccodedlg.h"

//---------------------------------------------------------------------------
//
//! \brief   constructs QSecCodeDlg object
//
//! \author  Jo2003
//! \date    30.05.2012
//
//! \param   parent pointer to parent widget
//
//! \return  --
//---------------------------------------------------------------------------
QSecCodeDlg::QSecCodeDlg(QWidget *parent) :
   QDialog(parent),
   ui(new Ui::QSecCodeDlg)
{
   ui->setupUi(this);
}

//---------------------------------------------------------------------------
//
//! \brief   destroys QSecCodeDlg object
//
//! \author  Jo2003
//! \date    30.05.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
QSecCodeDlg::~QSecCodeDlg()
{
   delete ui;
}

//---------------------------------------------------------------------------
//
//! \brief   catch change event for language change (translation update)
//
//! \author  Jo2003
//! \date    30.05.2012
//
//! \param   e pointer to change event
//
//! \return  --
//---------------------------------------------------------------------------
void QSecCodeDlg::changeEvent(QEvent *e)
{
   switch (e->type())
   {
   case QEvent::LanguageChange:
      ui->retranslateUi(this);
      break;
   default:
      QDialog::changeEvent(e);
      break;
   }
}

//---------------------------------------------------------------------------
//
//! \brief   ok button pressed, emit passwd signal, accept dialog
//
//! \author  Jo2003
//! \date    30.05.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QSecCodeDlg::on_buttonBox_accepted()
{
   sPasswd = ui->lineSecCode->text();
   ui->lineSecCode->clear();

   // internally store password for 60 seconds ...
   QTimer::singleShot(60000, this, SLOT(slotClearPasswd()));
   accept();
}

//---------------------------------------------------------------------------
//
//! \brief   cancel button pressed, reject dialog
//
//! \author  Jo2003
//! \date    30.05.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QSecCodeDlg::on_buttonBox_rejected()
{
   sPasswd = "";
   reject();
}

//---------------------------------------------------------------------------
//
//! \brief   get password
//
//! \author  Jo2003
//! \date    30.05.2012
//
//! \param   --
//
//! \return  internal stored password
//---------------------------------------------------------------------------
const QString& QSecCodeDlg::passWd()
{
   return sPasswd;
}

//---------------------------------------------------------------------------
//
//! \brief   clear internally stores password [slot]
//
//! \author  Jo2003
//! \date    30.05.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QSecCodeDlg::slotClearPasswd()
{
   sPasswd = "";
}

//---------------------------------------------------------------------------
//
//! \brief   store an external set password
//
//! \author  Jo2003
//! \date    30.05.2012
//
//! \param   pass password to store
//
//! \return  --
//---------------------------------------------------------------------------
void QSecCodeDlg::setPasswd(const QString &pass)
{
   sPasswd = pass;

   // internally store password for 60 seconds ...
   QTimer::singleShot(60000, this, SLOT(slotClearPasswd()));
}
