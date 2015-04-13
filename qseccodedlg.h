/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/qseccodedlg.h $
 *
 *  @file     qseccodedlg.h
 *
 *  @author   Jo2003
 *
 *  @date     30.05.2012
 *
 *  $Id: qseccodedlg.h 752 2012-06-06 13:23:58Z Olenka.Joerg $
 *
 *///------------------------- (c) 2012 by Jo2003  --------------------------
#ifndef __20120530_QSECCODEDLG_H
   #define __20120530_QSECCODEDLG_H

#include <QDialog>
#include <QString>
#include <QTimer>

//--------------------------------------------------------
/// Namespace
//--------------------------------------------------------
namespace Ui {
   class QSecCodeDlg;
}

//---------------------------------------------------------------------------
//! \class   QSecCodeDlg
//! \date    30.05.2012
//! \author  Jo2003
//! \brief   a simple dialog to capture security code
//---------------------------------------------------------------------------
class QSecCodeDlg : public QDialog
{
   Q_OBJECT
public:
   explicit QSecCodeDlg(QWidget *parent = 0);
   ~QSecCodeDlg();
   const QString &passWd();
   void setPasswd (const QString &pass);

protected:
   virtual void changeEvent(QEvent *e);

private slots:
   void on_buttonBox_accepted();
   void on_buttonBox_rejected();

public slots:
   void slotClearPasswd();

private:
   Ui::QSecCodeDlg *ui;
   QString          sPasswd;
};

#endif // __20120530_QSECCODEDLG_H
