/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/sunduk.tv/csundukclnt.cpp $
 *
 *  @file     csundukclnt.cpp
 *
 *  @author   Jo2003
 *
 *  @date     12.12.2013
 *
 *  $Id: csundukclnt.cpp 1509 2015-03-04 11:26:51Z Olenka.Joerg $
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#include "csundukclnt.h"
#include "qcustparser.h"


// global customization class ...
extern QCustParser *pCustomization;

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
   sStrProto = "hls";
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
//! \brief   overload queueRequest function
//
//! \author  Jo2003
//! \date    17.02.2015
//
//! \param   [in] req (CIptvDefs::EReq) request type
//! \param   [in] par_1 (const QVariant&) first parameter
//! \param   [in] par_2 (const QVariant&) second parameter
//
//! \return  0 -> ok; -1 -> error
//---------------------------------------------------------------------------
int CSundukClnt::queueRequest(CIptvDefs::EReq req, const QVariant& par_1, const QVariant& par_2)
{
   int iRet;

   // handled in parent class ... ?
   if ((iRet = CKartinaClnt::queueRequest(req, par_1, par_2)) < 0)
   {
      switch (req)
      {
      case CIptvDefs::REQ_GETVODURL_EX:
         GetVodUrl(par_1.toUrl());
         break;

      case CIptvDefs::REQ_GET_VODTYPES:
         GetVodTypes();
         break;

      default:
         iRet = -1;
         break;
      }
   }

   return iRet;
}

//---------------------------------------------------------------------------
//
//! \brief   login
//
//! \author  Jo2003
//! \date    22.11.2014
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void CSundukClnt::GetCookie()
{
   mInfo(tr("Request Authentication ..."));

   q_post((int)CIptvDefs::REQ_COOKIE, apiUrl() + "login",
        QString("login=%1&pass=%2&settings=all&softid=%3%4-%5&deviceId=%6")
            .arg(sUsr).arg(sPw)
            .arg(pCustomization->strVal("APPLICATION_SHORTCUT"))
            .arg(OP_SYS).arg(SOFTID_DEVELOPER)
            .arg(getStbSerial()),
        Iptv::Login);
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
   CIptvDefs::EReq ereq = bTimerRec ? CIptvDefs::REQ_TIMERREC : CIptvDefs::REQ_STREAM;

   QString req = QString("cid=%1&stream_protocol=%2").arg(iChanID).arg(getStreamProtocol(ereq));

   if (secCode != "")
   {
      req += QString("&protect_code=%1").arg(secCode);
   }

   q_post((int)ereq, apiUrl() + "get_url", req);
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
   req += "&stream_protocol=" + getStreamProtocol(CIptvDefs::REQ_ARCHIV);

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

   QString req = QString("vod_geturl?fileid=%1&ad=1&stream_protocol=%2")
         .arg(iVidId).arg(getStreamProtocol(CIptvDefs::REQ_GETVODURL));

   if (secCode != "")
   {
      req += QString("&protect_code=%1").arg(secCode);
   }

   q_get((int)CIptvDefs::REQ_GETVODURL, apiUrl() + req);
}

//---------------------------------------------------------------------------
//
//! \brief   get url for vod stream
//
//! \author  Jo2003
//! \date    12.12.2013
//
//! \param   [in] dst (const QUrl&) prepared query
//
//! \return  --
//---------------------------------------------------------------------------
void CSundukClnt::GetVodUrl (const QUrl& dst)
{
   mInfo(tr("Request Video Url for url '%1' ...").arg(dst.toString()));

   QString req = QString("vod_geturl?fileid=%1&ad=1&stream_protocol=%2")
         .arg(dst.queryItemValue("vid")).arg(getStreamProtocol(CIptvDefs::REQ_GETVODURL));

   if (dst.hasEncodedQueryItem("format"))
   {
      req += QString("&format=%1").arg(dst.queryItemValue("format"));
   }

   if (dst.queryItemValue("seccode") != "")
   {
      req += QString("&protect_code=%1").arg(dst.queryItemValue("seccode"));
   }

   q_get((int)CIptvDefs::REQ_GETVODURL, apiUrl() + req);
}

//---------------------------------------------------------------------------
//
//! \brief   get vod types
//
//! \author  Jo2003
//! \date    04.03.2015
//
//---------------------------------------------------------------------------
void CSundukClnt::GetVodTypes()
{
   mInfo(tr("Request VOD Types..."));
   q_get((int)CIptvDefs::REQ_GET_VODTYPES, apiUrl() + "vod_types");
}

//---------------------------------------------------------------------------
//
//! \brief   get stream protocol string
//
//! \author  Jo2003
//! \date    25.02.2015
//
//! \param   [in] req (CIptvDefs::EReq) request type
//
//! \return  stream protocol string
//---------------------------------------------------------------------------
QString CSundukClnt::getStreamProtocol(CIptvDefs::EReq req)
{
   // to support hls and mpegts within one setup
   // we created the string "mpegts / hls" as option.
   QString ret = sStrProto;

   if (sStrProto.contains("/"))
   {
      QStringList sl = sStrProto.split("/");

      switch(req)
      {
      case CIptvDefs::REQ_STREAM:
      case CIptvDefs::REQ_TIMERREC:
      case CIptvDefs::REQ_ARCHIV:
         // index 0: mpegts
         ret = sl.at(0).trimmed();
         break;

      default:
         // index 1: hls
         ret = sl.at(1).trimmed();
         break;
      }
   }

   return ret;
}

//---------------------------------------------------------------------------
//
//! \brief   set stream protocol
//
//! \author  Jo2003
//! \date    22.11.2014
//
//! \param   p (QString) new protocol
//
//! \return  --
//---------------------------------------------------------------------------
void CSundukClnt::slotStrProto(QString p)
{
   sStrProto = p;
}
