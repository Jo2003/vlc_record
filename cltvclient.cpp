/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     cltvclient.h
 *
 *  @author   Jo2003
 *
 *  @date     16.07.2013
 *
 *  $Id$
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#include "cltvclient.h"

// log file functions ...
extern CLogFile VlcLog;

//---------------------------------------------------------------------------
//
//! \brief   construct CLtvClient object
//
//! \author  Jo2003
//! \date    16.07.2013
//
//! \param   parent pointer to parent widget
//
//! \return  --
//---------------------------------------------------------------------------
CLtvClient::CLtvClient(QObject *parent) : CKartinaClnt(parent)
{
   setObjectName("CLtvClient");
}

//---------------------------------------------------------------------------
//
//! \brief   destroy CLtvClient object
//
//! \author  Jo2003
//! \date    16.07.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
CLtvClient::~CLtvClient()
{
   // nothing to do so far ...
}

//---------------------------------------------------------------------------
//
//! \brief   handle string response from API server
//
//! \author  Jo2003
//! \date    15.03.2013
//
//! \param   reqId (int) request identifier
//! \param   strResp (QString) response string
//
//! \return  --
//---------------------------------------------------------------------------
void CLtvClient::slotStringResponse (int reqId, QString strResp)
{
   if (reqId == (int)CIptvDefs::REQ_COOKIE)
   {
      // fake additional settings here ...
      mInfo(tr("Fake account server settings ..."));

      QString sPatch = "<settings><timeshift><value>0</value><list><item>0</item></list></timeshift>"
        "<timezone><value>0</value></timezone>"
        "<stream_server><value>111.111.111.111</value><list><item><ip>111.111.111.111</ip><descr>Standard</descr></item></list></stream_server>"
        "<bitrate><value>1500</value><list><item>1500</item></list></bitrate></settings>";

      int iPos;

      if ((iPos = strResp.indexOf("<servertime>")) != -1)
      {
         strResp.insert(iPos, sPatch);
      }
   }

   CKartinaClnt::slotStringResponse(reqId, strResp);
}
