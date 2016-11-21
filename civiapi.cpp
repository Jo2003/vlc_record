/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     civiapi.cpp
 *
 *  @author   Jo2003
 *
 *  @date     18.11.2016
 *
 *  $Id$
 *
 *///------------------------- (c) 2016 by Jo2003  --------------------------

#include "civiapi.h"
#include <QtJson>
#include "externals_inc.h"

#define IVI_APP_VERSION       4856                                  ///< replace with correct data!
#define IVI_KEY               "99328c878d1d6eb9e02a8f80470390e4"    ///< replace with correct data!
#define IVI_K1                "e3572989c2407c14"                    ///< replace with correct data!
#define IVI_K2                "c6ae53138480f833"                    ///< replace with correct data!

//------------------------------------------------------------------------------
//! @brief      Constructs the object.
//!
//! @param      parent  The parent
//------------------------------------------------------------------------------
CIviApi::CIviApi(QObject *parent) :
    QNetworkAccessManager(parent)
{
    mProtocol    = "https";
    mHost        = "api.ivi.ru";
    mQueryPrefix = "mobileapi";
    pHash        = new CCMAC_Bf(IVI_KEY, IVI_K1, IVI_K2);
    connect(this, SIGNAL(finished(QNetworkReply*)), this, SLOT(getReply(QNetworkReply*)));
}

//------------------------------------------------------------------------------
//! @brief      Destroys the object.
//------------------------------------------------------------------------------
CIviApi::~CIviApi()
{
    if (pHash)
    {
        delete pHash;
        pHash = NULL;
    }
}

//------------------------------------------------------------------------------
//! @brief      Sets the session key.
//!
//! @param[in]  key   The key
//------------------------------------------------------------------------------
void CIviApi::setSessionKey(const QString &key)
{
    mSessionKey = key;
}

//------------------------------------------------------------------------------
//! @brief      Gets the genres.
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CIviApi::getGenres()
{
    // "https://api.ivi.ru/mobileapi/categories/v5/?session=sesstoken"
    QString req = QString("%1://%2/%3/categories/v5/?session=%4")
            .arg(mProtocol)
            .arg(mHost)
            .arg(mQueryPrefix)
            .arg(mSessionKey);

#ifdef __TRACE
    mInfo(req);
#endif

    QNetworkRequest request;
    request.setUrl(QUrl(req));

    QNetworkReply* pReply = QNetworkAccessManager::get(request);

    if (pReply)
    {
        pReply->setProperty(IVI_REQ_ID, (int)ivi::IVI_GENRES);
    }

    return pReply ? 0 : -1;
}

//------------------------------------------------------------------------------
//! @brief      Gets the countries.
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CIviApi::getCountries()
{
    // https://api.ivi.ru/mobileapi/countries/v6/?session=sesstoken"
    QString req = QString("%1://%2/%3/countries/v6/?session=%4")
            .arg(mProtocol)
            .arg(mHost)
            .arg(mQueryPrefix)
            .arg(mSessionKey);

#ifdef __TRACE
    mInfo(req);
#endif

    QNetworkRequest request;
    request.setUrl(QUrl(req));

    QNetworkReply* pReply = QNetworkAccessManager::get(request);

    if (pReply)
    {
        pReply->setProperty(IVI_REQ_ID, (int)ivi::IVI_COUNTRIES);
    }

    return pReply ? 0 : -1;
}

//------------------------------------------------------------------------------
//! @brief      Gets the videos.
//!
//! @param[in]  filter  video list filter
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CIviApi::getVideos(const ivi::SVideoFilter &filter)
{
    // "https://api.ivi.ru/mobileapi/videos/v5/?session=sesstoken"
    QString req = QString("%1://%2/%3/videos/v5/?session=%4")
            .arg(mProtocol)
            .arg(mHost)
            .arg(mQueryPrefix)
            .arg(mSessionKey);

    // add filter stuff ...
    req += QString("&sort=%1").arg(filter.mSort);
    req += QString("&from=%1").arg(filter.mFrom);
    req += QString("&to=%1").arg(filter.mTo);
    req += QString("&genre=%1").arg(filter.mGenId);

#ifdef __TRACE
    mInfo(req);
#endif

    QNetworkRequest request;
    request.setUrl(QUrl(req));

    QNetworkReply* pReply = QNetworkAccessManager::get(request);

    if (pReply)
    {
        pReply->setProperty(IVI_REQ_ID, (int)ivi::IVI_VIDEOS);
    }

    return pReply ? 0 : -1;
}

//------------------------------------------------------------------------------
//! @brief      Gets the video information.
//!
//! @param[in]  id    The identifier
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CIviApi::getVideoInfo(int id)
{
    // "https://api.ivi.ru/mobileapi/videoinfo/v6/?session=sesstoken&id=1"
    QString req = QString("%1://%2/%3/videoinfo/v6/?session=%4")
            .arg(mProtocol)
            .arg(mHost)
            .arg(mQueryPrefix)
            .arg(mSessionKey);

    // add id ...
    req += QString("&id=%1").arg(id);

#ifdef __TRACE
    mInfo(req);
#endif

    QNetworkRequest request;
    request.setUrl(QUrl(req));

    QNetworkReply* pReply = QNetworkAccessManager::get(request);

    if (pReply)
    {
        pReply->setProperty(IVI_REQ_ID, (int)ivi::IVI_VIDEOINFO);
    }

    return pReply ? 0 : -1;
}

//------------------------------------------------------------------------------
//! @brief      Gets the content files.
//!
//! @param[in]  id    The identifier
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CIviApi::getFiles(int id)
{
    QString url = QString("%1://%2/light/")
            .arg(mProtocol)
            .arg(mHost);

    QString postData = QString(IVI_GETCONTENT_TMPL)
            .arg(id)
            .arg(mSessionKey)
            .arg(IVI_APP_VERSION);

    url += QString("?app_version=%1").arg(IVI_APP_VERSION);
    url += QString("&ts=%1").arg(mTs);
    url += QString("&sign=%1").arg(pHash->sign(postData));

#ifdef __TRACE
    mInfo(tr("Post '%1' to url '%2'").arg(postData).arg(url));
#endif

    QNetworkRequest request;
    request.setUrl(QUrl(url));

    QNetworkReply* pReply = QNetworkAccessManager::post(request, postData.toUtf8());

    if (pReply)
    {
        pReply->setProperty(IVI_REQ_ID, (int)ivi::IVI_FILES);
    }

    return pReply ? 0 : -1;
}

//------------------------------------------------------------------------------
//! @brief      Gets the time stamp.
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CIviApi::getTimeStamp()
{
    QString url = QString("%1://%2/light/")
            .arg(mProtocol)
            .arg(mHost);

    QString postData = IVI_GETTIMESTAMP_TMPL;

#ifdef __TRACE
    mInfo(tr("Post '%1' to url '%2'").arg(postData).arg(url));
#endif

    QNetworkRequest request;
    request.setUrl(QUrl(url));

    QNetworkReply* pReply = QNetworkAccessManager::post(request, postData.toUtf8());

    if (pReply)
    {
        pReply->setProperty(IVI_REQ_ID, (int)ivi::IVI_TIMESTAMP);
    }

    return pReply ? 0 : -1;
}


//------------------------------------------------------------------------------
//! @brief      parse ivi categories and genres
//!
//! @param[in]  resp  http response
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CIviApi::parseGenres(const QString &resp)
{
    mInfo(tr("We've got genres response ..."));

    int              iRV = 0;
    bool             bOk;
    QVariantMap      contentMap;
    ivi::CategoryMap cats;
    ivi::SCat        cat;
    ivi::SGenre      genre;

    mGenres.clear();

    contentMap = QtJson::parse(resp, bOk).toMap();

    if (bOk)
    {
        // ivi categories ...
        foreach (const QVariant& varCat, contentMap.value("result").toList())
        {
            QVariantMap mCat = varCat.toMap();

            cat.mGenres.clear();
            cat.mId    = mCat.value("id").toInt();
            cat.mTitle = mCat.value("title").toString();

            // ivi genres ...
            foreach (const QVariant& varGenre, mCat.value("genres").toList())
            {
                QVariantMap mGenre = varGenre.toMap();
                genre.mId             = mGenre.value("id").toInt();
                genre.mTitle          = mGenre.value("title").toString();
                genre.mCatId          = mGenre.value("category_id").toInt();
                genre.mCount          = mGenre.value("catalogue_count").toInt();
                genre.mNoCompilations = mGenre.value("compilation_count").toInt();
                genre.mNoContent      = mGenre.value("content_count").toInt();

                cat.mGenres.insert(genre.mId, genre);
                mGenres.insert(genre.mId, genre);
            }

            cats.insert(cat.mId, cat);
        }

        emit sigCategories(cats);
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

//------------------------------------------------------------------------------
//! @brief      parse ivi countries
//!
//! @param[in]  resp  http response
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CIviApi::parseCountries(const QString &resp)
{
    mInfo(tr("Parse IVI countries ..."));
    int              iRV = 0;
    bool             bOk;
    QVariantMap      contentMap;
    ivi::SCountry    country;

    mCountries.clear();

    contentMap = QtJson::parse(resp, bOk).toMap();

    if (bOk)
    {
        contentMap = contentMap.value("result").toMap();

        foreach(QString key, contentMap.keys())
        {
            QVariantMap mDetails = contentMap.value(key).toMap();

            country.mId    = key.toInt();
            country.mName  = mDetails.value("title").toString();
            country.mShort = mDetails.value("code").toString();

            mCountries.insert(country.mId, country);
        }

        emit sigCountries(mCountries);
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

//------------------------------------------------------------------------------
//! @brief      parse ivi vidoes
//!
//! @param[in]  resp  http response
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CIviApi::parseVideos(const QString &resp)
{
    mInfo(tr("Parse IVI videos ..."));
    int                  iRV = 0;
    bool                 bOk;
    QVariantMap          contentMap;
    cparser::VideoList   videos;
    cparser::SVodVideo   video;

    contentMap = QtJson::parse(resp, bOk).toMap();

    if (bOk)
    {
        // video by video
        foreach (const QVariant& varVideo, contentMap.value("result").toList())
        {
            QVariantMap mVideo = varVideo.toMap();
            video.sImg     = "";
            video.uiVidId  = mVideo.value("id").toUInt();
            video.sName    = mVideo.value("title").toString();
            video.sYear    = QString::number(mVideo.value("year").toInt());
            video.sCountry = mCountries.value(mVideo.value("country").toInt()).mName;

            QVariantList posters = mVideo.value("poster_originals").toList();

            if (posters.size() > 0)
            {
                // get first poster ...
                QVariantMap poster = posters.value(0).toMap();

                video.sImg = poster.value("path").toString();
            }

            videos.append(video);
        }

        emit sigVideoList(videos);
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

//------------------------------------------------------------------------------
//! @brief      parse ivi video info
//!
//! @param[in]  resp  http response
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CIviApi::parseVideoInfo(const QString &resp)
{
    mInfo(tr("Parse IVI video info ..."));
    int                  iRV = 0;
    bool                 bOk;
    QVariantMap          contentMap;
    cparser::SVodVideo   video;

    video.bProtected       = false;
    video.bFavourit        = false;

    contentMap = QtJson::parse(resp, bOk).toMap();

    if (bOk)
    {
        QVariantMap mVideo = contentMap.value("result").toMap();
        video.uiVidId  = mVideo.value("id").toUInt();
        video.sName    = mVideo.value("title").toString();

        if (!mVideo.value("orig_title").toString().isEmpty())
        {
            video.sName += " (" + mVideo.value("orig_title").toString() + ")";
        }

        video.sYear    = QString::number(mVideo.value("year").toInt());
        video.sCountry = mCountries.value(mVideo.value("country").toInt()).mName;

        // genres ...
        foreach(QVariant rawGenre, mVideo.value("genres").toList())
        {
            int genId = rawGenre.toInt();
            if (!video.sGenres.isEmpty())
            {
                video.sGenres += ", ";
            }
            video.sGenres += mGenres.value(genId).mTitle;
        }

        // actors ...
        foreach(QVariant rawActor, mVideo.value("artists").toList())
        {
            QString actor = rawActor.toString();
            if (!video.sActors.isEmpty())
            {
                video.sActors += ", ";
            }
            video.sActors += actor;
        }

        // description ...
        video.sDescr           = mVideo.value("description").toString();
        video.uiLength         = mVideo.value("duration_minutes").toInt();
        video.sImdbRating      = mVideo.value("imdb_rating").toString();
        video.sKinopoiskRating = mVideo.value("kp_rating").toString();

        QVariantList posters = mVideo.value("poster_originals").toList();

        if (posters.size() > 0)
        {
            // get first poster ...
            QVariantMap poster = posters.value(0).toMap();

            video.sImg = poster.value("path").toString();
        }

        getFiles(video.uiVidId);

        emit sigVideoInfo(video);
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

//------------------------------------------------------------------------------
//! @brief      parse ivi content files
//!
//! @param[in]  resp  files response
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CIviApi::parseFiles(const QString &resp)
{
    mInfo(tr("Parse IVI file info ..."));
}

//------------------------------------------------------------------------------
//! @brief      parse time stamp reply
//!
//! @param[in]  resp  ivi response to parse
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CIviApi::parseTimeStamp(const QString &resp)
{
    mInfo(tr("Parse IVI time stamp ..."));
    int                  iRV = 0;
    bool                 bOk;
    QVariantMap          contentMap;

    contentMap = QtJson::parse(resp, bOk).toMap();

    if (bOk)
    {
        mTs = contentMap.value("result").toString();

        mInfo(tr("Using time stamp '%1' for the next 9 minutes ...").arg(mTs));

        // update in 9 minutes ...
        QTimer::singleShot(9 * 60 * 1000, this, SLOT(getTimeStamp()));
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

//------------------------------------------------------------------------------
//! @brief      Gets the network reply.
//!
//! @param      reply  pointer to network reply
//------------------------------------------------------------------------------
void CIviApi::getReply(QNetworkReply *reply)
{
    ivi::eIviReq req = (ivi::eIviReq)reply->property(IVI_REQ_ID).toInt();

    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray ba   = reply->readAll();
        QString    resp = QString::fromUtf8(ba.constData());

#ifdef __TRACE
        mInfo(tr("IVI Response:\n ==8<==8<==8<==\n%1\n ==>8==>8==>8==")
                 .arg(resp));
#endif // __TRACE

        switch(req)
        {
        case ivi::IVI_GENRES:
            parseGenres(resp);
            break;
        case ivi::IVI_VIDEOS:
            parseVideos(resp);
            break;
        case ivi::IVI_COUNTRIES:
            parseCountries(resp);
            break;
        case ivi::IVI_VIDEOINFO:
            parseVideoInfo(resp);
            break;
        case ivi::IVI_FILES:
            parseFiles(resp);
            break;
        case ivi::IVI_TIMESTAMP:
            parseTimeStamp(resp);
            break;
        default:
            break;
        }
    }
    else
    {
        mInfo(tr("Network error: #%1: %2").arg((int)reply->error()).arg(reply->errorString()));
    }

    // mark for deletion ...
    reply->deleteLater();
}
