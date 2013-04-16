/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     cstdjsonparser.cpp
 *
 *  @author   Jo2003
 *
 *  @date     15.04.2013
 *
 *  $Id$
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#include "cstdjsonparser.h"

// log file functions ...
extern CLogFile VlcLog;

//---------------------------------------------------------------------------
//
//! \brief   construct json parser
//
//! \author  Jo2003
//! \date    15.04.2013
//
//! \param   parent pointer to parent widget
//
//! \return  --
//---------------------------------------------------------------------------
CStdJsonParser::CStdJsonParser(QObject * parent) : QObject(parent)
{
   iOffset    = 0;
}

//---------------------------------------------------------------------------
//
//! \brief   check time offset between server and local time
//
//! \author  Jo2003
//! \date    15.04.2013
//
//! \param   uiSrvTime (const uint &) ref. to server time
//
//! \return  --
//---------------------------------------------------------------------------
void CStdJsonParser::checkTimeOffSet (const uint &uiSrvTime)
{
   /// Note:
   /// This function is a little tricky ...
   /// Try to find out the real difference between the
   /// time kartina.tv assumes for us and the real time
   /// running on this machine ...
   /// Round offset to full 30 minutes (min. timezone step)

   // get difference between kartina.tv and our time ...
   int iOffSec    = (int)(QDateTime::currentDateTime().toTime_t() - uiSrvTime);

   // round offset to full timezone step ...
   int iHalfHours = qRound ((double)iOffSec / (double)DEF_TZ_STEP);

   if (iHalfHours)
   {
      iOffset = iHalfHours * DEF_TZ_STEP;
   }
   else
   {
      iOffset = 0;
   }

   if (iOffset)
   {
      mInfo(tr("Set time offset to %1 seconds!").arg(iOffset));
   }
}

//---------------------------------------------------------------------------
//
//! \brief   fix time in channel list as sent from server
//
//! \author  Jo2003
//! \date    15.04.2013
//
//! \param   uiSrvTime (uint &) ref. to time stamp
//
//! \return  0 --> unchanged; 1 --> fixed
//---------------------------------------------------------------------------
int CStdJsonParser::fixTime (uint &uiTime)
{
   if (iOffset)
   {
      // add offset ...
      // note that offset can be negative ...
      uiTime += iOffset;

      return 1;
   }

   return 0;
}

//---------------------------------------------------------------------------
//
//! \brief   init channel entry
//
//! \author  Jo2003
//! \date    15.04.2013
//
//! \param   entry (cparser::SChan &) ref. to channel entry
//! \param   bIsChan (bool) flag distinguish channel and group
//
//! \return  --
//---------------------------------------------------------------------------
void CStdJsonParser::initChanEntry(cparser::SChan &entry, bool bIsChan)
{
   // new item starts --> init chan struct ...
   entry.bHasArchive  = false;
   entry.bIsGroup     = (bIsChan) ? false : true;
   entry.bIsProtected = false;
   entry.bIsVideo     = true;
   entry.iId          = -1;
   entry.sIcon        = "";
   entry.sName        = "";
   entry.sProgramm    = "";
   entry.uiEnd        = 0;
   entry.uiStart      = 0;
   entry.bIsHidden    = false;
   entry.vTs.clear();
}

//---------------------------------------------------------------------------
//
//! \brief   parse channel list response into struct vector
//
//! \author  Jo2003
//! \date    15.04.2013
//
//! \param   sResp (const QString &) ref. to response string
//! \param   chanList (QVector<cparser::SChan> &) data vector
//! \param   bFixTime (bool) flag for time correction
//
//! \return  0 --> ok; -1 --> any error
//---------------------------------------------------------------------------
int CStdJsonParser::parseChannelList (const QString &sResp,
                                         QVector<cparser::SChan> &chanList,
                                         bool bFixTime)
{
   int  iRV = 0;
   bool bOk = false;
   cparser::SChan      chan;
   cparser::STimeShift ts;
   QVariantMap   contentMap;
   QJson::Parser parser;

   // clear channel list ...
   chanList.clear();

   contentMap = parser.parse(sResp.toUtf8(), &bOk).toMap();

   if (bOk)
   {
      foreach (const QVariant& lGroup, contentMap.value("groups").toList())
      {
         QVariantMap mGroup = lGroup.toMap();

         initChanEntry(chan, false);

         chan.iId       = mGroup.value("id").toInt();
         chan.sName     = mGroup.value("name").toString();
         chan.sProgramm = mGroup.value("color").toString();

         chanList.append(chan);

         foreach (const QVariant& lChannel, mGroup.value("channels").toList())
         {
            QVariantMap mChannel = lChannel.toMap();

            initChanEntry(chan);

            chan.iId          = mChannel.value("id").toInt();
            chan.sName        = mChannel.value("name").toString();
            chan.bIsVideo     = mChannel.value("is_video").toBool();
            chan.bHasArchive  = mChannel.value("have_archive").toBool();
            chan.bIsProtected = mChannel.value("protected").toBool();
            chan.sIcon        = mChannel.value("icon").toString();
            chan.sProgramm    = mChannel.value("epg_progname").toString();
            chan.uiStart      = mChannel.value("epg_start").toUInt();
            chan.uiEnd        = mChannel.value("epg_end").toUInt();
            chan.bIsHidden    = mChannel.value("hide").toBool();

            if (bFixTime)
            {
               fixTime(chan.uiStart);
               fixTime(chan.uiEnd);
            }

            foreach (const QVariant& lParam, mChannel.value("stream_params").toList())
            {
               QVariantMap mParam = lParam.toMap();

               ts.iBitRate   = mParam.value("rate").toInt();
               ts.iTimeShift = mParam.value("ts").toInt();

               chan.vTs.append(ts);
            }

            chanList.append(chan);
         }
      }
   }
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("Error QJSON can't parse respone!"));

      iRV = -1;
   }

   return iRV;
}

//---------------------------------------------------------------------------
//
//! \brief   parse server list response
//
//! \author  Jo2003
//! \date    15.04.2013
//
//! \param   sResp (const QString &) ref. to response string
//! \param   vSrv (QVector<cparser::SSrv> &) server vector
//! \param   sActIp (QString&) ref. to current value
//
//! \return  0 --> ok; -1 --> any error
//---------------------------------------------------------------------------
int CStdJsonParser::parseSServersLogin(const QString &sResp, QVector<cparser::SSrv> &vSrv,
                                     QString &sActIp)
{
   int  iRV = 0;
   bool bOk = false;
   cparser::SSrv   srv;
   QVariantMap     contentMap, nestedMap;
   QJson::Parser   parser;

   // clear server list ...
   vSrv.clear();

   contentMap = parser.parse(sResp.toUtf8(), &bOk).toMap();

   if (bOk)
   {
      nestedMap = contentMap.value("settings").toMap();
      nestedMap = nestedMap.value("stream_server").toMap();

      sActIp    = nestedMap.value("value").toString();

      foreach (const QVariant& lSrv, nestedMap.value("list").toList())
      {
         QVariantMap mSrv = lSrv.toMap();

         srv.sIp   = mSrv.value("ip").toString();
         srv.sName = mSrv.value("desc").toString();

         vSrv.append(srv);
      }
   }
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("Error QJSON can't parse respone!"));

      iRV = -1;
   }

   return iRV;
}

//---------------------------------------------------------------------------
//
//! \brief   parse cookie response
//
//! \author  Jo2003
//! \date    15.04.2013
//
//! \param   sResp (const QString &) ref. to response string
//! \param   sCookie (QString&) ref. to cookie string
//! \param   sInf (cparser::SAccountInfo &) ref. to account info
//
//! \return  0 --> ok; -1 --> any error
//---------------------------------------------------------------------------
int CStdJsonParser::parseCookie (const QString &sResp, QString &sCookie, cparser::SAccountInfo &sInf)
{
   int  iRV = 0;
   bool bOk = false;
   QVariantMap   contentMap, nestedMap;
   QJson::Parser parser;

   contentMap = parser.parse(sResp.toUtf8(), &bOk).toMap();

   if (bOk)
   {
      sCookie = QString("%1=%2")
            .arg(contentMap.value("sid_name").toString())
            .arg(contentMap.value("sid").toString());


      nestedMap        = contentMap.value("account").toMap();
      sInf.sExpires    = QDateTime::fromTime_t(nestedMap.value("packet_expire").toUInt())
                           .toString(DEF_TIME_FORMAT);

      nestedMap        = contentMap.value("services").toMap();
      sInf.bHasArchive = nestedMap.value("archive").toBool();
      sInf.bHasVOD     = nestedMap.value("vod").toBool();

      // check offset ...
      checkTimeOffSet (contentMap.value("servertime").toUInt());
   }
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("Error QJSON can't parse respone!"));

      iRV = -1;
   }

   return iRV;
}

//---------------------------------------------------------------------------
//
//! \brief   parse vod genres response
//
//! \author  Jo2003
//! \date    15.04.2013
//
//! \param   sResp (const QString &) ref. to response string
//! \param   vGenres (QVector<cparser::SGenre>&) ref. to genre vector
//
//! \return  0 --> ok; -1 --> any error
//---------------------------------------------------------------------------
int CStdJsonParser::parseGenres (const QString& sResp, QVector<cparser::SGenre>& vGenres)
{
   int  iRV = 0;
   bool bOk = false;
   cparser::SGenre sGenre;
   QVariantMap     contentMap;
   QJson::Parser   parser;

   // clear genres ...
   vGenres.clear();

   contentMap = parser.parse(sResp.toUtf8(), &bOk).toMap();

   if (bOk)
   {
      foreach (const QVariant& lGenre, contentMap.value("genres").toList())
      {
         QVariantMap mGenre = lGenre.toMap();

         sGenre.sGName = mGenre.value("name").toString();
         sGenre.uiGid  = mGenre.value("id").toUInt();

         vGenres.append(sGenre);
      }
   }
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("Error QJSON can't parse respone!"));

      iRV = -1;
   }

   return iRV;
}

//---------------------------------------------------------------------------
//
//! \brief   parse current epg current response
//
//! \author  Jo2003
//! \date    15.04.2013
//
//! \param   sResp (const QString &) ref. to response string
//! \param   currentEpg (QCurrentMap &) ref. to epg data map
//
//! \return  0 --> ok; -1 --> any error
//---------------------------------------------------------------------------
int CStdJsonParser::parseEpgCurrent (const QString& sResp, QCurrentMap &currentEpg)
{
   int  iRV = 0, cid;
   bool bOk = false;
   cparser::SEpgCurrent          entry;
   QVector<cparser::SEpgCurrent> vEntries;
   QVariantMap   contentMap;
   QJson::Parser parser;

   // clear map ...
   currentEpg.clear();

   contentMap = parser.parse(sResp.toUtf8(), &bOk).toMap();

   if (bOk)
   {
      foreach (const QVariant& lEpg1, contentMap.value("epg").toList())
      {
         QVariantMap mEpg1 = lEpg1.toMap();

         vEntries.clear();

         cid = mEpg1.value("cid").toInt();

         foreach (const QVariant& lEpg2, mEpg1.value("epg").toList())
         {
            QVariantMap mEpg2 = lEpg2.toMap();

            entry.sShow   = mEpg2.value("progname").toString();
            entry.uiStart = mEpg2.value("ts").toUInt();

            vEntries.append(entry);
         }

         currentEpg.insert(cid, vEntries);
      }
   }
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("Error QJSON can't parse respone!"));

      iRV = -1;
   }

   return iRV;
}

//---------------------------------------------------------------------------
//
//! \brief   parse Vod Manager data (genre rating)
//
//! \author  Jo2003
//! \date    15.04.2013
//
//! \param   sResp (const QString &) ref. to response string
//! \param   currentEpg (QVector<cparser::SVodRate> &) ref. to rates vector
//
//! \return  0 --> ok; -1 --> any error
//---------------------------------------------------------------------------
int CStdJsonParser::parseVodManager (const QString &sResp, QVector<cparser::SVodRate> &vRates)
{
   int  iRV = 0;
   bool bOk = false;
   cparser::SVodRate entry;
   QVariantMap   contentMap;
   QJson::Parser parser;

   // clear map ...
   vRates.clear();

   contentMap = parser.parse(sResp.toUtf8(), &bOk).toMap();

   if (bOk)
   {
      foreach (const QVariant& lAccess, contentMap.value("result").toList())
      {
         QVariantMap mAccess = lAccess.toMap();

         entry.iRateID = mAccess.value("id_rate").toInt();
         entry.sGenre  = mAccess.value("rate_name").toString();
         entry.sAccess = mAccess.value("action").toString();

         vRates.append(entry);
      }
   }
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("Error QJSON can't parse respone!"));

      iRV = -1;
   }

   return iRV;
}

//---------------------------------------------------------------------------
//
//! \brief   parse Vod list response
//
//! \author  Jo2003
//! \date    15.04.2013
//
//! \param   sResp (const QString &) ref. to response string
//! \param   vVodList (QVector<cparser::SVodVideo>&) ref. to vod vector
//! \param   gInfo (cparser::SGenreInfo&) ref. to genre info struct
//
//! \return  0 --> ok; -1 --> any error
//---------------------------------------------------------------------------
int CStdJsonParser::parseVodList(const QString &sResp, QVector<cparser::SVodVideo> &vVodList, cparser::SGenreInfo &gInfo)
{
   int  iRV = 0;
   bool bOk = false;
   cparser::SVodVideo entry;
   QVariantMap   contentMap;
   QJson::Parser parser;

   // clear vector ...
   vVodList.clear();

   contentMap = parser.parse(sResp.toUtf8(), &bOk).toMap();

   if (bOk)
   {
      gInfo.sType  = contentMap.value("type").toString();
      gInfo.iTotal = contentMap.value("total").toInt();
      gInfo.iPage  = contentMap.value("page").toInt();
      gInfo.iCount = contentMap.value("count").toInt();

      foreach (const QVariant& lRow, contentMap.value("rows").toList())
      {
         QVariantMap mRow = lRow.toMap();

         entry.uiVidId    = mRow.value("id").toUInt();
         entry.sName      = mRow.value("name").toString();
         entry.sDescr     = mRow.value("description").toString();
         entry.sYear      = mRow.value("year").toString();
         entry.sCountry   = mRow.value("country").toString();
         entry.sImg       = mRow.value("poster").toString();
         entry.bProtected = mRow.value("pass_protect").toBool();
         entry.bFavourit  = mRow.value("favorite").toBool();

         vVodList.append(entry);
      }
   }
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("Error QJSON can't parse respone!"));

      iRV = -1;
   }

   return iRV;
}

//---------------------------------------------------------------------------
//
//! \brief   parse video detail response
//
//! \author  Jo2003
//! \date    15.04.2013
//
//! \param   sResp (const QString &) ref. to response string
//! \param   vidInfo (cparser::SVodVideo&) ref. to vod info struct
//
//! \return  0 --> ok; -1 --> any error
//---------------------------------------------------------------------------
int CStdJsonParser::parseVideoInfo(const QString &sResp, cparser::SVodVideo &vidInfo)
{
   int  iRV = 0;
   bool bOk = false;
   QVariantMap   contentMap;
   QJson::Parser parser;
   cparser::SVodFileInfo fInfo;

   // init struct ...
   vidInfo.sActors    = "";
   vidInfo.sCountry   = "";
   vidInfo.sDescr     = "";
   vidInfo.sDirector  = "";
   vidInfo.sImg       = "";
   vidInfo.sName      = "";
   vidInfo.sYear      = "";
   vidInfo.uiLength   = 0;
   vidInfo.uiVidId    = 0;
   vidInfo.bProtected = false;
   vidInfo.bFavourit  = false;
   vidInfo.vVodFiles.clear();

   contentMap = parser.parse(sResp.toUtf8(), &bOk).toMap();

   if (bOk)
   {
      contentMap = contentMap.value("film").toMap();

      vidInfo.sActors   = contentMap.value("actors").toString();
      vidInfo.sCountry  = contentMap.value("country").toString();
      vidInfo.sDescr    = contentMap.value("description").toString();
      vidInfo.sDirector = contentMap.value("director").toString();
      vidInfo.sImg      = contentMap.value("poster").toString();
      vidInfo.sName     = contentMap.value("name").toString();
      vidInfo.sYear     = contentMap.value("year").toString();
      vidInfo.sGenres   = contentMap.value("genre_str").toString();
      vidInfo.uiLength  = contentMap.value("lenght").toUInt();
      vidInfo.uiVidId   = contentMap.value("id").toUInt();

      foreach (const QVariant& lVideo, contentMap.value("videos").toList())
      {
         QVariantMap mVideo = lVideo.toMap();

         fInfo.iHeight = mVideo.value("height").toInt();
         fInfo.iId     = mVideo.value("id").toInt();
         fInfo.iLength = mVideo.value("length").toInt();
         fInfo.iSize   = mVideo.value("size").toInt();
         fInfo.iWidth  = mVideo.value("width").toInt();
         fInfo.sCodec  = mVideo.value("codec").toString();
         fInfo.sFormat = mVideo.value("format").toString();
         fInfo.sTitle  = mVideo.value("title").toString();
         fInfo.sUrl    = mVideo.value("url").toString();

         vidInfo.vVodFiles.append(fInfo);
      }
   }
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("Error QJSON can't parse respone!"));

      iRV = -1;
   }

   return iRV;
}

//---------------------------------------------------------------------------
//
//! \brief   parse epg response response
//
//! \author  Jo2003
//! \date    15.04.2013
//
//! \param   sResp (const QString &) ref. to response string
//! \param   epgList (QVector<cparser::SEpg> &) ref. to epg data vector
//
//! \return  0 --> ok; -1 --> any error
//---------------------------------------------------------------------------
int CStdJsonParser::parseEpg (const QString &sResp, QVector<cparser::SEpg> &epgList)
{
   int  iRV = 0;
   bool bOk = false;
   cparser::SEpg entry;
   QVariantMap   contentMap;
   QJson::Parser parser;
   QString sTmp;

   // clear vector ...
   epgList.clear();

   contentMap = parser.parse(sResp.toUtf8(), &bOk).toMap();

   if (bOk)
   {
      foreach (const QVariant& lEpg, contentMap.value("epg").toList())
      {
         QVariantMap mEpg = lEpg.toMap();

         entry.sDescr = "";
         entry.uiGmt  = mEpg.value("ut_start").toUInt();
         sTmp         = mEpg.value("progname").toString();

         if (sTmp.contains('\n'))
         {
            entry.sName  = sTmp.left(sTmp.indexOf('\n'));
            entry.sDescr = sTmp.mid(sTmp.indexOf('\n') + 1);
         }
         else
         {
            entry.sName = sTmp;
         }

         if (mEpg.contains("pdescr"))
         {
            entry.sDescr = mEpg.value("pdescr").toString();
         }

         epgList.append(entry);
      }
   }
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("Error QJSON can't parse respone!"));

      iRV = -1;
   }

   return iRV;
}

//---------------------------------------------------------------------------
//
//! \brief   parse one setting entry sent in login response
//
//! \author  Jo2003
//! \date    15.04.2013
//
//! \param   sResp (const QString &) ref. to response string
//! \param   sName (const QString &) ref. to setting name
//! \param   vValues (QVector<int>&) ref. to value vector
//! \param   iActVal (int&) ref. to current value
//
//! \return  0 --> ok; -1 --> any error
//---------------------------------------------------------------------------
int CStdJsonParser::parseSetting(const QString& sResp, const QString &sName, QVector<int>& vValues, int& iActVal)
{
   int  iRV = 0;
   bool bOk = false;
   QVariantMap   contentMap;
   QJson::Parser parser;

   // clear vector ...
   vValues.clear();

   contentMap = parser.parse(sResp.toUtf8(), &bOk).toMap();

   if (bOk)
   {
      contentMap = contentMap.value("settings").toMap();
      contentMap = contentMap.value(sName).toMap();

      iActVal = contentMap.value("value").toInt();

      foreach (const QVariant& val, contentMap.value("list").toList())
      {
         vValues.append(val.toInt());
      }
   }
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("Error QJSON can't parse respone!"));

      iRV = -1;
   }

   return iRV;
}

//---------------------------------------------------------------------------
//
//! \brief   parse URL response
//
//! \author  Jo2003
//! \date    15.04.2013
//
//! \param   sResp (const QString &) ref. to response string
//! \param   sUrl (QString &) ref. to url string
//
//! \return  0 --> ok; -1 --> any error
//---------------------------------------------------------------------------
int CStdJsonParser::parseUrl(const QString &sResp, QString &sUrl)
{
   int  iRV = 0;
   bool bOk = false;
   QVariantMap   contentMap;
   QJson::Parser parser;

   contentMap = parser.parse(sResp.toUtf8(), &bOk).toMap();

   if (bOk)
   {
      sUrl = contentMap.value("url").toString();
      sUrl.replace("&amp;", "&");

      if (sUrl.contains(' '))
      {
         sUrl = sUrl.left(sUrl.indexOf(' '));
      }
   }
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("Error QJSON can't parse respone!"));

      iRV = -1;
   }

   return iRV;
}

//---------------------------------------------------------------------------
//
//! \brief   parse VOD URL response
//
//! \author  Jo2003
//! \date    15.04.2013
//
//! \param   sResp (const QString &) ref. to response string
//! \param   sUrls (QStringList &) ref. to url list
//
//! \return  0 --> ok; -1 --> any error
//---------------------------------------------------------------------------
int CStdJsonParser::parseVodUrls (const QString& sResp, QStringList& sUrls)
{
   int  iRV = 0;
   bool bOk = false;
   QVariantMap   contentMap;
   QJson::Parser parser;
   QString       sUrl, sAdUrl;

   // clear url list ...
   sUrls.clear();

   contentMap = parser.parse(sResp.toUtf8(), &bOk).toMap();

   if (bOk)
   {
      sUrl = contentMap.value("url").toString();
      sUrl.replace("&amp;", "&");

      if (sUrl.contains(' '))
      {
         sUrl = sUrl.left(sUrl.indexOf(' '));
      }

      sAdUrl = contentMap.value("ad_url").toString();
      sAdUrl.replace("&amp;", "&");

      if (sAdUrl.contains(' '))
      {
         sAdUrl = sAdUrl.left(sAdUrl.indexOf(' '));
      }

      sUrls << sUrl;

      if (!sAdUrl.isEmpty())
      {
         sUrls << sAdUrl;
      }
   }
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("Error QJSON can't parse respone!"));

      iRV = -1;
   }

   return iRV;
}

//---------------------------------------------------------------------------
//
//! \brief   parse update info xml
//
//! \author  Jo2003
//! \date    12.10.2011
//
//! \param   sResp (const QString &) ref. to response string
//! \param   updInfo (cparser::SUpdInfo &) ref. to update info struct
//
//! \return  0 --> ok; -1 --> any error
//---------------------------------------------------------------------------
int CStdJsonParser::parseUpdInfo(const QString &sResp, cparser::SUpdInfo &updInfo)
{
   int                    iRV = 0;
   QString                sSys = "n.a.";
   bool                   bStarted = false, bDone = false;
   QXmlStreamReader       xml;

#if defined Q_OS_WIN32
   sSys = "win";
#elif defined Q_OS_LINUX
   sSys = "nix";
#elif defined Q_OS_MAC
   sSys = "osx";
#endif

   // clear updInfo struct ...
   updInfo.iMajor   = 0;
   updInfo.iMinor   = 0;
   updInfo.sVersion = "";
   updInfo.sUrl     = "";

   xml.addData(sResp);

   while(!xml.atEnd() && !xml.hasError() && !bDone)
   {
      switch (xml.readNext())
      {
      // any xml element starts ...
      case QXmlStreamReader::StartElement:
         if (xml.name() == sSys)
         {
            bStarted = true;
         }
         else if ((xml.name() == "major") && bStarted)
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               updInfo.iMajor = xml.text().toString().toInt();
            }
         }
         else if ((xml.name() == "minor") && bStarted)
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               updInfo.iMinor = xml.text().toString().toInt();
            }
         }
         else if ((xml.name() == "string_version") && bStarted)
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               updInfo.sVersion = xml.text().toString();
            }
         }
         else if ((xml.name() == "link") && bStarted)
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               updInfo.sUrl = xml.text().toString();
            }
         }
         break;

      case QXmlStreamReader::EndElement:
         if (xml.name() == sSys)
         {
            bStarted = false;
            bDone    = true;
         }
         break;

      default:
         break;
      }
   }

   // check for xml errors ...
   if(xml.hasError())
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("XML Error String: %1").arg(xml.errorString()));

      iRV = -1;
   }

   return iRV;
}
