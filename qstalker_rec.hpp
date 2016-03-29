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
    mInfo(tr("Before parser: %1").arg(str));

    // parse cookie ...
    if (!pApiParser->parseAuth(str, mAuth))
    {
        pApiClient->SetCookie(QString("Bearer %1").arg(mAuth.token));
        pApiClient->setAuthData(mAuth);
        pApiClient->queueRequest(CIptvDefs::REQ_USER);
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
    if (!pApiParser->parseUserSettings(resp, mStalkSet))
    {
        pApiClient->queueRequest(CIptvDefs::REQ_CHANNELLIST);
    }
}

//---------------------------------------------------------------------------
//
//! \brief   got session renew response
//
//! \author  Jo2003
//! \date    29.03.2016
//
//! \param   str [in] (const QString&) response string
//
//---------------------------------------------------------------------------
void Recorder::sessionRenew (const QString& str)
{
    // parse cookie ...
    if (!pApiParser->parseAuth(str, mAuth))
    {
        pApiClient->SetCookie(QString("Bearer %1").arg(mAuth.token));
        pApiClient->setAuthData(mAuth);
    }
}
