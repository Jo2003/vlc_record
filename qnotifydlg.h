/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qnotifydlg.h
 *
 *  @author   Jo2003
 *
 *  @date     19.06.2013
 *
 *  $Id$
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#ifndef __20130619_QNOTIFYDLG_H
   #define __20130619_QNOTIFYDLG_H

#include <QDialog>
#include <QUrl>
#include <QComboBox>

namespace Ui {
   class QNotifyDlg;
}

//---------------------------------------------------------------------------
//! \class   QNotifyDlg
//! \date    19.06.2013
//! \author  Jo2003
//! \brief   a widget to inform about something
//---------------------------------------------------------------------------
class QNotifyDlg : public QDialog
{
   Q_OBJECT
   
public:
   explicit QNotifyDlg(QWidget *parent = 0);
   ~QNotifyDlg();
   void setNotifyContent (const QString& str);
   
protected:
   virtual void changeEvent(QEvent *e);
   QComboBox* remindCbx();

public slots:
   virtual void on_btnRemind_clicked();
   virtual void slotNotifyLinkClicked(QUrl url);

private:
   Ui::QNotifyDlg *ui;
};

#endif // __20130619_QNOTIFYDLG_H
