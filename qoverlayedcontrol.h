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
#include <QFlags>
#include "qfadewidget.h"

// sizes of control panel (don't change without a need)
#define __PANEL_WIDTH_EXT  686
#define __PANEL_WIDTH_STD  522
#define __PANEL_HEIGHT_STD 122
#define __PANEL_HEIGHT_INF 240

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

  enum displState {
     SHOW_STD = 0x00,
     SHOW_EXT = 0x01,
     SHOW_INF = 0x02
  };

  Q_DECLARE_FLAGS(displStates, displState)

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
  void on_pushInfo_clicked();

public slots:

private:
  Ui::QOverlayedControl *ui;
  QPoint              _offset;
  bool                _mouseOverMoveHandle;
  QPropertyAnimation *_pAnimation;
  displStates         _dState;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QOverlayedControl::displStates)

#endif // __20121109_QOVERLAYEDCONTROL_H
