/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qauthdlg.cpp
 *
 *  @author   Jo2003
 *
 *  @date     30.04.2014
 *
 *  $Id$
 *
 *///------------------------- (c) 2014 by Jo2003  --------------------------
#include "qauthdlg.h"
#include "ui_qauthdlg.h"
#include "externals_inc.h"

//---------------------------------------------------------------------------
//
//! \brief   construct QAuthDlg object
//
//! \author  Jo2003
//! \date    30.04.2014
//
//! \param   parent (QWidget *) pointer to parent widget
//
//! \return  --
//---------------------------------------------------------------------------
QAuthDlg::QAuthDlg(QWidget *parent) :
   QDialog(parent), ui(new Ui::QAuthDlg), pSettings(NULL)
{
   ui->setupUi(this);
}

//---------------------------------------------------------------------------
//
//! \brief   destroy QAuthDlg object
//
//! \author  Jo2003
//! \date    30.04.2014
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
QAuthDlg::~QAuthDlg()
{
   delete ui;
}

//---------------------------------------------------------------------------
//
//! \brief   dialog to be displayed -> grab account data
//
//! \author  Jo2003
//! \date    30.04.2014
//
//! \param   pEv (QShowEvent *) pointer to show event
//
//! \return  --
//---------------------------------------------------------------------------
void QAuthDlg::showEvent(QShowEvent *pEv)
{
   if (pSettings)
   {
      ui->lineAccount->setText(pSettings->GetUser());
      ui->linePasswd->setText(pSettings->GetPasswd());
   }

   QDialog::showEvent(pEv);
}

//---------------------------------------------------------------------------
//
//! \brief   handle language change event
//
//! \author  Jo2003
//! \date    30.04.2014
//
//! \param   pEv (QEvent *) pointer to event
//
//! \return  --
//---------------------------------------------------------------------------
void QAuthDlg::changeEvent(QEvent *pEv)
{
   if (pEv->type() == QEvent::LanguageChange)
   {
      ui->retranslateUi(this);
   }

   QDialog::changeEvent(pEv);
}


//---------------------------------------------------------------------------
//
//! \brief   get access to settings
//
//! \author  Jo2003
//! \date    30.04.2014
//
//! \param   pSet (CSettingsDlg *) pointer to settings
//
//! \return  --
//---------------------------------------------------------------------------
void QAuthDlg::setSettings(CSettingsDlg *pSet)
{
   pSettings = pSet;
}

//---------------------------------------------------------------------------
//
//! \brief   OK was pressed
//
//! \author  Jo2003
//! \date    30.04.2014
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QAuthDlg::on_buttonBox_accepted()
{
   if (pSettings)
   {
      if ((pSettings->GetUser() != ui->lineAccount->text())
         || (pSettings->GetPasswd() != ui->linePasswd->text()))
      {
         // check for changes ...
         pSettings->setUser(ui->lineAccount->text());
         pSettings->setPasswd(ui->linePasswd->text());

         // store account data ...
         pDb->setValue("User", ui->lineAccount->text());
         pDb->setPassword("PasswdEnc", ui->linePasswd->text());
      }
   }
   accept();
}

//---------------------------------------------------------------------------
//
//! \brief   Cancel was pressed
//
//! \author  Jo2003
//! \date    30.04.2014
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QAuthDlg::on_buttonBox_rejected()
{
   reject();
}
