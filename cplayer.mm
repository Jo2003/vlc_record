/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     cplayer.mm
 *
 *  @author   Jo2003
 *
 *  @date     07.08.2012
 *
 *  $Id$
 *
 *///------------------------- (c) 2012 by Jo2003  --------------------------

#include "cplayer.hpp"
#include "cplayer.cpp"

/* -----------------------------------------------------------------\
|  Method: setNoIdle
|  Begin: 06.08.2012
|  Author: Jo2003
|  Description: enable / disable no-idle
|
|  Parameters: bOn == true -> enable, else disable
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayer::setNoIdle(bool bOn)
{
   if (bOn != bNoIdleOn)
   {
      if (bOn)
      {
         if (IOPMAssertionCreateWithName(kIOPMAssertionTypeNoDisplaySleep,
                                         kIOPMAssertionLevelOn,
                                         CFSTR("Playing a stream!"),
                                         &assertionID) == kIOReturnSuccess)
         {
            mInfo(tr("No Idle successful initiated!"));
            bNoIdleOn = true;
         }
      }
      else
      {
         if (IOPMAssertionRelease(assertionID) == kIOReturnSuccess)
         {
            mInfo(tr("No Idle successful finished!"));
            bNoIdleOn = false;
         }
      }
   }
}
