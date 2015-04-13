/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/csundukclnt.cpp $
 *
 *  @file     csundukclnt.cpp
 *
 *  @author   Jo2003
 *
 *  @date     12.12.2013
 *
 *  $Id: csundukclnt.cpp 1273 2013-12-19 09:25:39Z Olenka.Joerg $
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#include "csundukclnt.h"

// log file functions ...
extern CLogFile VlcLog;

//---------------------------------------------------------------------------
//
//! \brief   constructs CSundukClnt object
//
//! \author  Jo2003
//! \date    12.12.2013
//
//! \param   parent (QObject*) pointer to parent object
//
//! \return  --
//---------------------------------------------------------------------------
CSundukClnt::CSundukClnt(QObject *parent) : CKartinaClnt(parent)
{
   // nothing to do so far ...
}

//---------------------------------------------------------------------------
//
//! \brief   destroys CSundukClnt object
//
//! \author  Jo2003
//! \date    12.12.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
CSundukClnt::~CSundukClnt()
{
   // nothing to do so far ...
}

//---------------------------------------------------------------------------
//
//! \brief   get url for video stream
//
//! \author  Jo2003
//! \date    12.12.2013
//
//! \param   iChanID (int) channel id
//! \param   secCode (const QString &) protect code
//! \param   bTimerRec (bool) timer record flag
//
//! \return  --
//---------------------------------------------------------------------------
void CSundukClnt::GetStreamURL (int iChanID, const QString &secCode, bool bTimerRec)
{
   mInfo(tr("Request URL for channel %1 ...").arg(iChanID));

   QString req = QString("cid=%1&stream_protocol=hls").arg(iChanID);

   if (secCode != "")
   {
      req += QString("&protect_code=%1").arg(secCode);
   }

   q_post((bTimerRec) ? (int)CIptvDefs::REQ_TIMERREC : (int)CIptvDefs::REQ_STREAM,
          apiUrl() + "get_url", req);
}

//---------------------------------------------------------------------------
//
//! \brief   get url for archive stream
//
//! \author  Jo2003
//! \date    12.12.2013
//
//! \param   prepared (const QString &) prepared request
//! \param   secCode (const QString &) protect code
//
//! \return  --
//---------------------------------------------------------------------------
void CSundukClnt::GetArchivURL (const QString &prepared, const QString &secCode)
{
   mInfo(tr("Request Archiv URL ..."));

   QString req = QUrl::fromPercentEncoding(prepared.toUtf8());
   req += "&stream_protocol=hls";

   if (secCode != "")
   {
      req += QString("&protect_code=%1").arg(secCode);
   }

   q_post((int)CIptvDefs::REQ_ARCHIV, apiUrl() + "get_url", req);
}

//---------------------------------------------------------------------------
//
//! \brief   get url for vod stream
//
//! \author  Jo2003
//! \date    12.12.2013
//
//! \param   iVidId (int) video id
//! \param   secCode (const QString &) protect code
//
//! \return  --
//---------------------------------------------------------------------------
void CSundukClnt::GetVodUrl (int iVidId, const QString &secCode)
{
   mInfo(tr("Request Video Url for video %1...").arg(iVidId));

   QString req = QString("vod_geturl?fileid=%1&ad=1&stream_protocol=hls")
         .arg(iVidId);

   if (secCode != "")
   {
      req += QString("&protect_code=%1").arg(secCode);
   }

   q_get((int)CIptvDefs::REQ_GETVODURL, apiUrl() + req);
}
