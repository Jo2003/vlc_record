/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     civistats.h
 *
 *  @author   Jo2003
 *
 *  @date     03.01.2017
 *
 *  $Id$
 *
 *///------------------------- (c) 2017 by Jo2003  --------------------------
#ifndef __20170103_CIVISTATS_H
    #define __20170103_CIVISTATS_H

#include <QObject>
#include <QTimer>
#include <QTime>
#include "cplayer.h"

namespace ivistats
{
    ///
    /// \brief map containing pixel audit links
    ///
    typedef QMap<QString, QString> PixelAuditMap_t;

    ///
    /// \brief The SIviStats struct
    ///
    struct SIviStats
    {
        SIviStats() : mPost(false){}
        QString mUrl;       ///< url for request
        QString mContent;   ///< optional content data
        bool    mPost;      ///< post if tree; else get
    };

    ///
    /// \brief The content data
    ///
    struct SContentData
    {
        ///
        /// \brief init structure
        ///
        SContentData()
            : mContentId(-1), mAppVersion(-1),
              mPreRoll(-1), mEndCredits(-1)
        {}

        QString mUid;
        QString mWatchId;
        QString mDevice;
        QString mFormat;
        int     mIviUid;
        int     mContentId;
        int     mAppVersion;
        int     mPreRoll;
        int     mEndCredits;
        PixelAuditMap_t mPixAudit;
    };

    ///
    /// \brief The ELoadType enum
    ///
    enum ELoadType
    {
        LT_LOAD   = 1,  ///< initial load time
        LT_BUFFER = 2,  ///< buffer time
        LT_REWIND = 3,  ///< buffer after rewind
        LT_UNKNOWN      ///< wtf ...
    };
}

///
/// \brief The CIviStats class
///
class CIviStats : public QObject
{
    Q_OBJECT

public:
    CIviStats(QObject* parent = 0);
    virtual ~CIviStats();
    void setPlayer(CPlayer* pPlayer);


public slots:
    void start(const ivistats::SContentData& cntData);
    void end();

    void bufferStart();
    void bufferEnd();
    void rewindStart();
    void rewindEnd();
    // loadStart() functionality is also handled by start() function
    void loadEnd();

private slots:
    void secsTick();

protected:
    QString createStatsContent(int loadType = -1, int duration = -1);
    void sendLoadStats(ivistats::ELoadType t, QTime& measure);

    unsigned long          mSecCounter;
    QTimer                 mTmSecTick;
    ivistats::SContentData mContentData;
    CPlayer*               mpPlayer;
    QTime                  mMeasureLoad;
    QTime                  mMeasureBuffer;
    QTime                  mMeasureRewind;

signals:
    void sigStats(ivistats::SIviStats stats);
};

#endif // __20170103_CIVISTATS_H
