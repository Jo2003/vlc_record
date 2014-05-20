/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qoverlayedcontrol.h
 *
 *  @author   Jo2003
 *
 *  @date     09.11.2012
 *
 *  $Id$
 *
 *///------------------------- (c) 2012 by Jo2003  --------------------------
#ifndef __20121109_QOVERLAYEDCONTROL_H
   #define __20121109_QOVERLAYEDCONTROL_H

#include <QEvent>
#include <QShowEvent>
#include <QMouseEvent>
#include <QPoint>
#include <QPropertyAnimation>
#include <QTimer>
#include "qfadewidget.h"

// sizes of control panel (don't change without a need)
#define __PANEL_WIDTH_EXT  646
#define __PANEL_WIDTH_STD  468
#define __PANEL_HEIGHT_STD 122
#define __PANEL_HEIGHT_INF 228

namespace Ui {
  class QOverlayedControl;
}

//---------------------------------------------------------------------------
//! \class   QOverlayedControl
//! \date    09.11.2012
//! \author  Jo2003
//! \brief   a player control widget to show while player is fullscreen
//---------------------------------------------------------------------------
class QOverlayedControl : public QFadeWidget
{
  Q_OBJECT

public:
  QOverlayedControl(QWidget *parent = 0, Qt::WindowFlags f = 0);
  virtual ~QOverlayedControl();
  void chgWindowed (bool on);
  void chgFullscreen (bool on);

  // define animation states ...
  typedef enum {
     STD_TO_EXT,
     EXT_TO_STD,
     STD_TO_INF,
     INF_TO_STD,
     NO_ANIMATION
  } ani_t;

protected:
  virtual void changeEvent(QEvent *e);
  virtual void enterEvent(QEvent *e);
  virtual void leaveEvent(QEvent *e);
  virtual void mousePressEvent(QMouseEvent *e);
  virtual void mouseMoveEvent(QMouseEvent *e);
  virtual void mouseReleaseEvent(QMouseEvent *e);
  virtual void wheelEvent(QWheelEvent *e);

signals:
  void sigMouseAboveOverlay ();
  void sigMouseLeavesOverlay ();
  void wheel(bool w);

private slots:
  void slotMouseEntersMoveHandle ();
  void slotMouseLeavesMoveHandle ();
  void on_pushHide_clicked();
  void on_pushExt_clicked();
  void fitToContent();
  void on_pushInfo_clicked();

public slots:

private:
  Ui::QOverlayedControl *ui;
  QPoint              _offset;
  bool                _mouseOverMoveHandle;
  QPropertyAnimation *_pAnimation;
  ani_t               _aniType;
};

#endif // __20121109_QOVERLAYEDCONTROL_H
