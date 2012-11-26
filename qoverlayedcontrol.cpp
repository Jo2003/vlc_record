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
   ui(new Ui::QOverlayedControl)
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
