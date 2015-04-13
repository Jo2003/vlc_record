/*********************** Information *************************\
| $HeadURL: https://vlc-record.googlecode.com/svn/branches/sunduk.tv/cfavaction.h $
|
| Author: Jo2003
|
| Begin: 18.01.2010 / 16:16:25
|
| Last edited by: $Author: Olenka.Joerg $
|
| $Id: cfavaction.h 90 2010-02-26 13:32:14Z Olenka.Joerg $
\*************************************************************/
#ifndef __022510__CFAVACTION_H
   #define __022510__CFAVACTION_H

#include <QAction>

// -----------------------------------------------------------------------------
// namespace
// -----------------------------------------------------------------------------
namespace kartinafav
{
   class CFavAction;
   enum eAct
   {
      FAV_ADD,
      FAV_DEL,
      FAV_WHAT = 255
   };
}

/********************************************************************\
|  Class: CFavAction
|  Date:  25.02.2010 / 10:45:59
|  Author: Jo2003
|  Description: customized QAction for favourite handling
|
\********************************************************************/
class CFavAction : public QAction
{
public:
   CFavAction (QObject * parent) : QAction(parent)
   {
      iCid   = -1;
      action = kartinafav::FAV_WHAT;
   }

   CFavAction (const QString & text, QObject * parent) : QAction(text, parent)
   {
      iCid   = -1;
      action = kartinafav::FAV_WHAT;
   }

   CFavAction (const QIcon & icon, const QString & text, QObject * parent)
      : QAction (icon, text, parent)
   {
      iCid   = -1;
      action = kartinafav::FAV_WHAT;
   }

   virtual ~CFavAction ()
   {
      // nothing to do ...
   }

   void setFavData (int cid, kartinafav::eAct act)
   {
      iCid   = cid;
      action = act;
   }

   void favData (int &cid, kartinafav::eAct &act)
   {
      cid  = iCid;
      act  = action;
   }

private:
   int              iCid;
   kartinafav::eAct action;
};

#endif /* __022510__CFAVACTION_H */
/************************* History ***************************\
| $Log$
\*************************************************************/

