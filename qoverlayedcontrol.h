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
#include <QTimer>

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
  virtual void enterEvent(QEvent *e);
  virtual void leaveEvent(QEvent *e);
  virtual void showEvent(QShowEvent *e);

signals:
  void sigMouseAboveOverlay ();
  void sigMouseLeavesOverlay ();

private slots:
  void slotFadeMore ();

private:
  Ui::QOverlayedControl *ui;
  qreal  _fOpaque;
  QTimer _tFade;
};

#endif // __20121109_QOVERLAYEDCONTROL_H
