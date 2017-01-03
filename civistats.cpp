/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     civistats.cpp
 *
 *  @author   Jo2003
 *
 *  @date     03.01.2017
 *
 *  $Id$
 *
 *///------------------------- (c) 2017 by Jo2003  --------------------------

#include "civistats.h"
#include "qurlex.h"

//------------------------------------------------------------------------------
//! @brief      create object
//!
//! @param      parent  (optional) parent object
//------------------------------------------------------------------------------
CIviStats::CIviStats(QObject *parent)
 : QObject(parent)
{
    mpPlayer = NULL;
    mTmSecTick.setSingleShot(false);
    mTmSecTick.setInterval(1000);
    mSecCounter = 0;

    connect(&mTmSecTick, SIGNAL(timeout()), this, SLOT(secsTick()));
}

//------------------------------------------------------------------------------
//! @brief      destroy object
//------------------------------------------------------------------------------
CIviStats::~CIviStats()
{
}

//------------------------------------------------------------------------------
//! @brief      create object
//!
//! @param[in]  cntData  data to be used when sending statistics
//------------------------------------------------------------------------------
void CIviStats::start(const ivistats::SContentData &cntData)
{
    // stop tick timer ...
    mTmSecTick.stop();

    // start measure load time ...
    mMeasureLoad.start();
    mMeasureBuffer = QTime();
    mMeasureRewind = QTime();

    // reset seconds counter ...
    mSecCounter  = 0;

    // store content data ...
    mContentData = cntData;

    // start tick timer ...
    mTmSecTick.start();

    if (mContentData.mPixAudit.contains("start_video_real"))
    {
        ivistats::SIviStats stats;
        stats.mUrl = mContentData.mPixAudit.value("start_video_real");

        emit sigStats(stats);
    }
}

//------------------------------------------------------------------------------
//! @brief      end statistic stuff ...
//------------------------------------------------------------------------------
void CIviStats::end()
{
    // stop tick timer ...
    mTmSecTick.stop();

    ivistats::SIviStats stats;
    stats.mUrl     = "http://logger.ivi.ru/logger/content/time";
    stats.mPost    = true;
    stats.mContent = createStatsContent();

    emit sigStats(stats);

    if (mContentData.mPixAudit.contains("end_content"))
    {
        stats.mContent = "";
        stats.mPost    = false;
        stats.mUrl     = mContentData.mPixAudit.value("end_content");

        emit sigStats(stats);
    }
}

//------------------------------------------------------------------------------
//! @brief      start measure buffering time
//------------------------------------------------------------------------------
void CIviStats::bufferStart()
{
    mMeasureBuffer.start();
}

//------------------------------------------------------------------------------
//! @brief      end measure buffering time
//------------------------------------------------------------------------------
void CIviStats::bufferEnd()
{
    sendLoadStats(ivistats::LT_BUFFER, mMeasureBuffer);
}

//------------------------------------------------------------------------------
//! @brief      start measure rewind time
//------------------------------------------------------------------------------
void CIviStats::rewindStart()
{
    mMeasureRewind.start();
}

//------------------------------------------------------------------------------
//! @brief      end measure rewind time
//------------------------------------------------------------------------------
void CIviStats::rewindEnd()
{
    sendLoadStats(ivistats::LT_REWIND, mMeasureRewind);
}

//------------------------------------------------------------------------------
//! @brief      end measure load time time
//------------------------------------------------------------------------------
void CIviStats::loadEnd()
{
    sendLoadStats(ivistats::LT_LOAD, mMeasureLoad);
}

//------------------------------------------------------------------------------
//! @brief      seconds tick (called every second) [slot]
//------------------------------------------------------------------------------
void CIviStats::secsTick()
{
    bool sendStats = false;
    ivistats::SIviStats stats;

    // count second ...
    mSecCounter ++;

    if (mSecCounter <= 5)
    {
        // every second ...
        sendStats = true;
    }
    else if (mSecCounter <= 15)
    {
        // every 3 seconds ...
        if (!(mSecCounter % 3))
        {
            sendStats = true;
        }
    }
    else if ((mSecCounter <= 60) || (mSecCounter >= (unsigned long)(mContentData.mEndCredits - 60)))
    {
        // every 5 seconds ...
        if (!(mSecCounter % 5))
        {
            sendStats = true;
        }
    }
    else
    {
        // every 60 seconds ...
        if (!(mSecCounter % 60))
        {
            sendStats = true;
        }
    }

    if (sendStats)
    {
        stats.mUrl     = "http://logger.ivi.ru/logger/content/time";
        stats.mPost    = true;
        stats.mContent = createStatsContent();

        emit sigStats(stats);
    }

    // independent from position ...
    if (!(mSecCounter % 60))
    {
        // pixelaudit bla bla bla ...
        if (mContentData.mPixAudit.contains("minute_content"))
        {
            stats.mUrl     = mContentData.mPixAudit.value("minute_content");
            stats.mPost    = false;
            stats.mContent = "";

            emit sigStats(stats);
        }
    }
}

//------------------------------------------------------------------------------
//! @brief      store player instance
//!
//! @param[in]  pPlayer pointer to player instance
//------------------------------------------------------------------------------
void CIviStats::setPlayer(CPlayer *pPlayer)
{
    mpPlayer = pPlayer;
}

//------------------------------------------------------------------------------
//! @brief      end measure and send load statistics
//!
//! @param[in]  t ivistats::ELoadType load type
//! @param[in]  measure ref. to QTime instance
//------------------------------------------------------------------------------
void CIviStats::sendLoadStats(ivistats::ELoadType t, QTime &measure)
{
    int elapsed  = measure.elapsed();
    int duration = elapsed / 1000;

    if (((elapsed % 1000) >= 500) || (duration == 0))
    {
        duration ++;
    }

    // reset measure (make invalid) ...
    measure = QTime();

    ivistats::SIviStats stats;
    stats.mUrl     = "http://logger.ivi.ru/logger/content/load";
    stats.mPost    = true;
    stats.mContent = createStatsContent(t, duration);

    emit sigStats(stats);
}

//------------------------------------------------------------------------------
//! @brief      create statistics content
//!
//! @param[in]  loadType (optional) needed for extended stats
//! @param[in]  duration (optional) needed for extended stats
//!
//! @returns    content as string
//------------------------------------------------------------------------------
QString CIviStats::createStatsContent(int loadType, int duration)
{
    QUrlEx  content;

    content.addQueryItem("uid"         , mContentData.mUid.toUtf8());
    content.addQueryItem("iviuid"      , mContentData.mIviUid.toUtf8());
    content.addQueryItem("watchid"     , mContentData.mWatchId.toUtf8());
    content.addQueryItem("device"      , mContentData.mWatchId.toUtf8());
    content.addQueryItem("app_version" , QString::number(mContentData.mAppVersion).toUtf8());
    content.addQueryItem("contentid"   , QString::number(mContentData.mContentId).toUtf8());
    content.addQueryItem("seconds"     , QString::number(mSecCounter).toUtf8());
    content.addQueryItem("fromstart"   , QString::number(mpPlayer->getMediaPosition()).toUtf8());

    if (loadType != -1)
    {
        content.addQueryItem("type_id"        , QString::number(loadType).toUtf8());
        content.addQueryItem("duration"       , QString::number(duration).toUtf8());
        content.addQueryItem("content_format" , mContentData.mFormat.toUtf8());
    }

    return content.query();
}
