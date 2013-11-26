/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qunwindow.cpp
 *
 *  @author   Jo2003
 *
 *  @date     25.11.2013
 *
 *  $Id$
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#include "qunwindow.h"
#include "ui_qunwindow.h"

//---------------------------------------------------------------------------
//
//! \brief   constructor
//
//! \author  Jo2003
//! \date    25.11.2013
//
//! \param   parent (QWidget *) pointer to parent widget
//! \param   f (Qt::WindowFlags) window flags
//
//! \return  --
//---------------------------------------------------------------------------
QUnWindow::QUnWindow(QWidget *parent, Qt::WindowFlags f) :
   QWidget(parent, f),
   ui(new Ui::QUnWindow),
   _fOpaque(0.0)
{
   ui->setupUi(this);

   // transparent background ...
   setAttribute(Qt::WA_TranslucentBackground);

   connect (&_tFade, SIGNAL(timeout()), this, SLOT(slotFadeMore()));
}

//---------------------------------------------------------------------------
//
//! \brief   destructor
//
//! \author  Jo2003
//! \date    25.11.2013
//
//! \return  --
//---------------------------------------------------------------------------
QUnWindow::~QUnWindow()
{
   delete ui;
}

//---------------------------------------------------------------------------
//
//! \brief   retranslate on language change event
//
//! \author  Jo2003
//! \date    25.11.2013
//
//! \param   event (QEvent *) pointer to change event
//
//! \return  --
//---------------------------------------------------------------------------
void QUnWindow::changeEvent(QEvent *event)
{
   switch (event->type())
   {
   case QEvent::LanguageChange:
      // retranslate context menu ...
      ui->retranslateUi(this);
      break;
   default:
      break;
   }

   QWidget::changeEvent(event);
}

//---------------------------------------------------------------------------
//
//! \brief   emit exit windowed signal when clicked on widget
//
//! \author  Jo2003
//! \date    25.11.2013
//
//! \param   e (QMouseEvent *) pointer to mouse click event
//
//! \return  --
//---------------------------------------------------------------------------
void QUnWindow::mousePressEvent(QMouseEvent *e)
{
   emit sigExitWindowed();

   QWidget::mousePressEvent(e);
}

//---------------------------------------------------------------------------
//
//! \brief   widget is about to be shown
//
//! \author  Jo2003
//! \date    27.11.2012
//
//! \param   e pointer to QShowEvent
//
//! \return  --
//---------------------------------------------------------------------------
void QUnWindow::showEvent(QShowEvent *e)
{
   // stop fader timer ...
   _tFade.stop();

   // reset opaque value to default (slightly transparent) ...
   _fOpaque = 0.75;

   // set transparancy to control panel ...
   setWindowOpacity(_fOpaque);

   // set pseudo caption ...
   // ui->labMoveHandle->setText(APP_NAME);

   QWidget::showEvent(e);
}

//---------------------------------------------------------------------------
//
//! \brief   start fade out
//
//! \author  Jo2003
//! \date    27.11.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QUnWindow::fadeOut()
{
   // fade a step every 10 msec ...
   _tFade.start(10);
}

//---------------------------------------------------------------------------
//
//! \brief   fade out more and finally hide widget
//
//! \author  Jo2003
//! \date    27.11.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QUnWindow::slotFadeMore()
{
   _fOpaque -= 0.03;

   if (_fOpaque <= 0.0)
   {
      _tFade.stop();
      hide();
   }
   else
   {
      setWindowOpacity(_fOpaque);
   }
}

