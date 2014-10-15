/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qauthdlg.h
 *
 *  @author   Jo2003
 *
 *  @date     30.04.2014
 *
 *  $Id$
 *
 *///------------------------- (c) 2014 by Jo2003  --------------------------
#ifndef __20140430_QAUTHDLG_H
   #define __20140430_QAUTHDLG_H

#include <QDialog>
#include <QTimer>
#include <QEvent>
#include <QShowEvent>

#include "csettingsdlg.h"

namespace Ui {
   class QAuthDlg;
}

//---------------------------------------------------------------------------
//! \class   QAuthDlg
//! \date    30.04.2014
//! \author  Jo2003
//! \brief   dialog to be displayed at authentication error
//!          (replaces Iptv namespace)
//---------------------------------------------------------------------------
class QAuthDlg : public QDialog
{
   Q_OBJECT

public:
   explicit QAuthDlg(QWidget *parent = 0);
   ~QAuthDlg();

   void setSettings(CSettingsDlg *pSet);

protected:
   virtual void showEvent(QShowEvent *pEv);
   virtual void changeEvent(QEvent *pEv);

private slots:
   void on_buttonBox_accepted();
   void on_buttonBox_rejected();

private:
   Ui::QAuthDlg *ui;
   CSettingsDlg *pSettings;
   QTimer        tmLastTry;
};

#endif // __20140430_QAUTHDLG_H
