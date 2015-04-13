/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/sunduk.tv/qoverlayedcontrol.h $
 *
 *  @file     qoverlayedcontrol.h
 *
 *  @author   Jo2003
 *
 *  @date     09.11.2012
 *
 *  $Id: qoverlayedcontrol.h 1284 2014-01-23 12:41:27Z Olenka.Joerg $
 *
 *///------------------------- (c) 2012 by Jo2003  --------------------------
#ifndef __20121109_QOVERLAYEDCONTROL_H
   #define __20121109_QOVERLAYEDCONTROL_H

#include <QEvent>
#include <QShowEvent>
#include <QMouseEvent>
#include <QPoint>
#include "qfadewidget.h"

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

public slots:

private:
  Ui::QOverlayedControl *ui;
  QPoint _offset;
  bool   _mouseOverMoveHandle;
};

#endif // __20121109_QOVERLAYEDCONTROL_H
