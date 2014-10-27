/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qstatemessage.cpp
 *
 *  @author   Jo2003
 *
 *  @date     05.08.2014
 *
 *  $Id$
 *
 *///------------------------- (c) 2014 by Jo2003  --------------------------
#include "qstatemessage.h"
#include "ui_qstatemessage.h"

//---------------------------------------------------------------------------
//
//! \brief   constructs a status message widget
//
//! \author  Jo2003
//! \date    05.08.2014
//
//! \param   parent [in] (QWidget*) pointer to parent widget
//! \param   f [in] (Qt::WindowFlags) fals for new widget
//
//---------------------------------------------------------------------------
QStateMessage::QStateMessage(QWidget* parent, Qt::WindowFlags f) :
   QFadeWidget(parent, f),
   ui(new Ui::QStateMessage)
{
   ui->setupUi(this);

   // don't steal focus when activated ...
   setAttribute(Qt::WA_ShowWithoutActivating);

   m_tHide.setSingleShot(true);
   m_tHide.setInterval(4000);

   connect (&m_tHide, SIGNAL(timeout()), this, SLOT(fadeOut()));

   setOpaque(1);
}

//---------------------------------------------------------------------------
//
//! \brief   destroys a status message widget
//
//! \author  Jo2003
//! \date    05.08.2014
//
//---------------------------------------------------------------------------
QStateMessage::~QStateMessage()
{
   delete ui;
}

//---------------------------------------------------------------------------
//
//! \brief   shows a status message [slot]
//
//! \author  Jo2003
//! \date    05.08.2014
//
//! \param   icon [in] (int) message type, cast to eIcon
//! \param   msg [in] (const QString&) message text
//! \param   iTmOut [in] (int) optional timeout value in ms
//
//---------------------------------------------------------------------------
void QStateMessage::showMessage(int icon, const QString &msg, int iTmOut)
{
   QIcon ico;

   // load pre-defined icons
   switch ((eIcon)icon)
   {
   case QStateMessage::S_INFO:
      ico.addFile(":/status/s_info");
      break;

   case QStateMessage::S_WARNING:
      ico.addFile(":/status/s_warn");
      break;

   case QStateMessage::S_ERROR:
      ico.addFile(":/status/s_error");
      break;

   default:
      break;
   }

   showMessage(ico, msg, iTmOut);
}

//---------------------------------------------------------------------------
//
//! \brief   shows a status message [slot]
//
//! \author  Jo2003
//! \date    05.08.2014
//
//! \param   icon [in] (const QIcon&) icon to display in widget
//! \param   msg [in] (const QString&) message text
//! \param   iTmOut [in] (int) optional timeout value in ms
//
//---------------------------------------------------------------------------
void QStateMessage::showMessage(const QIcon& icon, const QString &msg, int iTmOut)
{
   // get parent widget, needed for positioning
   QWidget *pParent = parentWidget();

   if (pParent != NULL)
   {
      // place icon ...
      ui->labIcon->setPixmap(icon.pixmap(40, 40));

      // set text ...
      ui->labMessage->setText(msg);

      // start hide timer ...
      if (iTmOut > 0)
      {
         m_tHide.start(iTmOut);
      }
      else
      {
         m_tHide.start();
      }

      // make positioning ...
      if (pParent != NULL)
      {
         QPoint x = pParent->mapToGlobal(QPoint(0, 0));
         int    w = pParent->width();
         int    h = pParent->height();

         // position at center of parent ...
         // x.setX(x.x() + w / 2 - width()  / 2);
         // x.setY(x.y() + h / 2 - height() / 2);

         // position at lower right of parent ...
         x.setX(x.x() + w - width()  - 50);
         x.setY(x.y() + h - height() - 50);

         move(x);
      }

      // start fade in ...
      fadeIn();
   }
}

//---------------------------------------------------------------------------
//
//! \brief   shows a status message [slot]
//
//! \author  Jo2003
//! \date    27.10.2014
//
//! \param   msg [in] (const QString&) message text
//
//---------------------------------------------------------------------------
void QStateMessage::showMessage(const QString &msg)
{
   showMessage((int)QStateMessage::S_INFO, msg);
}
