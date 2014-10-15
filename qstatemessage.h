/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qstatemessage.h
 *
 *  @author   Jo2003
 *
 *  @date     05.08.2014
 *
 *  $Id$
 *
 *///------------------------- (c) 2014 by Jo2003  --------------------------
#ifndef __20140805_QSTATEMESSAGE_H
   #define __20140805_QSTATEMESSAGE_H

#include <QWidget>
#include <QTimer>
#include <QString>
#include <QIcon>

#include "qfadewidget.h"

// ---------------------------------------------------------------------------
// UI namespace
// ---------------------------------------------------------------------------
namespace Ui {
   class QStateMessage;
}

/*
/// \brief various states have variuos icons
enum class StateMsg : uint8_t {
   INFO,
   WARNING,
   ERROR,
   UNKNOWN,
   // to be continued ...
};
*/
//---------------------------------------------------------------------------
//! \class   QStateMessage
//! \date    05.08.2014
//! \author  Jo2003
//! \brief   a status message widget which can fade in and -out
//---------------------------------------------------------------------------
class QStateMessage : public QFadeWidget
{
   Q_OBJECT

public:

   /// \brief various states have variuos icons
   enum eIcon {
      S_INFO,       ///< information only
      S_WARNING,    ///< warning
      S_ERROR,      ///< error
      S_UNKNOWN     ///< unknown or unsupported
   };

   QStateMessage(QWidget* parent = 0, Qt::WindowFlags f = 0);
   ~QStateMessage();

public slots:
   void showMessage (int icon, const QString& msg, int iTmOut = -1);
   void showMessage (const QIcon &icon, const QString& msg, int iTmOut = -1);

private:
   Ui::QStateMessage *ui;
   QTimer m_tHide;
};

#endif // __20140805_QSTATEMESSAGE_H
