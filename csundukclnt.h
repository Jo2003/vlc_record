/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     csundukclnt.h
 *
 *  @author   Jo2003
 *
 *  @date     12.12.2013
 *
 *  $Id$
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#ifndef __20131212_CSUNDUKCLNT_H
   #define __20131212_CSUNDUKCLNT_H

#include "ckartinaclnt.h"
#include <QObject>

//---------------------------------------------------------------------------
//! \class   CSundukClnt
//! \date    12.12.2013
//! \author  Jo2003
//! \brief   subclass kartina client to support sunduk special handling
//---------------------------------------------------------------------------
class CSundukClnt : public CKartinaClnt
{
   Q_OBJECT

public:
   CSundukClnt(QObject *parent);
   virtual ~CSundukClnt();

protected:
   virtual void GetStreamURL (int iChanID, const QString &secCode, bool bTimerRec = false);
   virtual void GetArchivURL (const QString &prepared, const QString &secCode);
   virtual void GetVodUrl (int iVidId, const QString &secCode);
};

#endif // __20131212_CSUNDUKCLNT_H
