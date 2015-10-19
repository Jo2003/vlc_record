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
        // no groups -> define our own ...
        initChanEntry(chan, false);

        chan.iId       = 1;
        chan.sName     = tr("Programs");

        // make sure group color isn't black ...
        checkColor(chan.sProgramm, iGrpIdx++);
        chanList.append(chan);

        foreach (const QVariant& lChan, contentMap.value("results").toList())
        {
            QVariantMap mChan = lChan.toMap();
            initChanEntry(chan);

            chan.iId          = mChan.value("id").toInt();
            chan.sName        = mChan.value("name").toString().simplified();
            chan.bIsVideo     = true;
            chan.bHasArchive  = mChan.value("archive").toBool();
            chan.bIsProtected = mChan.value("censored").toBool();
            chan.sIcon        = mChan.value("logo").toString();
            chan.bIsHidden    = !mChan.value("monitoring_status").toBool();
            chan.url          = mChan.value("url").toString();
            chanList.append(chan);

            mInfo(tr("We've got channel %1 (%2)").arg(chan.sName).arg(chan.iId));
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

