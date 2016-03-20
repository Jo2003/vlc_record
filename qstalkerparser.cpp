/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qstalkerparser.cpp
 *
 *  @author   Jo2003
 *
 *  @date     06.09.2015
 *
 *  $Id$
 *
 *///------------------------- (c) 2015 by Jo2003  --------------------------
#include "qstalkerparser.h"

// log file functions ...
extern CLogFile VlcLog;

//---------------------------------------------------------------------------
//! \brief   constructor
//
//! \author  Jo2003
//! \date    19.10.2015
//
//! \param   [in] parent (QObject*) parent pointer
//---------------------------------------------------------------------------
QStalkerParser::QStalkerParser(QObject *parent)
    : CStdJsonParser(parent)
{
}

//---------------------------------------------------------------------------
//
//! \brief   parse authetication
//
//! \author  Jo2003
//! \date    19.10.2015
//
//! \param   [in] sResp (const QString &) ref. to response string
//! \param   [out] auth (cparser::SAuth&) ref. to auth struct
//
//! \return  0 --> ok; -1 --> any error
//---------------------------------------------------------------------------
int QStalkerParser::parseAuth(const QString &sResp, cparser::SAuth& auth)
{
    int  iRV = 0;
    bool bOk = false;
    QVariantMap contentMap;

    contentMap = QtJson::parse(sResp, bOk).toMap();

    if (bOk)
    {
        auth.toktype      = contentMap.value("token_type").toString();
        auth.token        = contentMap.value("access_token").toString();
        auth.expires      = contentMap.value("expires_in").toInt();
        auth.refreshToken = contentMap.value("refresh_token").toString();
        auth.userId       = contentMap.value("user_id").toInt();
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
//! \brief   parse user settings
//
//! \author  Jo2003
//! \date    19.10.2015
//
//! \param   [in] sResp (const QString &) ref. to response string
//! \param   [out] stalkSet (QStalkerSettings) settings related to stalker API
//
//! \return  0 --> ok; -1 --> any error
//---------------------------------------------------------------------------
int QStalkerParser::parseUserSettings(const QString &sResp, QStalkerSettings &stalkSet)
{
    int  iRV = 0;
    bool bOk = false;
    QVariantMap contentMap;

    contentMap = QtJson::parse(sResp, bOk).toMap();

    if (bOk)
    {
        contentMap = contentMap.value("results").toMap();

        foreach(const QString& key, contentMap.keys())
        {
            stalkSet[key] = contentMap.value(key).toString();
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
int QStalkerParser::parseChannelList (const QString &sResp,
                                      QVector<cparser::SChan> &chanList,
                                      bool bFixTime)
{
    Q_UNUSED(bFixTime)
    int  iRV = 0;
    bool bOk = false;
    cparser::SChan chan;
    QVariantMap    contentMap;
    int iGrpIdx = 0;

    // clear channel list ...
    chanList.clear();

    contentMap = QtJson::parse(sResp, bOk).toMap();

    if (bOk)
    {
        foreach (const QVariant& groupListEntry, contentMap.value("results").toList())
        {
            QVariantMap mGroup = groupListEntry.toMap();
            initChanEntry(chan, false);

            chan.sName = mGroup.value("title").toString();
            checkColor(chan.sProgramm, iGrpIdx++);
            chanList.append(chan);

            foreach (const QVariant& chanListEntry, mGroup.value("channels").toList())
            {
                QVariantMap mChan = chanListEntry.toMap();
                initChanEntry(chan);

                chan.iId          = mChan.value("id").toInt();
                chan.sName        = mChan.value("name").toString().simplified();
                chan.bIsVideo     = true;
                chan.bHasArchive  = mChan.value("archive").toBool();
                chan.bIsProtected = mChan.value("censored").toBool();
                chan.sIcon        = mChan.value("logo").toString();
                chan.bIsHidden    = !mChan.value("monitoring_status").toBool();
                chan.url          = mChan.value("url").toString();
                chan.iArchRange   = mChan.value("archive_range").toInt();

                QVariantMap mEpg  = mChan.value("epg").toMap();
                chan.uiStart      = mEpg.value("start").toUInt();
                chan.uiEnd        = mEpg.value("end").toUInt();
                chan.sProgramm    = mEpg.value("name").toString();
                chanList.append(chan);

                mInfo(tr("We've got channel %1 (%2)").arg(chan.sName).arg(chan.iId));
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
int QStalkerParser::parseEpg (const QString &sResp, QVector<cparser::SEpg> &epgList)
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
      foreach (const QVariant& lEpg, contentMap.value("results").toList())
      {
         QVariantMap mEpg = lEpg.toMap();

         entry.sDescr = "";
         entry.uiGmt  = mEpg.value("start").toUInt();
         entry.uiEnd  = mEpg.value("end").toUInt();
         sTmp         = mEpg.value("name").toString();

         if (sTmp.contains('\n'))
         {
            entry.sName  = sTmp.left(sTmp.indexOf('\n'));
            entry.sDescr = sTmp.mid(sTmp.indexOf('\n') + 1);
         }
         else
         {
            entry.sName = sTmp;
         }

         entry.id = mEpg.contains("id") ? mEpg.value("id").toInt() : -1;

         mInfo(tr("EPG: %1 (#%2)").arg(entry.sName).arg(entry.id));

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
int QStalkerParser::parseUrl(const QString &sResp, QString &sUrl)
{
    int  iRV = 0;
    bool bOk = false;
    QVariantMap contentMap;

    contentMap = QtJson::parse(sResp, bOk).toMap();

    if (bOk)
    {
        if (contentMap.contains("url"))
        {
            sUrl = contentMap.value("url").toString();
        }
        else if (contentMap.contains("results"))
        {
            sUrl = contentMap.value("results").toString();
        }
        else
        {
            mInfo(tr("No url found in '%1'!").arg(sResp));
            iRV = -1;
        }

        if (iRV == 0)
        {
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
//! \date    20.03.2016
//
//! \param   sResp (const QString &) ref. to response string
//! \param   currentEpg (QCurrentMap &) ref. to epg data map
//
//! \return  0 --> ok; -1 --> any error
//---------------------------------------------------------------------------
int QStalkerParser::parseEpgCurrent (const QString& sResp, QCurrentMap &currentEpg)
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
      foreach (const QVariant& lEpg1, contentMap.value("results").toList())
      {
         QVariantMap mEpg1 = lEpg1.toMap();

         vEntries.clear();

         cid = mEpg1.value("cid").toInt();

         foreach (const QVariant& lEpg2, mEpg1.value("epg").toList())
         {
            QVariantMap mEpg2 = lEpg2.toMap();

            entry.sShow   = mEpg2.value("name").toString();
            entry.uiStart = mEpg2.value("start").toUInt();
            entry.uiEnd   = mEpg2.value("end").toUInt();

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

