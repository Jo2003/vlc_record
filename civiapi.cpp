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
#include "ivi_kartina_credits.h"

//------------------------------------------------------------------------------
//! @brief      Constructs the object.
//!
//! @param      parent  The parent
//------------------------------------------------------------------------------
CIviApi::CIviApi(QObject *parent) :
    QNetworkAccessManager(parent)
{
    mFavCount    = -1;
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
void CIviApi::setVerimatrixKey(const QString &key)
{
    mVerimatrix = key;

    // "https://api.ivi.ru/mobileapi/user/login/verimatrix/v2?verimatrix=abc&app_version=1234"
    QString req = QString("%1://%2/%3/user/login/verimatrix/v5?verimatrix=%4")
            .arg(mProtocol)
            .arg(mHost)
            .arg(mQueryPrefix)
            .arg(mVerimatrix);

    // add app_version ...
    req += QString("&app_version=%1").arg(IVI_APP_VERSION);

#ifdef __TRACE
    mInfo(req);
#endif

    QNetworkRequest request;
    request.setUrl(QUrl(req));

    QNetworkReply* pReply = QNetworkAccessManager::get(request);

    if (pReply)
    {
        pReply->setProperty(IVI_REQ_ID, (int)ivi::IVI_SESSION);
    }
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

    // add app_version ...
    req += QString("&app_version=%1").arg(IVI_APP_VERSION);

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

    // add app_version ...
    req += QString("&app_version=%1").arg(IVI_APP_VERSION);

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
    if (!filter.mSearch.isEmpty())
    {
        return searchVideos(filter);
    }
    else if (filter.mSort == "favorites")
    {
        return getFavourites(filter.mFrom);
    }

    // "https://api.ivi.ru/mobileapi/catalogue/v5/?session=sesstoken"
    QString req = QString("%1://%2/%3/catalogue/v5/?session=%4")
            .arg(mProtocol)
            .arg(mHost)
            .arg(mQueryPrefix)
            .arg(mSessionKey);

    // add app_version ...
    req += QString("&app_version=%1").arg(IVI_APP_VERSION);

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
//! @brief      search for videos
//!
//! @param[in]  filter  video list filter
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CIviApi::searchVideos(const ivi::SVideoFilter &filter)
{
    // /search/v5/?(str: query)[&(int: subsite)][&(int: app_version)][&(int: from)][&(int: to)][&(int: category)][&(int: genre)][&(i
    QString req = QString("%1://%2/%3/search/v5/?session=%4")
            .arg(mProtocol)
            .arg(mHost)
            .arg(mQueryPrefix)
            .arg(mSessionKey);

    // add app_version ...
    req += QString("&app_version=%1").arg(IVI_APP_VERSION);

    // add filter stuff ...
    req += QString("&from=%1").arg(filter.mFrom);
    req += QString("&to=%1").arg(filter.mTo);
    req += QString("&genre=%1").arg(filter.mGenId);
    req += QString("&query=%1").arg(filter.mSearch);

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

    // add app_version ...
    req += QString("&app_version=%1").arg(IVI_APP_VERSION);

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
    url += QString("&sign=%1").arg(pHash->sign(mTs + postData));

#ifdef __TRACE
    mInfo(tr("Post '%1' to url '%2'").arg(postData).arg(url));
#endif

    QNetworkRequest request;
    request.setUrl(QUrl(url));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* pReply = QNetworkAccessManager::post(request, postData.toUtf8());

    if (pReply)
    {
        pReply->setProperty(IVI_REQ_ID, (int)ivi::IVI_FILES);
    }

    return pReply ? 0 : -1;
}

//------------------------------------------------------------------------------
//! @brief      Gets info about persons in video
//!
//! @param[in]  id    The identifier
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CIviApi::getVideoPersons(int id)
{
    // "https://api.ivi.ru/mobileapi/video/persons/v5/?session=sesstoken&id=7029"
    QString req = QString("%1://%2/%3/video/persons/v5/?session=%4")
            .arg(mProtocol)
            .arg(mHost)
            .arg(mQueryPrefix)
            .arg(mSessionKey);

    // add app_version ...
    req += QString("&app_version=%1").arg(IVI_APP_VERSION);

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
        pReply->setProperty(IVI_REQ_ID, (int)ivi::IVI_VIDEO_PERSONS);
    }

    return pReply ? 0 : -1;
}

//------------------------------------------------------------------------------
//! @brief      add video to favourites
//!
//! @param      id video id
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CIviApi::addFav(int id)
{
    // /video/favourite/v5/add?(int: id)&(str: session)&(int: subsite)&(int: app_version)
    QString req = QString("%1://%2/%3/video/favourite/v5/add")
            .arg(mProtocol)
            .arg(mHost)
            .arg(mQueryPrefix);

    // content  ...
    QString cont;
    cont += QString("app_version=%1").arg(IVI_APP_VERSION);
    cont += QString("&session=%1").arg(mSessionKey);
    cont += QString("&id=%1").arg(id);

#ifdef __TRACE
    mInfo(tr("Post '%1' to url '%2'").arg(cont).arg(req));
#endif

    QNetworkRequest request;
    request.setUrl(QUrl(req));

    QNetworkReply* pReply = QNetworkAccessManager::post(request, cont.toUtf8());

    if (pReply)
    {
        pReply->setProperty(IVI_REQ_ID, (int)ivi::IVI_ADD_FAV);
    }

    return pReply ? 0 : -1;
}

//------------------------------------------------------------------------------
//! @brief      remove video from favourites
//!
//! @param      id video id
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CIviApi::delFav(int id)
{
    QString req = QString("%1://%2/%3/video/favourite/v5/delete")
            .arg(mProtocol)
            .arg(mHost)
            .arg(mQueryPrefix);

    // content  ...
    QString cont;
    cont += QString("app_version=%1").arg(IVI_APP_VERSION);
    cont += QString("&session=%1").arg(mSessionKey);
    cont += QString("&id=%1").arg(id);

#ifdef __TRACE
    mInfo(tr("Post '%1' to url '%2'").arg(cont).arg(req));
#endif

    QNetworkRequest request;
    request.setUrl(QUrl(req));

    QNetworkReply* pReply = QNetworkAccessManager::post(request, cont.toUtf8());

    if (pReply)
    {
        pReply->setProperty(IVI_REQ_ID, (int)ivi::IVI_DEL_FAV);
    }

    return pReply ? 0 : -1;
}

//------------------------------------------------------------------------------
//! @brief      get number of favourites
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CIviApi::getFavCount()
{
    // /user/favourites/v5/count?(str: session)[&(int: subsite)][&(int: app_version)][&(int: withunavailable)]
    QString req = QString("%1://%2/%3/user/favourites/v5/count?session=%4")
            .arg(mProtocol)
            .arg(mHost)
            .arg(mQueryPrefix)
            .arg(mSessionKey);

    // add app_version ...
    req += QString("&app_version=%1").arg(IVI_APP_VERSION);
    req += QString("&withunavailable=");

#ifdef __TRACE
    mInfo(req);
#endif

    QNetworkRequest request;
    request.setUrl(QUrl(req));

    QNetworkReply* pReply = QNetworkAccessManager::get(request);

    if (pReply)
    {
        pReply->setProperty(IVI_REQ_ID, (int)ivi::IVI_FAV_COUNT);
    }

    return pReply ? 0 : -1;
}

//------------------------------------------------------------------------------
//! @brief      get number of favourites
//!
//! @param      offset (optional) start at offset
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CIviApi::getFavourites(int offset)
{
    int from, to, reqId;

    if (offset == -1)
    {
        from  = 0;
        to    = mFavCount - 1;
        reqId = (int)ivi::IVI_ALL_FAV;
    }
    else
    {
        from = offset;
        to   = offset + (VIDEOS_PER_SITE - 1);
        reqId = (int)ivi::IVI_FAVOURITES;
    }

    // /user/favourites/v5/?(str: session)[&(int: subsite)][&(int: from)][&(int: to)][&(int: app_version)][&(int: showunavailable)]
    QString req = QString("%1://%2/%3/user/favourites/v5/?session=%4")
            .arg(mProtocol)
            .arg(mHost)
            .arg(mQueryPrefix)
            .arg(mSessionKey);

    // add app_version ...
    req += QString("&app_version=%1").arg(IVI_APP_VERSION);
    req += QString("&showunavailable=");
    req += QString("&from=%1").arg(from);
    req += QString("&to=%1").arg(to);

#ifdef __TRACE
    mInfo(req);
#endif

    QNetworkRequest request;
    request.setUrl(QUrl(req));

    QNetworkReply* pReply = QNetworkAccessManager::get(request);

    if (pReply)
    {
        pReply->setProperty(IVI_REQ_ID, reqId);
    }

    return pReply ? 0 : -1;
}

//------------------------------------------------------------------------------
//! @brief      return favourite count
//!
//! @return     number of favourites
//------------------------------------------------------------------------------
int CIviApi::favCount() const
{
    return mFavCount;
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
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply* pReply = QNetworkAccessManager::post(request, postData.toUtf8());

    if (pReply)
    {
        pReply->setProperty(IVI_REQ_ID, (int)ivi::IVI_TIMESTAMP);
    }

    return pReply ? 0 : -1;
}

//------------------------------------------------------------------------------
//! @brief      parse ivi login stuff
//!
//! @param[in]  resp  http response
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CIviApi::parseSession(const QString &resp)
{
    mInfo(tr("Parse IVI login respone ..."));
    int              iRV = 0;
    bool             bOk;
    QVariantMap      contentMap;

    contentMap = QtJson::parse(resp, bOk).toMap();

    if (bOk)
    {
        contentMap  = contentMap.value("result").toMap();
        mSessionKey = contentMap.value("session").toString();

        getCountries();
        getTimeStamp();

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
            video.iKind    = mVideo.value("kind").toInt();

            QStringList imgSrc;
            imgSrc << "poster_originals" << "thumbnails" << "thumb_originals";

            foreach (QString src, imgSrc)
            {
                QVariantList posters = mVideo.value(src).toList();

                if (posters.size() > 0)
                {
                    // get first poster ...
                    QVariantMap poster = posters.value(0).toMap();

                    video.sImg = poster.value("path").toString();

                    break;
                }
            }

            videos.append(video);
        }

        emit sigVideoList(videos);

        if (mFavCount == -1)
        {
            getFavCount();
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
        video.iKind    = mVideo.value("kind").toInt();

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

        QStringList imgSrc;
        imgSrc << "poster_originals" << "thumbnails" << "thumb_originals";

        foreach (QString src, imgSrc)
        {
            QVariantList posters = mVideo.value(src).toList();

            if (posters.size() > 0)
            {
                // get first poster ...
                QVariantMap poster = posters.value(0).toMap();

                video.sImg = poster.value("path").toString();

                break;
            }
        }

        video.bFavourit = mFavourites.contains((int)video.uiVidId);

        mCurrentVideo   = video;

        // request links ...
        getVideoPersons(video.uiVidId);
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
    int                   iRV = 0;
    bool                  bOk;
    QVariantMap           contentMap;
    cparser::SVodFileInfo fileInfo;

    contentMap = QtJson::parse(resp, bOk).toMap();

    if (bOk)
    {
        QVariantMap mVideo = contentMap.value("result").toMap();

        // files ...
        foreach(QVariant rawFile, mVideo.value("files").toList())
        {
            QVariantMap mFile = rawFile.toMap();

            fileInfo.iId    = mFile.value("id").toInt();
            fileInfo.sUrl   = mFile.value("url").toString();
            fileInfo.sTitle = mFile.value("content_format").toString();

            // try to get resolution from title ...
            if (fileInfo.sTitle.contains("hi"))
            {
                fileInfo.sFormat = "dvd";
            }
            else if (fileInfo.sTitle.contains("lo"))
            {
                fileInfo.sFormat = "tv";
            }
            else if (fileInfo.sTitle.contains("HD"))
            {
                fileInfo.sFormat = "fullhd";
            }
            else if (fileInfo.sTitle.contains("HQ"))
            {
                fileInfo.sFormat = "hq";
            }
            else
            {
                fileInfo.sFormat = "";
            }
            fileInfo.sCodec = "h264";
            mCurrentVideo.vVodFiles.append(fileInfo);
        }

        emit sigVideoInfo(mCurrentVideo);
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
//! @brief      parse video persons info
//!
//! @param[in]  resp  ivi response to parse
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CIviApi::parseVideoPersons(const QString &resp)
{
    mInfo(tr("We've got persons response ..."));
    int              iRV = 0;
    bool             bOk;
    QVariantMap      contentMap;

    contentMap = QtJson::parse(resp, bOk).toMap();

    if (bOk)
    {
        // ivi categories ...
        foreach (const QVariant& varPerson, contentMap.value("result").toList())
        {
            QVariantMap mPerson = varPerson.toMap();

            if (mPerson.value("id").toInt() == 3) // director
            {
                foreach (const QVariant& varDir, mPerson.value("persons").toList())
                {
                    QVariantMap mDirector = varDir.toMap();
                    if (!mCurrentVideo.sDirector.isEmpty())
                    {
                        mCurrentVideo.sDirector += ", ";
                    }

                    mCurrentVideo.sDirector += mDirector.value("name").toString();
                }

                break;
            }
        }

        getFiles(mCurrentVideo.uiVidId);
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
//! @brief      parse favourite count
//!
//! @param[in]  resp  ivi response to parse
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CIviApi::parseFavCount(const QString &resp)
{
    mInfo(tr("We've favourite count response ..."));
    int              iRV = 0;
    bool             bOk;
    QVariantMap      contentMap;

    contentMap = QtJson::parse(resp, bOk).toMap();

    if (bOk)
    {
        mFavCount = contentMap.value("result").toInt();
        getFavourites();
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
//! @brief      parse all favourites
//!
//! @param[in]  resp  ivi response to parse
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CIviApi::parseAllFavs(const QString &resp)
{
    mInfo(tr("parse all favourites ..."));
    int              iRV = 0;
    bool             bOk;
    QVariantMap      contentMap;

    contentMap = QtJson::parse(resp, bOk).toMap();

    if (bOk)
    {
        mFavourites.clear();

        foreach (const QVariant& rawFav, contentMap.value("result").toList())
        {
            QVariantMap mFav = rawFav.toMap();
            mFavourites.append(mFav.value("id").toInt());
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
        case ivi::IVI_SESSION:
            parseSession(resp);
            break;
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
        case ivi::IVI_VIDEO_PERSONS:
            parseVideoPersons(resp);
            break;
        case ivi::IVI_ADD_FAV:
            if (!resp.contains("error"))
            {
                mCurrentVideo.bFavourit = true;
                mFavCount ++;
                mFavourites.append((int)mCurrentVideo.uiVidId);
                emit sigVideoInfo(mCurrentVideo);
            }
            break;
        case ivi::IVI_DEL_FAV:
            if (!resp.contains("error"))
            {
                mFavCount --;
                int idx = mFavourites.indexOf((int)mCurrentVideo.uiVidId);

                if (idx > -1)
                {
                    mFavourites.remove(idx);
                }

                mCurrentVideo.bFavourit = false;
                emit sigVideoInfo(mCurrentVideo);
            }
            break;

        case ivi::IVI_FAV_COUNT:
            parseFavCount(resp);
            break;

        case ivi::IVI_ALL_FAV:
            parseAllFavs(resp);
            break;
        case ivi::IVI_FAVOURITES:
            parseVideos(resp);
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
