/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qnoidleproc.cpp
 *
 *  @author   Jo2003
 *
 *  @date     26.07.2012
 *
 *  $Id$
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
   QProcess(parent)
{
   connect (this, SIGNAL(stateChanged(QProcess::ProcessState)),
            this, SLOT(slotStateChanged(QProcess::ProcessState)));
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
//! \brief   start pmset with option "noidle"
//
//! \author  Jo2003
//! \date    26.07.2012
//
//---------------------------------------------------------------------------
void QNoIdleProc::startNoIdle()
{
   if (state() == QProcess::NotRunning)
   {
      start("pmset noidle");
   }
}

//---------------------------------------------------------------------------
//
//! \brief   terminate pmset (if running)
//
//! \author  Jo2003
//! \date    26.07.2012
//
//---------------------------------------------------------------------------
void QNoIdleProc::endNoIdle()
{
   if (state() != QProcess::NotRunning)
   {
      terminate();
   }
}

//---------------------------------------------------------------------------
//
//! \brief   log state change of pmset process
//
//! \author  Jo2003
//! \date    26.07.2012
//
//! \param   newState new process state
//
//---------------------------------------------------------------------------
void QNoIdleProc::slotStateChanged(QProcess::ProcessState newState)
{
#define mkProcState(__x__) case __x__: mInfo(tr("NoIdleProc state change to %1").arg(#__x__)); break

   switch (newState)
   {
   mkProcState(QProcess::NotRunning);
   mkProcState(QProcess::Starting);
   mkProcState(QProcess::Running);
   default:
      break;
   }

#undef mkProcState
}
