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

#include <QMouseEvent>
#include "qfadewidget.h"

namespace Ui {
   class QUnWindow;
}

//---------------------------------------------------------------------------
//! \class   QUnWindow
//! \date    25.11.2013
//! \author  Jo2003
//! \brief   helper widget used as overlay button
//---------------------------------------------------------------------------
class QUnWindow : public QFadeWidget
{
   Q_OBJECT
   
public:
   QUnWindow(QWidget *parent = 0, Qt::WindowFlags f = 0);
   virtual ~QUnWindow();

protected:
   virtual void mousePressEvent(QMouseEvent *e);
   virtual void changeEvent(QEvent *event);

signals:
   void sigExitWindowed();

private slots:
   
private:
   Ui::QUnWindow *ui;
};

#endif // __20131125_QUNWINDOW_H
