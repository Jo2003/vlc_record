/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qunwindow.h
 *
 *  @author   Jo2003
 *
 *  @date     25.11.2013
 *
 *  $Id$
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#ifndef __20131125_QUNWINDOW_H
   #define __20131125_QUNWINDOW_H

#include <QWidget>
#include <QEvent>
#include <QMouseEvent>
#include <QTimer>

namespace Ui {
   class QUnWindow;
}

//---------------------------------------------------------------------------
//! \class   QUnWindow
//! \date    25.11.2013
//! \author  Jo2003
//! \brief   helper widget used as overlay button
//---------------------------------------------------------------------------
class QUnWindow : public QWidget
{
   Q_OBJECT
   
public:
   QUnWindow(QWidget *parent = 0, Qt::WindowFlags f = 0);
   virtual ~QUnWindow();
   void fadeOut();

protected:
   virtual void mousePressEvent(QMouseEvent *e);
   virtual void changeEvent(QEvent *event);
   virtual void showEvent(QShowEvent *e);

signals:
   void sigExitWindowed();

private slots:
   void slotFadeMore ();
   
private:
   Ui::QUnWindow *ui;
   qreal  _fOpaque;
   QTimer _tFade;
};

#endif // __20131125_QUNWINDOW_H
