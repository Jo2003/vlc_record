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
#include "cshowinfo.h"
#include "defdef.h"

// fusion control ...
extern QFusionControl missionControl;

extern CShowInfo showInfo;

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
   QFadeWidget(parent, f),
   ui(new Ui::QOverlayedControl),
   _offset(0, 0),
   _mouseOverMoveHandle(false),
   _pAnimation(NULL)
{
   ui->setupUi(this);

   // feed mission control with control items ...
   missionControl.addButton(ui->pushPlay,          QFusionControl::BTN_PLAY);
   missionControl.addButton(ui->pushStop,          QFusionControl::BTN_STOP);
   missionControl.addButton(ui->pushRecord,        QFusionControl::BTN_REC);
   missionControl.addButton(ui->pushFwd,           QFusionControl::BTN_FWD);
   missionControl.addButton(ui->pushBwd,           QFusionControl::BTN_BWD);
   missionControl.addButton(ui->btnFullScreen,     QFusionControl::BTN_FS);
   missionControl.addButton(ui->btnSaveAspectCrop, QFusionControl::BTN_FRMT);
   missionControl.addButton(ui->btnScrShot,        QFusionControl::BTN_SCRSHOT);
   missionControl.addButton(ui->btnWindowed,       QFusionControl::BTN_WNDWD);

   missionControl.addCngSlider(ui->posSlider);
   missionControl.addJumpBox(ui->cbxTimeJumpVal);
   missionControl.addVolSlider(ui->volSlider);
   missionControl.addTimeLab(ui->labTime);
   missionControl.addLengthLab(ui->labLength);
   missionControl.addMuteLab(ui->labSound);
   missionControl.addInfoLab(ui->labMoveHandle);

   missionControl.addVidFormCbx(ui->cbxAspect, QFusionControl::CBX_ASPECT);
   missionControl.addVidFormCbx(ui->cbxCrop, QFusionControl::CBX_CROP);

   connect (ui->labMoveHandle, SIGNAL(mouseEnters()), this, SLOT(slotMouseEntersMoveHandle()));
   connect (ui->labMoveHandle, SIGNAL(mouseLeabes()), this, SLOT(slotMouseLeavesMoveHandle()));

   // animation stuff for extended settings and information ...
   _pAnimation = new QPropertyAnimation (this, "geometry");
   _pAnimation->setDuration(175);

   // resize to optimized size ...
   resize(__PANEL_WIDTH_STD, __PANEL_HEIGHT_STD);

   // make sure showinfo is updated in info part ...
   connect (&showInfo, SIGNAL(sigHtmlDescr(QString)), ui->textEdit, SLOT(setHtml(QString)));
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
//! \brief   make sure GUI is translated as needed
//
//! \author  Jo2003
//! \date    29.11.2012
//
//! \param   e pointer to event
//
//! \return  --
//---------------------------------------------------------------------------
void QOverlayedControl::changeEvent(QEvent *e)
{
   if (e->type() == QEvent::LanguageChange)
   {
      ui->retranslateUi(this);
   }

   QWidget::changeEvent(e);
}

//---------------------------------------------------------------------------
//
//! \brief   store widget coordinates when mouse is above move handle
//
//! \author  Jo2003
//! \date    29.11.2012
//
//! \param   e pointer to mouse event
//
//! \return  --
//---------------------------------------------------------------------------
void QOverlayedControl::mousePressEvent(QMouseEvent* e)
{
   if (_mouseOverMoveHandle)
   {
      if (isWindow())
      {
         _offset = e->globalPos() - pos();
      }
      else
      {
         _offset = e->pos();
      }
   }

   QWidget::mousePressEvent(e);
}

//---------------------------------------------------------------------------
//
//! \brief   move widget when mouse button is pressed and above move handle
//
//! \author  Jo2003
//! \date    29.11.2012
//
//! \param   e pointer to mouse event
//
//! \return  --
//---------------------------------------------------------------------------
void QOverlayedControl::mouseMoveEvent(QMouseEvent* e)
{
   if (_mouseOverMoveHandle)
   {
      if (isWindow())
      {
         move(e->globalPos() - _offset);
      }
      else
      {
         move(mapToParent(e->pos() - _offset));
      }
   }

   QWidget::mouseMoveEvent(e);
}

//---------------------------------------------------------------------------
//
//! \brief   reset offset value when mouse button released and above move hdl
//
//! \author  Jo2003
//! \date    29.11.2012
//
//! \param   e pointer to mouse event
//
//! \return  --
//---------------------------------------------------------------------------
void QOverlayedControl::mouseReleaseEvent(QMouseEvent* e)
{
   if (_mouseOverMoveHandle)
   {
      _offset = QPoint();
   }
   QWidget::mouseReleaseEvent(e) ;
}

//---------------------------------------------------------------------------
//
//! \brief   override QWidget::wheelEvent()
//
//! \author  Jo2003
//! \date    08.02.2012
//
//! \param   event pointer to wheel event
//
//! \return  --
//---------------------------------------------------------------------------
void QOverlayedControl::wheelEvent(QWheelEvent *event)
{
   event->ignore();

   emit wheel ((event->delta() > 0) ? true : false);
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
//! \brief   mouse entered move handle, change mouse cursor
//
//! \author  Jo2003
//! \date    29.11.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QOverlayedControl::slotMouseEntersMoveHandle()
{
   QApplication::setOverrideCursor(Qt::OpenHandCursor);
   _mouseOverMoveHandle = true;
}

//---------------------------------------------------------------------------
//
//! \brief   mouse leave move handle, change mouse cursor
//
//! \author  Jo2003
//! \date    29.11.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QOverlayedControl::slotMouseLeavesMoveHandle()
{
   QApplication::restoreOverrideCursor();
   _mouseOverMoveHandle = false;
}

//---------------------------------------------------------------------------
//
//! \brief   force fade out
//
//! \author  Jo2003
//! \date    03.12.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QOverlayedControl::on_pushHide_clicked()
{
   fadeOut();
}

//---------------------------------------------------------------------------
//
//! \brief   windowed mode changed -> adapt button
//
//! \author  Jo2003
//! \date    23.01.2014
//
//! \param   on (bool) windowed state
//
//! \return  --
//---------------------------------------------------------------------------
void QOverlayedControl::chgWindowed (bool on)
{
   if (on)
   {
      ui->btnWindowed->setIcon(QIcon(":player/from_wnd_panel"));
   }
   else
   {
      ui->btnWindowed->setIcon(QIcon(":player/to_wnd_panel"));
   }
}

//---------------------------------------------------------------------------
//
//! \brief   fullscreen mode changed -> adapt button
//
//! \author  Jo2003
//! \date    23.01.2014
//
//! \param   on (bool) fullscreen state
//
//! \return  --
//---------------------------------------------------------------------------
void QOverlayedControl::chgFullscreen (bool on)
{
   if (on)
   {
      ui->btnFullScreen->setIcon(QIcon(":player/leave-fullscreen"));
   }
   else
   {
      ui->btnFullScreen->setIcon(QIcon(":player/fullscreen"));
   }
}

//---------------------------------------------------------------------------
//
//! \brief   extend / reduce button clicked
//
//! \author  Jo2003
//! \date    19.05.2014
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QOverlayedControl::on_pushExt_clicked()
{
   if (_pAnimation->state() != QAbstractAnimation::Running)
   {
      QRect geo   = geometry();
      QRect trg   = geo;

      _pAnimation->setStartValue(geo);

      if (_dState & SHOW_EXT)
      {
         trg.setWidth(__PANEL_WIDTH_STD);
         _pAnimation->setEndValue(trg);
         _pAnimation->start();
         ui->pushExt->setIcon(QIcon(":png/open"));
      }
      else
      {
         trg.setWidth(__PANEL_WIDTH_EXT);
         _pAnimation->setEndValue(trg);
         _pAnimation->start();
         ui->pushExt->setIcon(QIcon(":png/close"));
      }

      _dState ^= SHOW_EXT;
   }
}

//---------------------------------------------------------------------------
//
//! \brief   show / hide additional information
//
//! \author  Jo2003
//! \date    20.05.2014
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QOverlayedControl::on_pushInfo_clicked()
{
   if (_pAnimation->state() != QAbstractAnimation::Running)
   {
      QRect geo   = geometry();
      QRect trg   = geo;

      _pAnimation->setStartValue(geo);

      if (_dState & SHOW_INF)
      {
         trg.setHeight(__PANEL_HEIGHT_STD);
         _pAnimation->setEndValue(trg);
         _pAnimation->start();
         ui->pushInfo->setIcon(QIcon(":png/info_open"));
      }
      else
      {
         trg.setHeight(__PANEL_HEIGHT_INF);
         _pAnimation->setEndValue(trg);
         _pAnimation->start();
         ui->pushInfo->setIcon(QIcon(":png/info_close"));
      }

      _dState ^= SHOW_INF;
   }
}
