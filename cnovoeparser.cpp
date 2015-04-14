/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/cnovoeparser.cpp $
 *
 *  @file     cnovoeparser.cpp
 *
 *  @author   Jo2003
 *
 *  @date     15.04.2013
 *
 *  $Id: cnovoeparser.cpp 1158 2013-08-01 10:10:02Z Olenka.Joerg $
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#include "cnovoeparser.h"

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
//! \param   bFixTime (bool) flag for time correction
//
//! \return  0 --> ok; -1 --> any error
//---------------------------------------------------------------------------
int CNovoeParser::parseChannelList (const QString &sResp,
                                         QVector<cparser::SChan> &chanList,
                                         bool bFixTime)
{
   int  iRV = 0;
   bool bOk = false;
   cparser::SChan      chan;
   cparser::STimeShift ts;
   QVariantMap contentMap;
   int iGrpIdx = 0;
   QString strImgPrefix = "/_logos/channelLogos/";

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
               chan.sIcon        = strImgPrefix + mChannel.value("logo_big").toString();
               chan.sProgramm    = mChannel.value("epg_progname").toString();
               chan.uiStart      = mChannel.value("epg_start").toUInt();
               chan.uiEnd        = mChannel.value("epg_end").toUInt();

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
int CNovoeParser::parseSServersLogin(const QString &sResp, QVector<cparser::SSrv> &vSrv,
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

         srv.sIp   = mSrv.value("id").toString();
         srv.sName = mSrv.value("desc").toString();

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

   return iRV
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
   int  iRV = 0;
   bool bOk = false;
   cparser::SVodVideo entry;
   QVariantMap contentMap;

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
         entry.sImg       = mRow.value("poster").toString();
         entry.bProtected = (mRow.value("vis").toString() != "on") ? true : false;
         entry.sCountry   = mRow.value("country").toMap().value("name").toString();

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
int CNovoeParser::parseVideoInfo(const QString &sResp, cparser::SVodVideo &vidInfo)
{
   int  iRV = 0;
   bool bOk = false;
   QVariantMap contentMap;
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

   contentMap = QtJson::parse(sResp, bOk).toMap();

   if (bOk)
   {
      contentMap = contentMap.value("film").toMap();

      vidInfo.sActors    = contentMap.value("actors").toString();
      vidInfo.sDescr     = contentMap.value("description").toString();
      vidInfo.sDirector  = contentMap.value("director").toString();
      vidInfo.sImg       = contentMap.value("poster").toString();
      vidInfo.sName      = contentMap.value("name").toString();
      vidInfo.sYear      = contentMap.value("year").toString();
      vidInfo.sGenres    = contentMap.value("genre_str").toString();
      vidInfo.uiLength   = contentMap.value("lenght").toUInt();
      vidInfo.uiVidId    = contentMap.value("id").toUInt();
      vidInfo.bProtected = (contentMap.value("vis").toString() != "on") ? true : false;
      vidInfo.sCountry   = contentMap.value("country").toMap().value("name").toString();

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
int CNovoeParser::parseEpg (const QString &sResp, QVector<cparser::SEpg> &epgList)
{
   int  iRV = 0;
   bool bOk = false;
   cparser::SEpg entry;
   QVariantMap   contentMap;
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
         entry.uiEnd  = mEpg.value("ut_end").toUInt();
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
   int  iRV = 0, cid;
   bool bOk = false;
   cparser::SEpgCurrent          entry;
   QVector<cparser::SEpgCurrent> vEntries;
   QVariantMap                   contentMap;

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

            entry.sShow   = mEpg2.value("epg_progname").toString();
            entry.uiStart = mEpg2.value("epg_start").toUInt();
            entry.uiEnd   = mEpg2.value("epg_end").toUInt();

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
   else
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("QtJson parser error in %1 %2():%3")
                    .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__));

      iRV = -1;
   }

   return iRV;
}
