#ifndef CKARTINAADS_H
#define CKARTINAADS_H

#include <QTimer>
#include <QNetworkAccessManager>
#include <QMap>
#include "cplayer.h"

namespace ads
{
    ///
    /// \brief The SChanAds struct
    ///
    struct SChanAds
    {
        int mShowTime;      ///< show ad for x seconds
        int mStartPause;    ///< show ad x seconds after start / pause
        int mPause;         ///< pause between ads
    };

    typedef QMap<int, SChanAds> AddChanMap;

    ///
    /// \brief actions for Ads
    ///
    enum EActions
    {
        ADS_POLICY,
        ADS_GET   ,
        ADS_UNKNOWN
    };
}


//------------------------------------------------------------------------------
//! @brief      Class for kartina ads.
//------------------------------------------------------------------------------
class CKartinaAds : public QNetworkAccessManager
{
    Q_OBJECT

public:
    CKartinaAds(QObject *parent = 0);
    virtual ~CKartinaAds();
    void setPlayer(CPlayer *pPlay);
    void setLogin(const QString& s);

public slots:
    void channelChanged(int cid);

private:
    CPlayer*        mpPlayer;
    QTimer          mTick;
    int             mCacheTime;
    QString         mLogin;
    ads::AddChanMap mAddChanMap;
    int             mCurrentCid;
    int             mAdCounter;
};

#endif // CKARTINAADS_H
