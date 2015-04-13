/*********************** Information *************************\
| $HeadURL: https://vlc-record.googlecode.com/svn/branches/sunduk.tv/cshortcutex.h $
|
| Author: Jo2003
|
| Begin: 22.03.2010 / 08:45:22
|
| Last edited by: $Author: Olenka.Joerg $
|
| $Id: cshortcutex.h 447 2011-03-25 11:04:08Z Olenka.Joerg $
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
