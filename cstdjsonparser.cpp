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

   int                 iRV     = 0;
   int                 iGrpIdx = 0;
   cparser::SChan      chan;
   cparser::STimeShift ts;
   QJsonParseError     jErr;
   QJsonDocument       jsonDoc = QJsonDocument::fromJson(sResp.toUtf8(), &jErr);

   // clear channel list ...
   chanList.clear();

   if (jErr.error == QJsonParseError::NoError)
   {
      if (jsonDoc.isObject())
      {
         QJsonArray  groups  = jsonDoc.object().value("groups").toArray();
         QJsonObject groupObj;

         foreach (const QJsonValue& group, groups)
         {
            groupObj = group.toObject();

            initChanEntry(chan, false);

            chan.iId       = groupObj.value("id").toVariant().toInt();
            chan.sName     = groupObj.value("name").toString();
            chan.sProgramm = groupObj.value("color").toString();

            if (!ignoreGroup(chan))
            {
               // make sure group color isn't black ...
               checkColor(chan.sProgramm, iGrpIdx++);

               chanList.append(chan);

               QJsonArray  channels = groupObj.value("channels").toArray();
               QJsonObject channObj;

               foreach (const QJsonValue& channel, channels)
               {
                  channObj = channel.toObject();

                  initChanEntry(chan);

                  chan.iId          = channObj.value("id").toVariant().toInt();
                  chan.sName        = channObj.value("name").toString();
                  chan.bIsVideo     = channObj.value("is_video").toVariant().toBool();
                  chan.bHasArchive  = channObj.value("have_archive").toVariant().toBool();
                  chan.bIsProtected = channObj.value("protected").toVariant().toBool();
#ifdef _TASTE_CHITRAM_TV
                  // due to resource problems chitram.tv hasn't so far
                  // normal channel icons ...
                  chan.sIcon        = channObj.value("big_icon").toString();
#else
                  chan.sIcon        = channObj.value("icon").toString();
#endif // _TASTE_CHITRAM_TV
                  chan.sProgramm    = channObj.value("epg_progname").toString();
                  chan.uiStart      = channObj.value("epg_start").toVariant().toUInt();
                  chan.uiEnd        = channObj.value("epg_end").toVariant().toUInt();
                  chan.bIsHidden    = channObj.value("hide").toVariant().toBool();

                  QJsonArray  strParams = channObj.value("stream_params").toArray();
                  QJsonObject stpObj;

                  foreach (const QJsonValue& strParam, strParams)
                  {
                     stpObj        = strParam.toObject();

                     ts.iBitRate   = stpObj.value("rate").toVariant().toInt();
                     ts.iTimeShift = stpObj.value("ts").toVariant().toInt();

                     chan.vTs.append(ts);
                  }

                  chanList.append(chan);
               }
            }
         }
      }
   }
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("QJson parser error in %1 %2():%3 -> %4")
                    .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__)
                    .arg(jErr.errorString()));
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
   int             iRV = 0;
   cparser::SSrv   srv;
   QJsonParseError jErr;
   QJsonDocument   jsonDoc = QJsonDocument::fromJson(sResp.toUtf8(), &jErr);

   // clear server list ...
   vSrv.clear();

   if (jErr.error == QJsonParseError::NoError)
   {
      // answer always comes as object ...
      if (jsonDoc.isObject())
      {
         QJsonObject jObj = jsonDoc.object();

         // dive to stream servers ...
         QJsonValue jVal = jObj.value("settings").toObject().value("stream_server");

         if (jVal.isObject())
         {
            sActIp = jVal.toObject().value("value").toString();

            QJsonArray jArr = jVal.toObject().value("list").toArray();

            foreach (const QJsonValue& jVal2, jArr)
            {
               if (jVal2.isObject())
               {
                  srv.sIp   = jVal2.toObject().value("ip").toString();
                  srv.sName = jVal2.toObject().value("descr").toString();
                  vSrv.append(srv);
               }
            }
         }
      }
   }
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("QJson parser error in %1 %2():%3 -> %4")
                    .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__)
                    .arg(jErr.errorString()));
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
   int             iRV = 0;
   QJsonParseError jErr;
   QJsonDocument   jsonDoc = QJsonDocument::fromJson(sResp.toUtf8(), &jErr);

   sCookie = "";

   if (jErr.error == QJsonParseError::NoError)
   {
      // answer always comes as object ...
      if (jsonDoc.isObject())
      {
         QJsonObject mainObj = jsonDoc.object();

         // cookie ...
         sCookie = QString("%1=%2")
               .arg(mainObj.value("sid_name").toString())
               .arg(mainObj.value("sid").toString());

         // check offset ...
         checkTimeOffSet (mainObj.value("servertime").toVariant().toUInt());

         QJsonObject accObj = mainObj.value("account").toObject();

         // expires ...
         sInf.dtExpires   = QDateTime::fromTime_t(accObj.value("packet_expire").toVariant().toUInt());
         sInf.sExpires    = sInf.dtExpires.toString(DEF_TIME_FORMAT);


         QJsonObject servObj = mainObj.value("services").toObject();

         // services ...
         sInf.bHasArchive = servObj.value("archive").toVariant().toBool();
         sInf.bHasVOD     = servObj.value("vod").toVariant().toBool();
      }
   }
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("QJson parser error in %1 %2():%3 -> %4")
                    .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__)
                    .arg(jErr.errorString()));
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
   int             iRV = 0;
   cparser::SGenre sGenre;
   QJsonParseError jErr;
   QJsonDocument   jsonDoc = QJsonDocument::fromJson(sResp.toUtf8(), &jErr);

   // clear genres ...
   vGenres.clear();

   if (jErr.error == QJsonParseError::NoError)
   {
      // answer always comes as object ...
      if (jsonDoc.isObject())
      {
         QJsonArray  genres  = jsonDoc.object().value("genres").toArray();
         QJsonObject genreObj;

         foreach (const QJsonValue& genre, genres)
         {
            genreObj = genre.toObject();

            sGenre.sGName = genreObj.value("name").toString();
            sGenre.uiGid  = genreObj.value("id").toVariant().toUInt();

            vGenres.append(sGenre);
         }
      }
   }
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("QJson parser error in %1 %2():%3 -> %4")
                    .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__)
                    .arg(jErr.errorString()));
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
   int iRV =  0;
   int cid = -1;
   cparser::SEpgCurrent          entry;
   QVector<cparser::SEpgCurrent> vEntries;
   QJsonParseError jErr;
   QJsonDocument   jsonDoc = QJsonDocument::fromJson(sResp.toUtf8(), &jErr);

   // clear map ...
   currentEpg.clear();

   if (jErr.error == QJsonParseError::NoError)
   {
      // answer always comes as object ...
      if (jsonDoc.isObject())
      {
         QJsonArray  epgEntries = jsonDoc.object().value("epg").toArray();
         QJsonObject epgObj, epgObjL2;

         foreach (const QJsonValue& epg, epgEntries)
         {
            epgObj = epg.toObject();

            vEntries.clear();

            cid = epgObj.value("cid").toVariant().toInt();

            foreach (const QJsonValue& epg2, epgObj.value("epg").toArray())
            {
               epgObjL2 = epg2.toObject();

               entry.sShow   = epgObjL2.value("progname").toString();
               entry.uiStart = epgObjL2.value("ts").toVariant().toUInt();

               vEntries.append(entry);
            }

            currentEpg.insert(cid, vEntries);
         }
      }
   }
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("QJson parser error in %1 %2():%3 -> %4")
                    .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__)
                    .arg(jErr.errorString()));
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
   int               iRV = 0;
   cparser::SVodRate entry;
   QJsonParseError   jErr;
   QJsonDocument     jsonDoc = QJsonDocument::fromJson(sResp.toUtf8(), &jErr);

   // clear rates ...
   vRates.clear();

   if (jErr.error == QJsonParseError::NoError)
   {
      // answer always comes as object ...
      if (jsonDoc.isObject())
      {
         QJsonArray  results = jsonDoc.object().value("result").toArray();
         QJsonObject resObj;

         foreach (const QJsonValue& result, results)
         {
            resObj = result.toObject();

            entry.iRateID = resObj.value("id_rate").toVariant().toInt();
            entry.sGenre  = resObj.value("rate_name").toString();
            entry.sAccess = resObj.value("action").toString();

            vRates.append(entry);
         }
      }
   }
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("QJson parser error in %1 %2():%3 -> %4")
                    .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__)
                    .arg(jErr.errorString()));
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
   int                iRV = 0;
   cparser::SVodVideo entry;
   QJsonParseError    jErr;
   QJsonDocument      jsonDoc = QJsonDocument::fromJson(sResp.toUtf8(), &jErr);

   // clear list ...
   vVodList.clear();

   if (jErr.error == QJsonParseError::NoError)
   {
      // answer always comes as object ...
      if (jsonDoc.isObject())
      {
         QJsonObject mainObj = jsonDoc.object();

         gInfo.sType  = mainObj.value("type").toString();
         gInfo.iTotal = mainObj.value("total").toVariant().toInt();
         gInfo.iPage  = mainObj.value("page").toVariant().toInt();
         gInfo.iCount = mainObj.value("count").toVariant().toInt();

         QJsonArray  rows = mainObj.value("rows").toArray();
         QJsonObject rowObj;

         foreach (const QJsonValue& row, rows)
         {
            rowObj = row.toObject();

            entry.uiVidId    = rowObj.value("id").toVariant().toUInt();
            entry.sName      = rowObj.value("name").toString();
            entry.sDescr     = rowObj.value("description").toString();
            entry.sYear      = rowObj.value("year").toString();
            entry.sCountry   = rowObj.value("country").toString();
            entry.sImg       = rowObj.value("poster").toString();
            entry.bProtected = rowObj.value("pass_protect").toVariant().toBool();
            entry.bFavourit  = rowObj.value("favorite").toVariant().toBool();

            vVodList.append(entry);
         }
      }
   }
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("QJson parser error in %1 %2():%3 -> %4")
                    .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__)
                    .arg(jErr.errorString()));
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
   int             iRV = 0;
   QJsonParseError jErr;
   QJsonDocument   jsonDoc = QJsonDocument::fromJson(sResp.toUtf8(), &jErr);
   cparser::SVodFileInfo fInfo;

   // init struct ...
   vidInfo.sActors    = "";
   vidInfo.sCountry   = "";
   vidInfo.sDescr     = "";
   vidInfo.sDirector  = "";
   vidInfo.sImg       = "";
   vidInfo.sName      = "";
   vidInfo.sYear      = "";
   vidInfo.sPgRating        = "";
   vidInfo.sImdbRating      = "";
   vidInfo.sKinopoiskRating = "";
   vidInfo.uiLength   = 0;
   vidInfo.uiVidId    = 0;
   vidInfo.bProtected = false;
   vidInfo.bFavourit  = false;
   vidInfo.vVodFiles.clear();

   if (jErr.error == QJsonParseError::NoError)
   {
      // answer always comes as object ...
      if (jsonDoc.isObject())
      {
         QJsonObject filmObj = jsonDoc.object().value("film").toObject();

         vidInfo.sActors          = filmObj.value("actors").toString();
         vidInfo.sCountry         = filmObj.value("country").toString();
         vidInfo.sDescr           = filmObj.value("description").toString();
         vidInfo.sDirector        = filmObj.value("director").toString();
         vidInfo.sImg             = filmObj.value("poster").toString();
         vidInfo.sName            = filmObj.value("name").toString();
         vidInfo.sYear            = filmObj.value("year").toString();
         vidInfo.sGenres          = filmObj.value("genre_str").toString();
         vidInfo.uiLength         = filmObj.value("lenght").toVariant().toUInt();
         vidInfo.uiVidId          = filmObj.value("id").toVariant().toUInt();
         vidInfo.bFavourit        = filmObj.value("favorite").toVariant().toBool();
         vidInfo.bProtected       = filmObj.value("pass_protect").toVariant().toBool();
         vidInfo.sPgRating        = filmObj.value("rate_mpaa").toString();
         vidInfo.sImdbRating      = filmObj.value("rate_imdb").toString();
         vidInfo.sKinopoiskRating = filmObj.value("rate_kinopoisk").toString();

         QJsonArray  videos = filmObj.value("videos").toArray();
         QJsonObject vidObj;

         foreach (const QJsonValue& video, videos)
         {
            vidObj = video.toObject();

            fInfo.iHeight = vidObj.value("height").toVariant().toInt();
            fInfo.iId     = vidObj.value("id").toVariant().toInt();
            fInfo.iLength = vidObj.value("length").toVariant().toInt();
            fInfo.iSize   = vidObj.value("size").toVariant().toInt();
            fInfo.iWidth  = vidObj.value("width").toVariant().toInt();
            fInfo.sCodec  = vidObj.value("codec").toString();
            fInfo.sFormat = vidObj.value("format").toString();
            fInfo.sTitle  = vidObj.value("title").toString();
            fInfo.sUrl    = vidObj.value("url").toString();

            vidInfo.vVodFiles.append(fInfo);
         }
      }
   }
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("QJson parser error in %1 %2():%3 -> %4")
                    .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__)
                    .arg(jErr.errorString()));
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
   int             iRV = 0;
   QString         sTmp;
   cparser::SEpg   entry;
   QJsonParseError jErr;
   QJsonDocument   jsonDoc = QJsonDocument::fromJson(sResp.toUtf8(), &jErr);

   // clear list ...
   epgList.clear();

   if (jErr.error == QJsonParseError::NoError)
   {
      // answer always comes as object ...
      if (jsonDoc.isObject())
      {
         QJsonArray  epgArr = jsonDoc.object().value("epg").toArray();
         QJsonObject epgObj;

         foreach (const QJsonValue& epg, epgArr)
         {
            epgObj = epg.toObject();

            entry.sDescr = "";
            entry.uiGmt  = epgObj.value("ut_start").toVariant().toUInt();
            sTmp         = epgObj.value("progname").toString();

            if (sTmp.contains('\n'))
            {
               entry.sName  = sTmp.left(sTmp.indexOf('\n'));
               entry.sDescr = sTmp.mid(sTmp.indexOf('\n') + 1);
            }
            else
            {
               entry.sName = sTmp;
            }

            if (epgObj.contains("pdescr"))
            {
               entry.sDescr = epgObj.value("pdescr").toString();
            }
            else if(epgObj.contains("description"))
            {
               entry.sDescr = epgObj.value("description").toString();
            }

            epgList.append(entry);
         }
      }
   }
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("QJson parser error in %1 %2():%3 -> %4")
                    .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__)
                    .arg(jErr.errorString()));
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
   int             iRV = 0;
   QJsonParseError jErr;
   QJsonDocument   jsonDoc = QJsonDocument::fromJson(sResp.toUtf8(), &jErr);

   // clear vector ...
   vValues.clear();

   if (jErr.error == QJsonParseError::NoError)
   {
      // answer always comes as object ...
      if (jsonDoc.isObject())
      {
         QJsonObject setObj = jsonDoc.object().value("settings").toObject().value(sName).toObject();

         iActVal = setObj.value("value").toVariant().toInt();

         foreach (const QJsonValue& val, setObj.value("list").toArray())
         {
            vValues.append(val.toVariant().toInt());
         }
      }
   }
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("QJson parser error in %1 %2():%3 -> %4")
                    .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__)
                    .arg(jErr.errorString()));
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
   int             iRV = 0;
   QJsonParseError jErr;
   QJsonDocument   jsonDoc = QJsonDocument::fromJson(sResp.toUtf8(), &jErr);

   // clear url ...
   sUrl = "";

   if (jErr.error == QJsonParseError::NoError)
   {
      // answer always comes as object ...
      if (jsonDoc.isObject())
      {
         sUrl = jsonDoc.object().value("url").toString();
         sUrl.replace("&amp;", "&");

         if (sUrl.contains(' '))
         {
            sUrl = sUrl.left(sUrl.indexOf(' '));
         }
      }
   }
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("QJson parser error in %1 %2():%3 -> %4")
                    .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__)
                    .arg(jErr.errorString()));
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
   int             iRV = 0;
   QJsonParseError jErr;
   QJsonDocument   jsonDoc = QJsonDocument::fromJson(sResp.toUtf8(), &jErr);
   QString         sUrl, sAdUrl;

   // clear url ...
   sUrls.clear();

   if (jErr.error == QJsonParseError::NoError)
   {
      // answer always comes as object ...
      if (jsonDoc.isObject())
      {
         sUrl = jsonDoc.object().value("url").toString();
         sUrl.replace("&amp;", "&");

         if (sUrl.contains(' '))
         {
            sUrl = sUrl.left(sUrl.indexOf(' '));
         }

         sAdUrl = jsonDoc.object().value("ad_url").toString();
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
   }
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("QJson parser error in %1 %2():%3 -> %4")
                    .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__)
                    .arg(jErr.errorString()));
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
   int             iRV = 0;
   QJsonParseError jErr;
   QJsonDocument   jsonDoc = QJsonDocument::fromJson(sResp.toUtf8(), &jErr);

   // clear message ...
   sMsg = "";

   if (jErr.error == QJsonParseError::NoError)
   {
      // answer always comes as object ...
      if (jsonDoc.isObject())
      {
         QJsonObject errObj = jsonDoc.object().value("error").toObject();
         sMsg  = errObj.value("message").toString();
         eCode = errObj.value("code").toVariant().toInt();
      }
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
   int             iRV = 0;
   QJsonParseError jErr;
   QJsonDocument   jsonDoc = QJsonDocument::fromJson(sResp.toUtf8(), &jErr);

   // clear map ...
   lMap.clear();

   if (jErr.error == QJsonParseError::NoError)
   {
      // answer always comes as object ...
      if (jsonDoc.isObject())
      {
         QJsonArray  langArr = jsonDoc.object().value("langs").toArray();
         QJsonObject langObj;

         foreach(const QJsonValue& lang, langArr)
         {
            langObj = lang.toObject();

            lMap.insert(langObj.value("name").toString(), langObj.value("lang").toString());
         }
      }
   }
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("QJson parser error in %1 %2():%3 -> %4")
                    .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__)
                    .arg(jErr.errorString()));
      iRV = -1;
   }

   return iRV;
}
