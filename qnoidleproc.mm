/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/trunk/vlc-record/qnoidleproc.cpp $
 *
 *  @file     qnoidleproc.cpp
 *
 *  @author   Jo2003
 *
 *  @date     26.07.2012
 *
 *  $Id: qnoidleproc.cpp 836 2012-07-26 09:48:01Z Olenka.Joerg@gmail.com $
 *
 *///------------------------- (c) 2012 by Jo2003  --------------------------

#include "qnoidleproc.h"

// for logging ...
extern CLogFile VlcLog;

//---------------------------------------------------------------------------
//
//! \brief   constructs QNoIdleProc object
//
//! \author  Jo2003
//! \date    26.07.2012
//
//! \param   parent pointer to parent object
//
//---------------------------------------------------------------------------
QNoIdleProc::QNoIdleProc(QObject *parent) :
   QObject(parent), bNoIdleOn(false)
{
   reasonForActivity = CFSTR("Playing a stream!");
}

//---------------------------------------------------------------------------
//
//! \brief   destroys QNoIdleProc object
//
//! \author  Jo2003
//! \date    26.07.2012
//
//---------------------------------------------------------------------------
QNoIdleProc::~QNoIdleProc()
{
   endNoIdle();
}

//---------------------------------------------------------------------------
//
//! \brief   start "noidle" in kernel
//
//! \author  Jo2003
//! \date    26.07.2012
//
//---------------------------------------------------------------------------
void QNoIdleProc::startNoIdle()
{
   if (!bNoIdleOn)
   {
      if (IOPMAssertionCreateWithName(kIOPMAssertionTypeNoDisplaySleep,
                                      kIOPMAssertionLevelOn,
                                      reasonForActivity,
                                      &assertionID) == kIOReturnSuccess)
      {
         mInfo(tr("No Idle successful initiated!"));
         bNoIdleOn = true;
      }
   }
}

//---------------------------------------------------------------------------
//
//! \brief   stop "nodile" in kernel
//
//! \author  Jo2003
//! \date    26.07.2012
//
//---------------------------------------------------------------------------
void QNoIdleProc::endNoIdle()
{
   if (bNoIdleOn)
   {
      if (IOPMAssertionRelease(assertionID) == kIOReturnSuccess)
      {
         mInfo(tr("No Idle successful finished!"));
         bNoIdleOn = false;
      }
   }
}
