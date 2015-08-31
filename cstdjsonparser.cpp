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
#include "externals_inc.h"

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
CStdJsonParser::CStdJsonParser(QObject * parent) : CApiParser(parent)
{
   // nothing to do so far
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
//
//! \return  0 --> ok; -1 --> any error
//---------------------------------------------------------------------------
int CStdJsonParser::parseChannelList (const QString &sResp,
                                         QVector<cparser::SChan> &chanList)
{
   int  iRV = 0;
   bool bOk = false;
   cparser::SChan      chan;
   cparser::STimeShift ts;
   QVariantMap   contentMap;
   int iGrpIdx = 0;

   // clear channel list ...
   chanList.clear();

   contentMap = QtJson::parse(sResp, bOk).toMap();

   if (bOk)
   {
      foreach (const QVariant& lGroup, contentMap.value("groups").toList())
      {
         QVariantMap mGroup = lGroup.toMap();

         initChanEntry(chan, false);

         chan.iId       = mGroup.value("id").toInt();
         chan.sName     = mGroup.value("name").toString();
         chan.sProgramm = mGroup.value("color").toString();

         if (!ignoreGroup(chan))
         {
            // make sure group color isn't black ...
            checkColor(chan.sProgramm, iGrpIdx++);

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
#ifdef _TASTE_CHITRAM_TV
               // due to resource problems chitram.tv hasn't so far
               // normal channel icons ...
               chan.sIcon        = mChannel.value("big_icon").toString();
#else
               chan.sIcon        = mChannel.value("icon").toString();
#endif // _TASTE_CHITRAM_TV
               chan.sProgramm    = mChannel.value("epg_progname").toString();
               chan.uiStart      = mChannel.value("epg_start").toUInt();
               chan.uiEnd        = mChannel.value("epg_end").toUInt();
               chan.bIsHidden    = mChannel.value("hide").toBool();

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
   }
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("QtJson parser error in %1 %2():%3")
                    .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__));

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
int CStdJsonParser::parseStrStd(const QString &sResp, cparser::QStrStdMap &strStdMap, QString &curr)
{
    int  iRV = 0;
    bool bOk = false;
    cparser::StrStdDescr ssDescr;
    QVariantMap     contentMap, nestedMap;

    contentMap = QtJson::parse(sResp, bOk).toMap();

    // clear given parameters ...
    strStdMap.clear();
    curr.clear();

    contentMap = QtJson::parse(sResp, bOk).toMap();

    if (bOk)
    {
       nestedMap = contentMap.value("settings").toMap();
       nestedMap = nestedMap.value("stream_standard").toMap();

       curr      = nestedMap.value("value").toString();

       foreach (const QVariant& lSrv, nestedMap.value("list").toList())
       {
          QVariantMap mSrv = lSrv.toMap();

          ssDescr.sName  = mSrv.value("title").toString();
          ssDescr.sDescr = mSrv.value("description").toString();
          strStdMap[mSrv.value("value").toString()] = ssDescr;
       }
    }
    else
    {
       emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                     tr("QtJson parser error in %1 %2():%3")
                     .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__));

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

   contentMap = QtJson::parse(sResp, bOk).toMap();

   // clear server list ...
   vSrv.clear();

   contentMap = QtJson::parse(sResp, bOk).toMap();

   if (bOk)
   {
      nestedMap = contentMap.value("settings").toMap();
      nestedMap = nestedMap.value("stream_server").toMap();

      sActIp    = nestedMap.value("value").toString();

      foreach (const QVariant& lSrv, nestedMap.value("list").toList())
      {
         QVariantMap mSrv = lSrv.toMap();

         srv.sIp   = mSrv.value("ip").toString();
         srv.sName = mSrv.value("descr").toString();

         vSrv.append(srv);
      }
   }
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("QtJson parser error in %1 %2():%3")
                    .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__));

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

   contentMap = QtJson::parse(sResp, bOk).toMap();

   if (bOk)
   {
      sCookie = QString("%1=%2")
            .arg(contentMap.value("sid_name").toString())
            .arg(contentMap.value("sid").toString());


      nestedMap        = contentMap.value("account").toMap();
      sInf.sExpires    = QDateTime::fromTime_t(nestedMap.value("packet_expire").toUInt())
                           .toString(DEF_TIME_FORMAT);
      sInf.dtExpires   = QDateTime::fromTime_t(nestedMap.value("packet_expire").toUInt());

      nestedMap        = contentMap.value("services").toMap();
      sInf.bHasArchive = nestedMap.value("archive").toBool();
      sInf.bHasVOD     = nestedMap.value("vod").toBool();

      // check offset ...
      checkTimeOffSet (contentMap.value("servertime").toUInt());
   }
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("QtJson parser error in %1 %2():%3")
                    .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__));

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

   // clear genres ...
   vGenres.clear();

   contentMap = QtJson::parse(sResp, bOk).toMap();

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
                    tr("QtJson parser error in %1 %2():%3")
                    .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__));

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

   // clear map ...
   currentEpg.clear();

   contentMap = QtJson::parse(sResp, bOk).toMap();

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
                    tr("QtJson parser error in %1 %2():%3")
                    .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__));

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

   // clear map ...
   vRates.clear();

   contentMap = QtJson::parse(sResp, bOk).toMap();

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
                    tr("QtJson parser error in %1 %2():%3")
                    .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__));

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

   // clear vector ...
   vVodList.clear();

   contentMap = QtJson::parse(sResp, bOk).toMap();

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
                    tr("QtJson parser error in %1 %2():%3")
                    .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__));

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
   QVariantMap contentMap;
   cparser::SVodFileInfo fInfo;

   // init struct ...
   vidInfo.sActors          = "";
   vidInfo.sCountry         = "";
   vidInfo.sDescr           = "";
   vidInfo.sDirector        = "";
   vidInfo.sImg             = "";
   vidInfo.sName            = "";
   vidInfo.sYear            = "";
   vidInfo.sPgRating        = "";
   vidInfo.sImdbRating      = "";
   vidInfo.sKinopoiskRating = "";
   vidInfo.uiLength         = 0;
   vidInfo.uiVidId          = 0;
   vidInfo.bProtected       = false;
   vidInfo.bFavourit        = false;
   vidInfo.vVodFiles.clear();

   contentMap = QtJson::parse(sResp, bOk).toMap();

   if (bOk)
   {
      contentMap = contentMap.value("film").toMap();

      vidInfo.sActors          = contentMap.value("actors").toString();
      vidInfo.sCountry         = contentMap.value("country").toString();
      vidInfo.sDescr           = contentMap.value("description").toString();
      vidInfo.sDirector        = contentMap.value("director").toString();
      vidInfo.sImg             = contentMap.value("poster").toString();
      vidInfo.sName            = contentMap.value("name").toString();
      vidInfo.sYear            = contentMap.value("year").toString();
      vidInfo.sGenres          = contentMap.value("genre_str").toString();
      vidInfo.uiLength         = contentMap.value("lenght").toUInt();
      vidInfo.uiVidId          = contentMap.value("id").toUInt();
      vidInfo.bFavourit        = contentMap.value("favorite").toBool();
      vidInfo.bProtected       = contentMap.value("pass_protect").toBool();
      vidInfo.sPgRating        = contentMap.value("rate_mpaa").toString();
      vidInfo.sImdbRating      = contentMap.value("rate_imdb").toString();
      vidInfo.sKinopoiskRating = contentMap.value("rate_kinopoisk").toString();

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
                    tr("QtJson parser error in %1 %2():%3")
                    .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__));

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
   QVariantMap contentMap;
   QString sTmp;

   // clear vector ...
   epgList.clear();

   contentMap = QtJson::parse(sResp, bOk).toMap();

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
         else if(mEpg.contains("description"))
         {
            entry.sDescr = mEpg.value("description").toString();
         }

         epgList.append(entry);
      }
   }
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("QtJson parser error in %1 %2():%3")
                    .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__));

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
   QVariantMap contentMap;

   // clear vector ...
   vValues.clear();

   contentMap = QtJson::parse(sResp, bOk).toMap();

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
                    tr("QtJson parser error in %1 %2():%3")
                    .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__));

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
   QVariantMap contentMap;

   contentMap = QtJson::parse(sResp, bOk).toMap();

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
                    tr("QtJson parser error in %1 %2():%3")
                    .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__));

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
   QVariantMap contentMap;
   QString     sUrl, sAdUrl;

   // clear url list ...
   sUrls.clear();

   contentMap = QtJson::parse(sResp, bOk).toMap();

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
                    tr("QtJson parser error in %1 %2():%3")
                    .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__));

      iRV = -1;
   }

   return iRV;
}

//---------------------------------------------------------------------------
//
//! \brief   parse error
//
//! \author  Jo2003
//! \date    16.04.2013
//
//! \param   sResp (const QString &) ref. to response string
//! \param   sMsg (QString &) ref. to error message
//! \param   eCode (int&) ref. to error code
//
//! \return  0 --> ok; -1 --> any error
//---------------------------------------------------------------------------
int CStdJsonParser::parseError (const QString& sResp, QString &sMsg, int &eCode)
{
   int  iRV = 0;
   bool bOk = false;
   QVariantMap contentMap;

   contentMap = QtJson::parse(sResp, bOk).toMap();

   if (bOk)
   {
      contentMap = contentMap.value("error").toMap();

      sMsg       = contentMap.value("message").toString();
      eCode      = contentMap.value("code").toInt();
   }
   else
   {
      // we shouldn't report any error using sigError when
      // parsing an error ... !
      iRV = -1;
   }

   return iRV;
}

//---------------------------------------------------------------------------
//
//! \brief   parse available VOD languages
//
//! \author  Jo2003
//! \date    16.04.2013
//
//! \param   sResp (const QString &) ref. to response string
//! \param   lMap (QVodLangMap &) ref. to language map
//
//! \return  0 --> ok; -1 --> any error
//---------------------------------------------------------------------------
int CStdJsonParser::parseVodLang(const QString &sResp, QVodLangMap &lMap)
{
   int  iRV = 0;
   bool bOk = false;
   QVariantMap contentMap;

   contentMap = QtJson::parse(sResp, bOk).toMap();

   if (bOk)
   {
      foreach (const QVariant& lRow, contentMap.value("langs").toList())
      {
         QVariantMap mRow = lRow.toMap();
         lMap.insert(mRow.value("name").toString(), mRow.value("lang").toString());
      }
   }
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("QtJson parser error in %1 %2():%3")
                    .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__));

      iRV = -1;
   }

   return iRV;
}

//---------------------------------------------------------------------------
//
//! \brief   parse service settings
//
//! \author  Jo2003
//! \date    16.10.2014
//
//! \param   sResp [in] (const QString &) ref. to response string
//! \param   servset [out] (cparser::ServiceSettings &) ref. to settings
//
//! \return  0 --> ok; -1 --> any error
//---------------------------------------------------------------------------
int CStdJsonParser::parseService(const QString &sResp, cparser::ServiceSettings &servset)
{
   int  iRV = 0;
   bool bOk = false;
   QtJson::JsonObject obj;

   obj = QtJson::parse(sResp, bOk).toMap();

   if (bOk)
   {
      if (obj.contains("server_address"))
      {
         servset.apiServer = obj.value("server_address").toString();
      }

      if (obj.contains("login"))
      {
         servset.login = obj.value("login").toString();
      }

      if (obj.contains("password"))
      {
         servset.pass = obj.value("password").toString();
      }

      if (obj.contains("stats_on"))
      {
         servset.stats = obj.value("stats_on").toInt();
      }

      if (obj.contains("buffering"))
      {
         servset.buffering = obj.value("buffering").toInt();
      }

      if (obj.contains("bitrate"))
      {
         servset.bitrate = obj.value("bitrate").toInt();
      }

      if (obj.contains("timeshift"))
      {
         servset.timeShift = obj.value("timeshift").toInt();
      }

      if (obj.contains("stream_server"))
      {
         servset.strServer = obj.value("stream_server").toString();
      }
   }
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("QtJson parser error in %1 %2():%3")
                    .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__));

      iRV = -1;
   }

   return iRV;
}
