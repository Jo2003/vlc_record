/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     cnovoeparser.cpp
 *
 *  @author   Jo2003
 *
 *  @date     15.04.2013
 *
 *  $Id$
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#include "cnovoeparser.h"
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
CNovoeParser::CNovoeParser(QObject * parent) : CStdJsonParser(parent)
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
int CNovoeParser::parseChannelList (const QString &sResp,
                                         QVector<cparser::SChan> &chanList)
{
   int                 iRV     = 0;
   int                 iGrpIdx = 0;
   cparser::SChan      chan;
   cparser::STimeShift ts;
   QString             strImgPrefix = "/_logos/channelLogos/";
   QJsonParseError     jErr;
   QJsonDocument       jsonDoc = QJsonDocument::fromJson(sResp.toUtf8(), &jErr);

   // clear channel list ...
   chanList.clear();

   if (jErr.error == QJsonParseError::NoError)
   {
      // answer always comes as object ...
      if (jsonDoc.isObject())
      {
         QJsonArray  groups = jsonDoc.object().value("groups").toArray();
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
               QJsonObject chanObj;

               foreach (const QJsonValue& channel, channels)
               {
                  chanObj = channel.toObject();

                  initChanEntry(chan);

                  chan.iId          = chanObj.value("id").toVariant().toInt();
                  chan.sName        = chanObj.value("name").toString();
                  chan.bIsVideo     = chanObj.value("is_video").toVariant().toBool();
                  chan.bHasArchive  = chanObj.value("have_archive").toVariant().toBool();
                  chan.bIsProtected = chanObj.value("protected").toVariant().toBool();
                  chan.sIcon        = strImgPrefix + chanObj.value("logo_big").toString();
                  chan.sProgramm    = chanObj.value("epg_progname").toString();
                  chan.uiStart      = chanObj.value("epg_start").toVariant().toUInt();
                  chan.uiEnd        = chanObj.value("epg_end").toVariant().toUInt();

                  QJsonArray  params = chanObj.value("stream_params").toArray();
                  QJsonObject paramObj;

                  foreach (const QJsonValue& param, params)
                  {
                     paramObj = param.toObject();

                     ts.iBitRate   = paramObj.value("rate").toVariant().toInt();
                     ts.iTimeShift = paramObj.value("ts").toVariant().toInt();

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
int CNovoeParser::parseSServersLogin(const QString &sResp, QVector<cparser::SSrv> &vSrv,
                                     QString &sActIp)
{
   // Entries in server list are faked only -> ignore them!
   Q_UNUSED(sResp)
   vSrv.clear();
   sActIp = "";

   return 0;
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
int CNovoeParser::parseVodList(const QString &sResp, QVector<cparser::SVodVideo> &vVodList, cparser::SGenreInfo &gInfo)
{
   int                iRV     = 0;
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
            entry.sImg       = rowObj.value("poster").toString();
            entry.bProtected = (rowObj.value("vis").toString() != "on") ? true : false;
            entry.sCountry   = rowObj.value("country").toObject().value("name").toString();

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
int CNovoeParser::parseVideoInfo(const QString &sResp, cparser::SVodVideo &vidInfo)
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

         vidInfo.sActors    = filmObj.value("actors").toString();
         vidInfo.sDescr     = filmObj.value("description").toString();
         vidInfo.sDirector  = filmObj.value("director").toString();
         vidInfo.sImg       = filmObj.value("poster").toString();
         vidInfo.sName      = filmObj.value("name").toString();
         vidInfo.sYear      = filmObj.value("year").toString();
         vidInfo.sGenres    = filmObj.value("genre_str").toString();
         vidInfo.uiLength   = filmObj.value("lenght").toVariant().toUInt();
         vidInfo.uiVidId    = filmObj.value("id").toVariant().toUInt();
         vidInfo.bProtected = (filmObj.value("vis").toString() != "on") ? true : false;
         vidInfo.sCountry   = filmObj.value("country").toObject().value("name").toString();

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
int CNovoeParser::parseEpg (const QString &sResp, QVector<cparser::SEpg> &epgList)
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
            entry.uiEnd  = epgObj.value("ut_end").toVariant().toUInt();
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
int CNovoeParser::parseSetting(const QString& sResp, const QString &sName, QVector<int>& vValues, int& iActVal)
{
   // all settings are faked only -> ignore them!
   Q_UNUSED(sResp)
   Q_UNUSED(sName)
   vValues.clear();
   iActVal = -1;
   return 0;
}

//---------------------------------------------------------------------------
//
//! \brief   parse current epg current response
//
//! \author  Jo2003
//! \date    01.08.2013
//
//! \param   sResp (const QString &) ref. to response string
//! \param   currentEpg (QCurrentMap &) ref. to epg data map
//
//! \return  0 --> ok; -1 --> any error
//---------------------------------------------------------------------------
int CNovoeParser::parseEpgCurrent (const QString& sResp, QCurrentMap &currentEpg)
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

               entry.sShow   = epgObjL2.value("epg_progname").toString();
               entry.uiStart = epgObjL2.value("epg_start").toVariant().toUInt();
               entry.uiEnd   = epgObjL2.value("epg_end").toVariant().toUInt();

               vEntries.append(entry);
            }

            currentEpg.insert(cid, vEntries);
#ifdef __TRACE
            QString s;
            s = tr("Update Entries for channel %1:\n").arg(cid);

            for (int i = 0; i < vEntries.count(); i++)
            {
               s += QString("%1 - %2: %3\n")
                     .arg(QDateTime::fromTime_t(vEntries.at(i).uiStart).toString("dd.MM.yyyy hh:mm"))
                     .arg(QDateTime::fromTime_t(vEntries.at(i).uiEnd).toString("dd.MM.yyyy hh:mm"))
                     .arg(vEntries.at(i).sShow);
            }

            mInfo(s);
#endif // __TRACE
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
