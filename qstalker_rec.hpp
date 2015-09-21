/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qstalker_rec.hpp
 *
 *  @author   Jo2003
 *
 *  @date     21.09.2015
 *
 *  $Id$
 *
 *///------------------------- (c) 2015 by Jo2003  --------------------------

// since this is included already in the real code file,
// it will be not included again (include guard).
#include "recorder.h"

//---------------------------------------------------------------------------
//
//! \brief   got cookie response
//
//! \author  Jo2003
//! \date    21.09.2015
//
//! \param   str [in] (const QString&) response string
//
//---------------------------------------------------------------------------
void Recorder::slotCookie (const QString &str)
{
    QString s;

    // parse cookie ...
    if (!pApiParser->parseAuth(str, mAuth))
    {
        pApiClient->queueRequest(CIptvDefs::REQ_USER, mAuth.userId);
    }
}

//---------------------------------------------------------------------------
//
//! \brief   got user settings response
//
//! \author  Jo2003
//! \date    21.09.2015
//
//! \param   resp [in] (const QString&) response string
//
//---------------------------------------------------------------------------
void Recorder::userData (const QString& resp)
{
    mInfo(tr("Got user settings! %1").arg(resp));
}
