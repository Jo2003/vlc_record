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

#include <QWidget>
#include <QEvent>
#include <QShowEvent>
#include <QMouseEvent>
#include <QTimer>
#include <QPoint>

namespace Ui {
  class QOverlayedControl;
}

//---------------------------------------------------------------------------
//! \class   QOverlayedControl
//! \date    09.11.2012
//! \author  Jo2003
//! \brief   a player control widget to show while player is fullscreen
//---------------------------------------------------------------------------
class QOverlayedControl : public QWidget
{
  Q_OBJECT

public:
  QOverlayedControl(QWidget *parent = 0, Qt::WindowFlags f = 0);
  virtual ~QOverlayedControl();
  void fadeOut();

protected:
  virtual void changeEvent(QEvent *e);
  virtual void enterEvent(QEvent *e);
  virtual void leaveEvent(QEvent *e);
  virtual void showEvent(QShowEvent *e);
  virtual void mousePressEvent(QMouseEvent *e);
  virtual void mouseMoveEvent(QMouseEvent *e);
  virtual void mouseReleaseEvent(QMouseEvent *e);

signals:
  void sigMouseAboveOverlay ();
  void sigMouseLeavesOverlay ();

private slots:
  void slotFadeMore ();
  void slotMouseEntersMoveHandle ();
  void slotMouseLeavesMoveHandle ();

private:
  Ui::QOverlayedControl *ui;
  qreal  _fOpaque;
  QTimer _tFade;
  QPoint _offset;
  bool   _mouseOverMoveHandle;
};

#endif // __20121109_QOVERLAYEDCONTROL_H
