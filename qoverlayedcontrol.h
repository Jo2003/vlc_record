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

protected:
  virtual void enterEvent(QEvent *e);
  virtual void leaveEvent(QEvent *e);

signals:
  void sigMouseAboveOverlay ();
  void sigMouseLeavesOverlay ();

private:
  Ui::QOverlayedControl *ui;
};

#endif // __20121109_QOVERLAYEDCONTROL_H
