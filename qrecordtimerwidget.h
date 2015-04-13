/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/sunduk.tv/qrecordtimerwidget.h $
 *
 *  @file     qrecordtimerwidget.h
 *
 *  @author   Jo2003
 *
 *  @date     09.12.2012
 *
 *  $Id: qrecordtimerwidget.h 1238 2013-11-27 15:15:56Z Olenka.Joerg $
 *
 *///------------------------- (c) 2012 by Jo2003  --------------------------
#ifndef __20121209_QRECORDTIMERWIDGET_H
   #define __20121209_QRECORDTIMERWIDGET_H

#include <QDialog>
#include <QTime>
#include <QTimer>
#include <QEvent>
#include <QAbstractButton>

//================================================================
/// \name UI Namespace
//================================================================
namespace Ui {
   class QRecordTimerWidget;
}

//---------------------------------------------------------------------------
//! \class   QRecordTimerWidget
//! \date    09.12.2012
//! \author  Jo2003
//! \brief   a widget to control the record timer
//---------------------------------------------------------------------------
class QRecordTimerWidget : public QDialog
{
   Q_OBJECT
   
public:
   QRecordTimerWidget(QWidget *parent = 0);
   virtual ~QRecordTimerWidget();
   void startExtern(int iSec);

public slots:
   void stop();
   
protected:
   virtual void changeEvent(QEvent *e);

signals:
   void timeOut();

private slots:
   void on_pushStart_clicked();
   void slotCheckTime ();
   void on_pushCancel_clicked();

private:
   Ui::QRecordTimerWidget *ui;
   QTime                  _tRef;
   QTime                  _tRun;
   QTimer                 _tick;
};

#endif // __20121209_QRECORDTIMERWIDGET_H
