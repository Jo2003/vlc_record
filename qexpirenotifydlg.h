/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qexpirenotifydlg.h
 *
 *  @author   Jo2003
 *
 *  @date     12.02.2014
 *
 *  $Id$
 *
 *///------------------------- (c) 2014 by Jo2003  --------------------------
#ifndef __20130619_QEXPIRENOTIFYDLG_H
   #define __20130619_QEXPIRENOTIFYDLG_H

#include "qnotifydlg.h"
#include <QEvent>

//---------------------------------------------------------------------------
//! \class   QExpireNotifyDlg
//! \date    12.02.2014
//! \author  Jo2003
//! \brief   a widget to inform about account expire
//---------------------------------------------------------------------------
class QExpireNotifyDlg : public QNotifyDlg
{
   Q_OBJECT
   
public:
   explicit QExpireNotifyDlg(QWidget *parent = 0);
   ~QExpireNotifyDlg();
   
protected:
   virtual void changeEvent(QEvent *e);

public slots:
   virtual void on_btnRemind_clicked();

private:
};

#endif // __20130619_QEXPIRENOTIFYDLG_H
