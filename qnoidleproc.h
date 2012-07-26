/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qnoidleproc.h
 *
 *  @author   Jo2003
 *
 *  @date     26.07.2012
 *
 *  $Id$
 *
 *///------------------------- (c) 2012 by Jo2003  --------------------------
#ifndef __20120726_QNOIDLEPROC_H
   #define __20120726_QNOIDLEPROC_H

#include <QProcess>
#include "clogfile.h"

//---------------------------------------------------------------------------
//! \class   QNoIdleProc
//! \date    26.07.2012
//! \author  Jo2003
//! \brief   Mac only - disable idle as long as started
//---------------------------------------------------------------------------
class QNoIdleProc : public QProcess
{
   Q_OBJECT

public:
   QNoIdleProc(QObject *parent = 0);
   virtual ~QNoIdleProc ();

signals:

public slots:
   void startNoIdle();
   void endNoIdle();

private slots:
   void slotStateChanged (QProcess::ProcessState newState);
};

#endif // __20120726_QNOIDLEPROC_H
