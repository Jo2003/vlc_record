/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/qnotifydlg.cpp $
 *
 *  @file     qnotifydlg.cpp
 *
 *  @author   Jo2003
 *
 *  @date     19.06.2013
 *
 *  $Id: qnotifydlg.cpp 1299 2014-02-12 14:00:23Z Olenka.Joerg $
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#include "qnotifydlg.h"
#include "ui_qnotifydlg.h"

#include <QDesktopServices>

//---------------------------------------------------------------------------
//
//! \brief   constructs QNotifyDlg
//
//! \author  Jo2003
//! \date    19.06.2013
//
//! \param   parent pointer to parent widget
//
//! \return  --
//---------------------------------------------------------------------------
QNotifyDlg::QNotifyDlg(QWidget *parent) :
   QDialog(parent),
   ui(new Ui::QNotifyDlg)
{
   ui->setupUi(this);

   connect (ui->txtNotify, SIGNAL(anchorClicked(QUrl)), this, SLOT(slotNotifyLinkClicked(QUrl)));
}

//---------------------------------------------------------------------------
//
//! \brief   destroys QNotifyDlg
//
//! \author  Jo2003
//! \date    19.06.2013
//
//! \return  --
//---------------------------------------------------------------------------
QNotifyDlg::~QNotifyDlg()
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
void QNotifyDlg::changeEvent(QEvent *e)
{
   if (e->type() == QEvent::LanguageChange)
   {
      // translate ...
      ui->retranslateUi(this);
   }

   QDialog::changeEvent(e);
}

//---------------------------------------------------------------------------
//
//! \brief   export the remind combobox
//
//! \author  Jo2003
//! \date    12.02.2014
//
//! \param   --
//
//! \return  pointer to combobox
//---------------------------------------------------------------------------
QComboBox *QNotifyDlg::remindCbx()
{
   return ui->cbxRemind;
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
void QNotifyDlg::on_btnRemind_clicked()
{
   accept();
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
void QNotifyDlg::slotNotifyLinkClicked(QUrl url)
{
   QDesktopServices::openUrl(url);
   accept();
}

//---------------------------------------------------------------------------
//
//! \brief   set content for text browser
//
//! \author  Jo2003
//! \date    12.02.2014
//
//! \param   str (const QString&) content
//
//! \return  --
//---------------------------------------------------------------------------
void QNotifyDlg::setNotifyContent(const QString &str)
{
   ui->txtNotify->setHtml(str);
}
