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
#ifndef __20130619_QUPDATENOTIFYDLG_H
   #define __20130619_QUPDATENOTIFYDLG_H

#include <QDialog>
#include <QUrl>

namespace Ui {
   class QUpdateNotifyDlg;
}

//---------------------------------------------------------------------------
//! \class   QUpdateNotifyDlg
//! \date    19.06.2013
//! \author  Jo2003
//! \brief   a widget to inform about program updates
//---------------------------------------------------------------------------
class QUpdateNotifyDlg : public QDialog
{
   Q_OBJECT
   
public:
   explicit QUpdateNotifyDlg(QWidget *parent = 0);
   ~QUpdateNotifyDlg();
   void setUpdateData(const QString &str, int minor, int major);
   
protected:
   virtual void changeEvent(QEvent *e);

private slots:
   void on_btnRemind_clicked();
   void slotUpdlinkClicked(QUrl url);

private:
   Ui::QUpdateNotifyDlg *ui;
   int _iMinor;
   int _iMajor;
};

#endif // __20130619_QUPDATENOTIFYDLG_H
