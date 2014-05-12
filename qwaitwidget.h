/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qwaitwidget.h
 *
 *  @author   Jo2003
 *
 *  @date     12.05.2014
 *
 *  $Id$
 *
 *///------------------------- (c) 2014 by Jo2003  --------------------------
#ifndef __20140512_QWAITWIDGET_H
   #define __20140512_QWAITWIDGET_H

#include <QWidget>
#include <QEvent>
#include <QTimer>
#include <QMovie>

namespace Ui {
   class QWaitWidget;
}

//---------------------------------------------------------------------------
//! \class   QWaitWidget
//! \date    12.05.2014
//! \author  Jo2003
//! \brief   a simple wait splash screen
//---------------------------------------------------------------------------
class QWaitWidget : public QWidget
{
   Q_OBJECT

public:
   explicit QWaitWidget(QWidget *parent = 0);
   ~QWaitWidget();

   void longWaitShow();
   void longWaitHide();

protected:
   virtual void changeEvent(QEvent *pEv);

private:
   Ui::QWaitWidget *ui;
   QTimer           longWait;
   QMovie          *pMov;
};

#endif // __20140512_QWAITWIDGET_H
