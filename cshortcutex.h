/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 22.03.2010 / 08:45:22
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#ifndef __032410__CMYSHORTCUT_H
   #define __032410__CMYSHORTCUT_H

#include <QShortcut>
#include <QString>

/*=============================================================================\
| Class:       CShortcutEx
|
| Author:      Jo2003
|
| Begin:       24.03.2010 / 11:47:02
|
| Description: extends QShortcut with activate function
|              (trigger activated signal)
|
\=============================================================================*/
class CShortcutEx : public QShortcut
{
   Q_OBJECT

public:
   // constuctors ...
   CShortcutEx(QWidget * parent) : QShortcut(parent)
   {
      // nothing to do so far ...
   }

   CShortcutEx (const QKeySequence & key, QWidget * parent,
                const char * member = 0, const char * ambiguousMember = 0,
                Qt::ShortcutContext context = Qt::WindowShortcut)
                   : QShortcut (key, parent, member, ambiguousMember, context)
   {
      // nothing to do so far ...
   }

   // destructor ...
   virtual ~CShortcutEx()
   {
      // nothing to do so far ...
   }

   /* -----------------------------------------------------------------\
   |  Method: createShortcutString [static]
   |  Begin: 24.03.2010 / 13:26:10
   |  Author: Jo2003
   |  Description: create shortcut string from modifier and key text
   |
   |  Parameters: modifier flags, key text, buffer for shortcut
   |
   |  Returns: 0 --> ok
   |          -1 --> can't create shortcut
   \----------------------------------------------------------------- */
   static int createShortcutString (const Qt::KeyboardModifiers &flags,
                                    const QString &key, QString &sBuf)
   {
      int iRV = -1;
      sBuf    = "";

      // is this a "normal" ascii key?
      if (key != "")
      {
         // add modifier to shortcut  ...
         if (flags & Qt::AltModifier)       // ALT+ ...
         {
            sBuf += QString("ALT+");
         }

         if (flags & Qt::ControlModifier)  // CTRL+ ...
         {
            sBuf += QString("CTRL+");
         }

         if (flags & Qt::ShiftModifier)    // CTRL+ ...
         {
            sBuf += QString("SHIFT+");
         }

         if (flags & Qt::MetaModifier)     // META+ ...
         {
            sBuf += QString("META+");
         }

         // any modifier added to shortcut ... ?
         if (sBuf != "")
         {
            sBuf += key.toUpper();

            // all is well ...
            iRV = 0;
         }
      }

      return iRV;
   }


public slots:

   /* -----------------------------------------------------------------\
   |  Method: activate
   |  Begin: 24.03.2010 / 13:28:10
   |  Author: Jo2003
   |  Description: trigger activated signal
   |
   |  Parameters: --
   |
   |  Returns: --
   \----------------------------------------------------------------- */
   void activate ()
   {
      emit activated ();
   }
};

#endif // __032410__CMYSHORTCUT_H
/************************* History ***************************\
| $Log$
\*************************************************************/
