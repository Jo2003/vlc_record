/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qoverlayedcontrol.cpp
 *
 *  @author   Jo2003
 *
 *  @date     09.11.2012
 *
 *  $Id$
 *
 *///------------------------- (c) 2012 by Jo2003  --------------------------
#include "qoverlayedcontrol.h"
#include "ui_qoverlayedcontrol.h"
#include "qfusioncontrol.h"

// fusion control ...
extern QFusionControl missionControl;

//---------------------------------------------------------------------------
//
//! \brief   constructs QOverlayedControl object
//
//! \author  Jo2003
//! \date    09.11.2012
//
//! \param   parent pointer to parent widget
//! \param   f window flags
//
//! \return  --
//---------------------------------------------------------------------------
QOverlayedControl::QOverlayedControl(QWidget *parent, Qt::WindowFlags f) :
   QWidget(parent, f),
   ui(new Ui::QOverlayedControl),
   _fOpaque(0.0)
{
   ui->setupUi(this);

   // transparent background ...
   setAttribute(Qt::WA_TranslucentBackground);

   // feed mission control with control items ...
   missionControl.addButton(ui->pushPlay,      QFusionControl::BTN_PLAY);
   missionControl.addButton(ui->pushStop,      QFusionControl::BTN_STOP);
   missionControl.addButton(ui->pushRecord,    QFusionControl::BTN_REC);
   missionControl.addButton(ui->pushFwd,       QFusionControl::BTN_FWD);
   missionControl.addButton(ui->pushBwd,       QFusionControl::BTN_BWD);
   missionControl.addButton(ui->btnFullScreen, QFusionControl::BTN_FS);

   missionControl.addCngSlider(ui->posSlider_2);
   missionControl.addJumpBox(ui->cbxTimeJumpVal);
   missionControl.addVolSlider(ui->volSlider);
   missionControl.addTimeLab(ui->labPos_2);

   connect (&_tFade, SIGNAL(timeout()), this, SLOT(slotFadeMore()));
}

//---------------------------------------------------------------------------
//
//! \brief   destroys QOverlayedControl object
//
//! \author  Jo2003
//! \date    09.11.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
QOverlayedControl::~QOverlayedControl()
{
   delete ui;
}

//---------------------------------------------------------------------------
//
//! \brief   widget is about to be shown
//
//! \author  Jo2003
//! \date    27.11.2012
//
//! \param   e pointer to QShowEvent
//
//! \return  --
//---------------------------------------------------------------------------
void QOverlayedControl::showEvent(QShowEvent *e)
{
   // stop fader timer ...
   _tFade.stop();

   // reset opaque value to default (slightly transparent) ...
   _fOpaque = 0.75;

   // set transparancy to control panel ...
   setWindowOpacity(_fOpaque);

   QWidget::showEvent(e);
}

//---------------------------------------------------------------------------
//
//! \brief   mouse enters widget -> emit signal
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   e pointer to QEvent
//
//! \return  --
//---------------------------------------------------------------------------
void QOverlayedControl::enterEvent(QEvent *e)
{
   emit sigMouseAboveOverlay();
   QWidget::enterEvent(e);
}

//---------------------------------------------------------------------------
//
//! \brief   mouse leaves widget -> emit signal
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   e pointer to QEvent
//
//! \return  --
//---------------------------------------------------------------------------
void QOverlayedControl::leaveEvent(QEvent *e)
{
   emit sigMouseLeavesOverlay();
   QWidget::leaveEvent(e);
}

//---------------------------------------------------------------------------
//
//! \brief   start fade out
//
//! \author  Jo2003
//! \date    27.11.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QOverlayedControl::fadeOut()
{
   // fade a step every 10 msec ...
   _tFade.start(10);
}

//---------------------------------------------------------------------------
//
//! \brief   fade out more and finally hide widget
//
//! \author  Jo2003
//! \date    27.11.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QOverlayedControl::slotFadeMore()
{
   _fOpaque -= 0.03;

   if (_fOpaque <= 0.0)
   {
      _tFade.stop();
      hide();
   }
   else
   {
      setWindowOpacity(_fOpaque);
   }
}
