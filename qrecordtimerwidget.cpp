/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qrecordtimerwidget.cpp
 *
 *  @author   Jo2003
 *
 *  @date     09.12.2012
 *
 *  $Id$
 *
 *///------------------------- (c) 2012 by Jo2003  --------------------------
#include "qrecordtimerwidget.h"
#include "ui_qrecordtimerwidget.h"

//---------------------------------------------------------------------------
//
//! \fn      timeOut
//
//! \brief   signal is sent when record timer timed out
//
//! \author  Jo2003
//! \date    10.12.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
//! \brief   constructs QRecordTimerWidget
//
//! \author  Jo2003
//! \date    09.12.2012
//
//! \param   parent pointer to parent widget
//
//! \return  --
//---------------------------------------------------------------------------
QRecordTimerWidget::QRecordTimerWidget(QWidget *parent) :
   QDialog(parent),
   ui(new Ui::QRecordTimerWidget)
{
   ui->setupUi(this);

   // 0 ... 55 minutes ...
   ui->spinMinutes->setRange(0, 55);

   // 0 ... 5 hours ...
   ui->spinHours->setRange(0, 5);

   // increase minutes in 5 minute steps ...
   ui->spinMinutes->setSingleStep(5);

   // don't use buffer feature ...
   ui->timeLabel->useBuffer(false);

   ui->spinHours->setSuffix(tr(" h."));
   ui->spinMinutes->setSuffix(tr(" min."));

   _tick.setInterval(1000);
   _tick.setSingleShot(false);
   connect (&_tick, SIGNAL(timeout()), this, SLOT(slotCheckTime()));
}

//---------------------------------------------------------------------------
//
//! \brief   destroys QRecordTimerWidget
//
//! \author  Jo2003
//! \date    09.12.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
QRecordTimerWidget::~QRecordTimerWidget()
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
void QRecordTimerWidget::changeEvent(QEvent *e)
{
   if (e->type() == QEvent::LanguageChange)
   {
      // get current values ...
      int h = ui->spinHours->value();
      int m = ui->spinMinutes->value();

      // translate ...
      ui->retranslateUi(this);

      // translate suffix ...
      ui->spinHours->setSuffix(tr(" h."));
      ui->spinMinutes->setSuffix(tr(" min."));

      // set current value ...
      ui->spinHours->setValue(h);
      ui->spinMinutes->setValue(m);
   }

   QWidget::changeEvent(e);
}

//---------------------------------------------------------------------------
//
//! \brief   start button was pressed
//
//! \author  Jo2003
//! \date    09.12.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QRecordTimerWidget::on_pushStart_clicked()
{
   int h = ui->spinHours->value();
   int m = ui->spinMinutes->value();

   // make sure a time is set ...
   if ((h > 0) || (m > 0))
   {
      // stop tick counter of active ...
      if (_tick.isActive())
      {
         _tick.stop();
      }

      // set reference time ...
      _tRef = QTime(h, m);

      // display time to go ...
      ui->timeLabel->setTime(h * 3600 + m * 60);

      // reset run time ...
      _tRun = QTime();

      // start time ...
      _tRun.start();

      // start tick counter ...
      _tick.start();

      // activate cancel button ...
      ui->pushCancel->setEnabled(true);
   }
}

//---------------------------------------------------------------------------
//
//! \brief   start record timer from extern
//
//! \author  Jo2003
//! \date    27.11.2013
//
//! \param   iSec (int) show length in seconds
//
//! \return  --
//---------------------------------------------------------------------------
void QRecordTimerWidget::startExtern(int iSec)
{
   ui->spinHours->setValue(iSec / 3600);
   ui->spinMinutes->setValue((iSec % 3600) / 60);

   on_pushStart_clicked();
}

//---------------------------------------------------------------------------
//
//! \brief   tick timeout -> check time, emit timeout signal if needed
//
//! \author  Jo2003
//! \date    09.12.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QRecordTimerWidget::slotCheckTime()
{
   QTime tTmp;
   int s = (int)(_tRun.elapsed() / 1000);
   int h = (int)(s / 3600);
   int m = (int)((s % 3600) / 60);
   s    %= 60;

   tTmp.setHMS(h, m, s);

   s = (tTmp.secsTo(_tRef) < 0) ? 0 : tTmp.secsTo(_tRef);

   ui->timeLabel->setTime(s);

   if (!s)
   {
      _tick.stop();
      emit timeOut();
   }
}

//---------------------------------------------------------------------------
//
//! \brief   disable record timer
//
//! \author  Jo2003
//! \date    09.12.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QRecordTimerWidget::on_pushCancel_clicked()
{
   stop();
}

//---------------------------------------------------------------------------
//
//! \brief   stop timer, reset gui
//
//! \author  Jo2003
//! \date    10.12.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QRecordTimerWidget::stop()
{
   _tick.stop();
   ui->pushCancel->setEnabled(false);
   ui->spinHours->setValue(0);
   ui->spinMinutes->setValue(0);
   ui->timeLabel->setTime(0);
}
