/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     civiapi.h
 *
 *  @author   Jo2003
 *
 *  @date     18.11.2016
 *
 *  $Id$
 *
 *///------------------------- (c) 2016 by Jo2003  --------------------------
#ifndef __20161118_CIVIAPI_H
    #define __20161118_CIVIAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QMap>
#include <QTimer>
#include <QSslError>
#include "ccmac_bf.h"
#include "cparser.h"
#include "civistats.h"

#define IVI_REQ_ID            "reqid"
#define IVI_GETCONTENT_TMPL   "{\"params\":[\"%1\", {\"session\": \"%2\", \"app_version\": %3}], \"method\":\"da.content.get\"}"
#define IVI_GETTIMESTAMP_TMPL "{\"method\": \"da.timestamp.get\", \"params\": []}"

namespace ivi {

    // looking forward ...
    struct SGenre;
    struct SCat;
    struct SCountry;

    typedef QMap<int, SCat>     CategoryMap;
    typedef QMap<int, SGenre>   GenreMap;
    typedef QMap<int, SCountry> CountryMap;

    enum eIviReq {
        IVI_APP_VER,
        IVI_SESSION,
        IVI_GENRES,
        IVI_VIDEOS,
        IVI_VIDEOINFO,
        IVI_COMPINFO,
        IVI_VIDEO_PERSONS,
        IVI_COMP_PERSONS,
        IVI_FILES,
        IVI_COUNTRIES,
        IVI_TIMESTAMP,
        IVI_ADD_FAV,
        IVI_DEL_FAV,
        IVI_ALL_FAV,
        IVI_FAV_COUNT,
        IVI_FAVOURITES,
        IVI_STATS,
        IVI_UNKNOWN=256
    };

    enum eKind {
        KIND_UNKNOWN     = 0,
        KIND_VIDEO       = 1,
        KIND_COMPILATION = 2
    };

    struct SCat {
        QString  mTitle;
        int      mId;
        GenreMap mGenres;
    };

    struct SGenre {
        QString mTitle;
        int     mId;
        int     mCatId;
        int     mCount;
        int     mNoCompilations;
        int     mNoContent;
    };

    struct SVideoFilter {
        SVideoFilter() :mGenId(-1), mCatId(-1), mFrom(-1), mTo(-1), mCompId(-1){}
        int     mGenId;
        int     mCatId;
        int     mFrom;
        int     mTo;
        int     mCompId;
        QString mSort;
        QString mSearch;
    };

    struct SCountry {
        int     mId;
        QString mName;
        QString mShort;
    };
}

///
/// \brief The CIviApi class
///
class CIviApi : public QNetworkAccessManager
{
    Q_OBJECT

public:
    CIviApi(QObject *parent = 0);
    virtual ~CIviApi();

    void setVerimatrixKey(const QString& key);
    int login();

    // get / set
    int getGenres();
    int getCountries();
    int getVideos(const ivi::SVideoFilter& filter);
    int searchVideos(const ivi::SVideoFilter& filter);
    int getVideoInfo(int id, ivi::eKind kind);
    int getVideoFromCompilation(const ivi::SVideoFilter& filter);
    int getFiles(int id);
    int getVideoPersons(int id, ivi::eKind kind);
    int addFav(int id, ivi::eKind kind);
    int delFav(int id, ivi::eKind kind);
    int getFavCount();
    int getFavourites(int offset = -1);
    int favCount() const;
    int getRealAppVersion();

    // parse
    int parseSession(const QString& resp);
    int parseGenres(const QString& resp);
    int parseCountries(const QString& resp);
    int parseVideos(const QString& resp);
    int parseVideoInfo(const QString& resp, ivi::eIviReq req);
    int parseFiles(const QString& resp);
    int parseTimeStamp(const QString& resp);
    int parseVideoPersons(const QString& resp, ivi::eIviReq req);
    int parseFavCount(const QString& resp);
    int parseAllFavs(const QString& resp);
    int parseRealAppVer(const QString& resp);

    void combineInfo();

protected:
    int  hasError(const QString& resp, QString& errText);

signals:
    void sigCategories(ivi::CategoryMap cats);
    void sigCountries(ivi::CountryMap countr);
    void sigVideoList(cparser::VideoList vidoes);
    void sigVideoInfo(cparser::SVodVideo video);
    void sigError(int iType, const QString& cap, const QString& descr);

public slots:
    int getTimeStamp();
    int sendIviStats(ivistats::SIviStats stats);

private slots:
    void getReply(QNetworkReply* reply);
    void slotSslError(QNetworkReply* pReply,QList<QSslError> errors);

public slots:

protected:
    ivi::CountryMap  mCountries;
    ivi::GenreMap    mGenres;

private:
    QString            mProtocol;
    QString            mHost;
    QString            mQueryPrefix;
    QString            mVerimatrix;
    QString            mSessionKey;
    QString            mTs;
    CCMAC_Bf          *pHash;
    cparser::SVodVideo mCurrentVideo;
    cparser::SVodVideo mCompilationInfo;
    ivi::SVideoFilter  mCompFilter;
    int                mFavCount;
    QVector<int>       mFavourites;
    int                mRealAppVer;
};

#endif // __20161118_CIVIAPI_H
