/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     ctvclubparser.cpp
 *
 *  @author   Jo2003
 *
 *  @date     15.01.2017
 *
 *  $Id$
 *
 *///------------------------- (c) 2017 by Jo2003  --------------------------
#include "ctvclubparser.h"
#include "csettingsdlg.h"

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
CTVClubParser::CTVClubParser(QObject * parent) : CStdJsonParser(parent)
{
   // nothing to do so far
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
int CTVClubParser::parseCookie (const QString &sResp, QString &sCookie, cparser::SAccountInfo &sInf)
{
    int  iRV = 0;
    bool bOk = false;
    QVariantMap   contentMap, nestedMap;

    contentMap = QtJson::parse(sResp, bOk).toMap();

    if (bOk)
    {
        QtJson::JsonObject session = contentMap.value("session").toMap();

        // create cookie ...
        sCookie = QString("token=%1")
            .arg(session.value("token").toString());

        // check offset ...
        checkTimeOffSet (session.value("now").toUInt());

        // account ...
        QtJson::JsonObject account = contentMap.value("account").toMap();

        // archive ...
        QtJson::JsonObject options = account.value("options").toMap();
        sInf.bHasArchive = options.value("archive").toBool();

        // account info ...
        QtJson::JsonObject info = account.value("info").toMap();
        sInf.sName = info.value("name").toString();
        sInf.sMail = info.value("mail").toString();

        // services ...
        foreach (const QVariant& service, account.value("services").toList())
        {
            QtJson::JsonObject serv = service.toMap();
            QString servName = QString("%1 (%2)")
                    .arg(serv.value("name").toString())
                    .arg(serv.value("type").toString());

            mInfo(tr("Found service %2 ...").arg(servName));

            sInf.services[servName] = QDateTime::fromTime_t(serv.value("expire").toUInt()).toString(DEF_TIME_FORMAT);

            if (serv.value("id").toInt() == 1)
            {
                sInf.sExpires  = QDateTime::fromTime_t(serv.value("expire").toUInt()).toString(DEF_TIME_FORMAT);
                sInf.dtExpires = QDateTime::fromTime_t(serv.value("expire").toUInt());
            }
        }

        sInf.bHasVOD     = false; // nestedMap.value("vod").toBool();
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
int CTVClubParser::parseChannelList (const QString &sResp,
                                         QVector<cparser::SChan> &chanList,
                                         bool bFixTime)
{
   int  iRV = 0;
   bool bOk = false;
   cparser::SChan      chan;
   QtJson::JsonObject  contentMap;
   QString strImgPrefix = "http://tvclub.us/logo/72_72_1/%1.png";

   // clear channel list ...
   chanList.clear();

   contentMap = QtJson::parse(sResp, bOk).toMap();

   if (bOk)
   {
      foreach (const QVariant& rawChan, contentMap.value("channels").toList())
      {
         QtJson::JsonObject info  = rawChan.toMap().value("info").toMap();
         QtJson::JsonObject epg   = rawChan.toMap().value("epg").toMap();

         initChanEntry(chan, false);

         chan.iId          = info.value("id").toInt();
         chan.sName        = info.value("name").toString();
         chan.bIsProtected = info.value("protected").toBool();
         chan.iPrimGrp     = info.value("groups").toInt();
         chan.sProgramm    = epg.value("text").toString();
         chan.uiStart      = epg.value("start").toUInt();
         chan.uiEnd        = epg.value("end").toUInt();
         chan.iArchHours   = info.value("records").toInt();
         chan.bHasArchive  = (chan.iArchHours > 0);
         chan.sIcon        = strImgPrefix.arg(chan.iId);
         chan.bIsVideo     = true;
         chan.bIsGroup     = false;

         if (bFixTime)
         {
            fixTime(chan.uiStart);
            fixTime(chan.uiEnd);
         }

         chanList.append(chan);
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
//! \brief   parse channel groups
//
//! \param   sResp (const QString &) ref. to response string
//! \param   chanGroups (QVector<cparser::SGrp> &) channel groups
//
//! \return  0 --> ok; -1 --> any error
//---------------------------------------------------------------------------
int CTVClubParser::parseChannelGroups(const QString &sResp, QGrpMap &chanGroups)
{
    int  iRV = 0;
    bool bOk = false;
    QtJson::JsonObject contentMap;
    cparser::SGrp      chanGroup;

    contentMap = QtJson::parse(sResp, bOk).toMap();

    if (bOk)
    {
        // groups ...
        foreach(const QVariant& rawGrp, contentMap.value("groups").toList())
        {
            QtJson::JsonObject grp = rawGrp.toMap();

            chanGroup.sColor  = "";
            chanGroup.iId     = grp.value("id").toInt();
            chanGroup.iCount  = grp.value("count").toInt();
            chanGroup.sName   = grp.value("name_ru").toString();
            chanGroup.sNameEn = grp.value("name_en").toString();
            checkColor(chanGroup.sColor, chanGroup.iId);

            chanGroups.insert(chanGroup.iId, chanGroup);
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
//! \brief   parse url
//
//! \param   sResp (const QString &) ref. to response string
//! \param   sUrl (QString &) buffer for url
//
//! \return  0 --> ok; -1 --> any error
//---------------------------------------------------------------------------
int CTVClubParser::parseUrl(const QString &sResp, QString &sUrl)
{
    int  iRV = 0;
    bool bOk = false;
    QtJson::JsonObject contentMap;

    contentMap = QtJson::parse(sResp, bOk).toMap();

    if (bOk)
    {
        QtJson::JsonObject url;

        if (contentMap.contains("live"))
        {
            url = contentMap.value("live").toMap();
        }
        else if (contentMap.contains("rec"))
        {
            url = contentMap.value("rec").toMap();
        }

        sUrl = url.value("url").toString();
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
int CTVClubParser::parseSServersLogin(const QString &sResp, QVector<cparser::SSrv> &vSrv,
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
int CTVClubParser::parseVodList(const QString &sResp, QVector<cparser::SVodVideo> &vVodList, cparser::SGenreInfo &gInfo)
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
int CTVClubParser::parseVideoInfo(const QString &sResp, cparser::SVodVideo &vidInfo)
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
//! \param[in]    sResp (const QString &) ref. to response string
//! \param[out]   epgList (QVector<cparser::SEpg> &) ref. to epg data vector
//! \param[out]   pCid (int*) optional buffer for channel id ...
//
//! \return  0 --> ok; -1 --> any error
//---------------------------------------------------------------------------
int CTVClubParser::parseEpg (const QString &sResp, QVector<cparser::SEpg> &epgList)
{
   int  iRV = 0;
   bool bOk = false;
   cparser::SEpg      entry;
   QtJson::JsonObject contentMap;

   // clear vector ...
   epgList.clear();

   contentMap = QtJson::parse(sResp, bOk).toMap();

   if (bOk)
   {
       // get entry point ...
       contentMap = contentMap.value("epg").toMap();
       QtJson::JsonArray channels = contentMap.value("channels").toList();

       // request was for one channel only ...
       QtJson::JsonArray epg      = channels.at(0).toMap().value("epg").toList();

       foreach (const QVariant& rawEpg, epg)
       {
           QtJson::JsonObject epgEntry = rawEpg.toMap();
           entry.sName  = epgEntry.value("text").toString();
           entry.sDescr = epgEntry.value("description").toString();
           entry.uiGmt  = epgEntry.value("start").toUInt();
           entry.uiEnd  = epgEntry.value("end").toUInt();

           // remove html tag fragments ...
           QRegExp rx("<[^>]*>.*(</[^>]*>|$)");
           entry.sDescr.replace(rx, "");

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
int CTVClubParser::parseSetting(const QString& sResp, const QString &sName, QVector<int>& vValues, int& iActVal)
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
int CTVClubParser::parseEpgCurrent (const QString& sResp, QCurrentMap &currentEpg)
{
    int  iRV = 0, cid;
    bool bOk = false;
    cparser::SEpgCurrent          entry;
    QVector<cparser::SEpgCurrent> vEntries;
    QtJson::JsonObject            contentMap;
    cparser::SEpgChanInf          chanInf;

    // clear vector ...
    currentEpg.clear();
    mEpgChanInf.clear();

    contentMap = QtJson::parse(sResp, bOk).toMap();

    if (bOk)
    {
        // get entry point ...
        contentMap = contentMap.value("epg").toMap();

        foreach(const QVariant& rawChan, contentMap.value("channels").toList())
        {
            QtJson::JsonObject chan = rawChan.toMap();
            cid = chan.value("id").toInt();

            chanInf.mbProtected = !!chan.value("protected").toInt();
            chanInf.miArchHours = chan.value("records").toInt();
            mEpgChanInf[cid]    = chanInf;

            vEntries.clear();

            foreach(const QVariant& rawEpg, chan.value("epg").toList())
            {
                QtJson::JsonObject epgEntry = rawEpg.toMap();
                entry.sShow   = epgEntry.value("text").toString();
                entry.uiStart = epgEntry.value("start").toUInt();
                entry.uiEnd   = epgEntry.value("end").toUInt();

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

//---------------------------------------------------------------------------
//
//! \brief   parse settings
//
//! \param[in]      sResp (const QString &) ref. to response string
//! \param[in/out]  settings (CSettingsDlg &) ref. to settings dialog
//
//! \return  0 --> ok; -1 --> any error
//---------------------------------------------------------------------------
int CTVClubParser::parseSettings(const QString &sResp, CSettingsDlg &settings)
{
    bool ok;
    int  iRV = 0;
    QtJson::JsonObject root = QtJson::parse(sResp, ok).toMap();

    if (ok)
    {
        // due to historical reason server id is handled here as string...
        QString                currentSrv;
        cparser::SSrv          srv;
        QVector<cparser::SSrv> servers;

        QtJson::JsonObject current = root.value("settings").toMap().value("current").toMap();
        QtJson::JsonObject lists   = root.value("settings").toMap().value("lists").toMap();

        currentSrv = QString::number(current.value("server").toMap().value("id").toInt());

        foreach(const QVariant& rawSrv, lists.value("servers").toList())
        {
            QtJson::JsonObject srvData = rawSrv.toMap();
            srv.sName = tr("%1 (%2HP @ %3%)")
                    .arg(srvData.value("name").toString())
                    .arg(srvData.value("power").toInt())
                    .arg(srvData.value("load").toInt());

            srv.sIp = QString::number(srvData.value("id").toInt());

            servers.append(srv);
        }

        settings.SetStreamServerCbx(servers, currentSrv);
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

//------------------------------------------------------------
/// \brief get additional info for channels from EPG
/// \param epgChanInf buffer for data map
/// \return 0 -> ok; -1 -> no data
//------------------------------------------------------------
int CTVClubParser::addChanInfo(QEpgChanInfMap &epgChanInf)
{
    epgChanInf = mEpgChanInf;
    return epgChanInf.isEmpty() ? -1 : 0;
}
