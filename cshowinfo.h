/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 15.06.2010 / 16:41:34
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#ifndef __150610_CSHOWINFO_H
   #define __150610_CSHOWINFO_H

#include <QString>
#include "playstates.h"

/********************************************************************\
|  Class: CShowInfo
|  Date:  15.06.2010 / 14:02:24
|  Author: Jo2003
|  Description: a simple storage class for show information
|
\********************************************************************/
class CShowInfo
{
public:

   // constructor ...
   CShowInfo();

   // save values ...
   void setShowName (const QString &name);
   void setChanName (const QString &name);
   void setChanId (int id);
   void setArchive (bool arch);
   void setStartTime (uint start);
   void setEndTime (uint end);
   void setPlayState (IncPlay::ePlayStates state);
   void setLastJumpTime (int start);

   // get values ...
   const QString &showName();
   const QString &chanName();
   const int &channelId();
   const uint &starts();
   const uint &ends();
   const int &lastJump();
   const IncPlay::ePlayStates &playState();
   const bool &archive();

private:
   QString              sShowName;
   int                  iChannelId;
   QString              sChanName;
   bool                 bIsArchive;
   uint                 uiStart;
   uint                 uiEnd;
   int                  iJumpTime;
   IncPlay::ePlayStates ePlayState;
};

#endif // __150610_CSHOWINFO_H
/************************* History ***************************\
| $Log$
\*************************************************************/
