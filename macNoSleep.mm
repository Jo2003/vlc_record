/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     macNoSleep.mm
 *
 *  @author   Jo2003
 *
 *  @date     01.06.2015
 *
 *  $Id$
 *
 *///------------------------- (c) 2015 by Jo2003  --------------------------
#include "macNoSleep.h"

#import <IOKit/pwr_mgt/IOPMLib.h>


// global variables
static IOPMAssertionID assertionID;
static bool bNoSleepActive = false;

// give a reason for no sleep mode
static const __CFString* reasonForActivity = CFSTR("VLC-Record plays video");

//---------------------------------------------------------------------------
//
//! \brief   put Mac to no sleep mode
//
//! \author  Jo2003
//! \date    01.06.2015
//
//! \return  0 -> ok, -1 -> error
//---------------------------------------------------------------------------
int macNoSleep()
{
   int iRet    = -1;

   if (!bNoSleepActive)
   {
      if (IOPMAssertionCreateWithName(kIOPMAssertionTypeNoDisplaySleep,
                                      kIOPMAssertionLevelOn,
                                      reasonForActivity,
                                      &assertionID) == kIOReturnSuccess)
      {
         iRet           = 0;
         bNoSleepActive = true;
      }
   }
   else
   {
      iRet = 0;
   }

   return iRet;
}

//---------------------------------------------------------------------------
//
//! \brief   stop no sleep mode
//
//! \author  Jo2003
//! \date    01.06.2015
//
//! \return  0 -> ok, -1 -> error
//---------------------------------------------------------------------------
int macSleep()
{
   int iRet    = -1;

   if (bNoSleepActive)
   {
      if (IOPMAssertionRelease(assertionID) == kIOReturnSuccess)
      {
         iRet = 0;
         bNoSleepActive = false;
      }
   }
   else
   {
      iRet = 0;
   }

   return iRet;
}

//---------------------------------------------------------------------------
//
//! \brief   check if no sleep mode is active
//
//! \author  Jo2003
//! \date    01.06.2015
//
//! \return  1 -> active; 0 -> not active
//---------------------------------------------------------------------------
int macNoSleepActive()
{
   return bNoSleepActive ? 1 : 0;
}
