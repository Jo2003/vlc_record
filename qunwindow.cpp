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
   QFadeWidget(parent, f),
   ui(new Ui::QUnWindow)
{
   ui->setupUi(this);
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
