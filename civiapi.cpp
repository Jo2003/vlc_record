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

#ifdef __TRACE
    #define __TRACE_IVI
#endif // __TRACE

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
    mRealAppVer  = -1;
    pHash        = new CCMAC_Bf(IVI_KEY, IVI_K1, IVI_K2);

    mContData.mDevice = QString("%1%2-%3")
            .arg(pCustomization->strVal("APPLICATION_SHORTCUT"))
            .arg(OP_SYS).arg(SOFTID_DEVELOPER);

    connect(this, SIGNAL(finished(QNetworkReply*)), this, SLOT(getReply(QNetworkReply*)));
    connect(this, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(slotSslError(QNetworkReply*,QList<QSslError>)));
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
    getRealAppVersion();
}

//------------------------------------------------------------------------------
//! @brief      Sets the session key.
//!
//! @param[in]  key   The key
//------------------------------------------------------------------------------
int CIviApi::login()
{
    // "https://api.ivi.ru/mobileapi/user/login/verimatrix/v2?verimatrix=abc&app_version=1234"
    QString req = QString("%1://%2/%3/user/login/verimatrix/v5?verimatrix=%4")
            .arg(mProtocol)
            .arg(mHost)
            .arg(mQueryPrefix)
            .arg(mVerimatrix);

    // add app_version ...
    req += QString("&app_version=%1").arg(mRealAppVer);

#ifdef __TRACE_IVI
    mInfo(req);
#endif

    QNetworkRequest request;
    request.setUrl(QUrl(req));

    QNetworkReply* pReply = QNetworkAccessManager::get(request);

    if (pReply)
    {
        pReply->setProperty(IVI_REQ_ID, (int)ivi::IVI_SESSION);
    }

    return pReply ? 0 : -1;
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
    req += QString("&app_version=%1").arg(mRealAppVer);

#ifdef __TRACE_IVI
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
    req += QString("&app_version=%1").arg(mRealAppVer);

#ifdef __TRACE_IVI
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
    if (filter.mCompId != -1)
    {
        // check for compilation info ...
        if (filter.mCompId != (int)mCompilationInfo.uiVidId)
        {
            mCompFilter = filter;
            return getVideoInfo(filter.mCompId, ivi::KIND_COMPILATION);
        }
        else
        {
            return getVideoFromCompilation(filter);
        }
    }
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
    req += QString("&app_version=%1").arg(mRealAppVer);

    // add filter stuff ...
    req += QString("&sort=%1").arg(filter.mSort);
    req += QString("&from=%1").arg(filter.mFrom);
    req += QString("&to=%1").arg(filter.mTo);

    if (filter.mGenId != -1)
    {
        req += QString("&genre=%1").arg(filter.mGenId);
    }
    else if (filter.mCatId != -1)
    {
        req += QString("&category=%1").arg(filter.mCatId);
    }

#ifdef __TRACE_IVI
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
    req += QString("&app_version=%1").arg(mRealAppVer);

    // add filter stuff ...
    req += QString("&from=%1").arg(filter.mFrom);
    req += QString("&to=%1").arg(filter.mTo);

    if (filter.mGenId != -1)
    {
        req += QString("&genre=%1").arg(filter.mGenId);
    }
    else if (filter.mCatId != -1)
    {
        req += QString("&category=%1").arg(filter.mCatId);
    }

    req += QString("&query=%1").arg(filter.mSearch);

#ifdef __TRACE_IVI
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
//! @param[in]  id      The identifier
//! @param[in]  kind    video or compilation
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CIviApi::getVideoInfo(int id, ivi::eKind kind)
{
    // "https://api.ivi.ru/mobileapi/videoinfo/v6/?session=sesstoken&id=1"
    QString req = QString((kind == ivi::KIND_VIDEO)
                          ? "%1://%2/%3/videoinfo/v6/?session=%4"
                          : "%1://%2/%3/compilationinfo/v5/?session=%4")
            .arg(mProtocol)
            .arg(mHost)
            .arg(mQueryPrefix)
            .arg(mSessionKey);

    // add app_version ...
    req += QString("&app_version=%1").arg(mRealAppVer);

    // add id ...
    req += QString("&id=%1").arg(id);

#ifdef __TRACE_IVI
    mInfo(req);
#endif

    QNetworkRequest request;
    request.setUrl(QUrl(req));

    QNetworkReply* pReply = QNetworkAccessManager::get(request);

    if (pReply)
    {
        pReply->setProperty(IVI_REQ_ID, ((ivi::eKind)kind == ivi::KIND_VIDEO)
                                         ? (int)ivi::IVI_VIDEOINFO
                                         : (int)ivi::IVI_COMPINFO);
    }

    return pReply ? 0 : -1;
}

//------------------------------------------------------------------------------
//! @brief      get videos belonging to one compilation
//!
//! @param[in]  id    The identifier
//! @param[in]  count number of contens
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CIviApi::getVideoFromCompilation(const ivi::SVideoFilter &filter)
{
    // "https://api.ivi.ru/mobileapi/videofromcompilation/v5/?session=sesstoken&id=1"
    QString req = QString("%1://%2/%3/videofromcompilation/v5/?session=%4")
            .arg(mProtocol)
            .arg(mHost)
            .arg(mQueryPrefix)
            .arg(mSessionKey);

    // add app_version ...
    req += QString("&app_version=%1").arg(mRealAppVer);

    // add id ...
    req += QString("&id=%1").arg(filter.mCompId);
    req += QString("&from=%1").arg(filter.mFrom);
    req += QString("&to=%1").arg(filter.mTo);

#ifdef __TRACE_IVI
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
            .arg(mRealAppVer);

    url += QString("?app_version=%1").arg(mRealAppVer);
    url += QString("&ts=%1").arg(mTs);
    url += QString("&sign=%1").arg(pHash->sign(mTs + postData));

#ifdef __TRACE_IVI
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
//! @param      kind compilation or video
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CIviApi::getVideoPersons(int id, ivi::eKind kind)
{
    // "https://api.ivi.ru/mobileapi/video/persons/v5/?session=sesstoken&id=7029"
    QString req = QString((kind == ivi::KIND_VIDEO)
                          ? "%1://%2/%3/video/persons/v5/?session=%4"
                          : "%1://%2/%3/compilation/persons/v5/?session=%4")
            .arg(mProtocol)
            .arg(mHost)
            .arg(mQueryPrefix)
            .arg(mSessionKey);

    // add app_version ...
    req += QString("&app_version=%1").arg(mRealAppVer);

    // add id ...
    req += QString("&id=%1").arg(id);

#ifdef __TRACE_IVI
    mInfo(req);
#endif

    QNetworkRequest request;
    request.setUrl(QUrl(req));

    QNetworkReply* pReply = QNetworkAccessManager::get(request);

    if (pReply)
    {
        int req = ((kind == ivi::KIND_COMPILATION) && (id == (int)mCompilationInfo.uiVidId))
                ? (int)ivi::IVI_COMP_PERSONS
                : (int)ivi::IVI_VIDEO_PERSONS;

        pReply->setProperty(IVI_REQ_ID, req);
    }

    return pReply ? 0 : -1;
}

//------------------------------------------------------------------------------
//! @brief      add video to favourites
//!
//! @param      id video id
//! @param      kind compilation or video
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CIviApi::addFav(int id, ivi::eKind kind)
{
    // /video/favourite/v5/add?(int: id)&(str: session)&(int: subsite)&(int: app_version)
    QString req = QString((kind == ivi::KIND_VIDEO)
                          ? "%1://%2/%3/video/favourite/v5/add"
                          : "%1://%2/%3/compilation/favourite/v5/add")
            .arg(mProtocol)
            .arg(mHost)
            .arg(mQueryPrefix);

    // content  ...
    QString cont;
    cont += QString("app_version=%1").arg(mRealAppVer);
    cont += QString("&session=%1").arg(mSessionKey);
    cont += QString("&id=%1").arg(id);

#ifdef __TRACE_IVI
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
//! @param      kind compilation or video
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CIviApi::delFav(int id, ivi::eKind kind)
{
    QString req = QString((kind == ivi::KIND_VIDEO)
                          ? "%1://%2/%3/video/favourite/v5/delete"
                          : "%1://%2/%3/compilation/favourite/v5/delete")
            .arg(mProtocol)
            .arg(mHost)
            .arg(mQueryPrefix);

    // content  ...
    QString cont;
    cont += QString("app_version=%1").arg(mRealAppVer);
    cont += QString("&session=%1").arg(mSessionKey);
    cont += QString("&id=%1").arg(id);

#ifdef __TRACE_IVI
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
    req += QString("&app_version=%1").arg(mRealAppVer);
    req += QString("&withunavailable=");

#ifdef __TRACE_IVI
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
    req += QString("&app_version=%1").arg(mRealAppVer);
    req += QString("&showunavailable=");
    req += QString("&from=%1").arg(from);
    req += QString("&to=%1").arg(to);

#ifdef __TRACE_IVI
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
//! @brief      Gets real app version for my location
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CIviApi::getRealAppVersion()
{
    // /geocheck/whoami/v6/?(int: app_version)
    QString req = QString("%1://%2/%3/geocheck/whoami/v6/?app_version=%4")
            .arg(mProtocol)
            .arg(mHost)
            .arg(mQueryPrefix)
            .arg(IVI_APP_VERSION);

#ifdef __TRACE_IVI
    mInfo(req);
#endif

    QNetworkRequest request;
    request.setUrl(QUrl(req));

    QNetworkReply* pReply = QNetworkAccessManager::get(request);

    if (pReply)
    {
        pReply->setProperty(IVI_REQ_ID, (int)ivi::IVI_APP_VER);
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

#ifdef __TRACE_IVI
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
//! @brief      send ivi statistics
//!
//! @param[in]  stats ivi statistiks structure
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CIviApi::sendIviStats(ivistats::SIviStats stats)
{
#ifdef __TRACE_IVI
    if (stats.mPost)
    {
        mInfo(tr("Post statistics '%1' to url '%2'").arg(stats.mContent).arg(stats.mUrl));
    }
    else
    {
        mInfo(tr("Statistics request: '%1'").arg(stats.mUrl));
    }
#endif

    QNetworkRequest request;
    request.setUrl(QUrl(stats.mUrl));

    QNetworkReply* pReply = NULL;

    if (stats.mPost)
    {
        pReply = QNetworkAccessManager::post(request, stats.mContent.toUtf8());
    }
    else
    {
        pReply = QNetworkAccessManager::get(request);
    }

    if (pReply)
    {
        pReply->setProperty(IVI_REQ_ID, (int)ivi::IVI_STATS);
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
        contentMap        = contentMap.value("result").toMap();
        mSessionKey       = contentMap.value("session").toString();
        mContData.mIviUid = contentMap.value("id").toInt();

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
            video.sImg          = "";
            video.sYear         = "";
            video.sCompName     = "";
            video.iContentCount = -1;
            video.iCompId       = -1;
            video.uiVidId  = mVideo.value("id").toUInt();
            video.sName    = mVideo.value("title").toString();

            if (mVideo.contains("total_contents"))
            {
                video.iContentCount = mVideo.value("total_contents").toInt();
            }

            if (mVideo.contains("compilation"))
            {
                video.iCompId = mVideo.value("compilation").toInt();
            }

            if (mVideo.contains("compilation_title"))
            {
                video.sCompName = mVideo.value("compilation_title").toString();
            }

            // compilation and video handle year in a different way ...
            if (mVideo.contains("year"))
            {
                video.sYear    = QString::number(mVideo.value("year").toInt());
            }
            else if (mVideo.contains("years"))
            {
                foreach(const QVariant& rawYear, mVideo.value("years").toList())
                {
                    if(!video.sYear.isEmpty())
                    {
                        video.sYear += ", ";
                    }
                    video.sYear += QString::number(rawYear.toInt());
                }
            }

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
//! @param[in]  req   ivi request type
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CIviApi::parseVideoInfo(const QString &resp, ivi::eIviReq req)
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

        // no record in IVI VOD!
        video.bAllowRecord = false;

        if (mVideo.contains("total_contents"))
        {
            video.iContentCount = mVideo.value("total_contents").toInt();
        }

        if (mVideo.contains("compilation"))
        {
            video.iCompId = mVideo.value("compilation").toInt();
        }

        if (!mVideo.value("orig_title").toString().isEmpty())
        {
            video.sName += " (" + mVideo.value("orig_title").toString() + ")";
        }

        // compilation and video handle year in a different way ...
        if (mVideo.contains("year"))
        {
            video.sYear    = QString::number(mVideo.value("year").toInt());
        }
        else if (mVideo.contains("years"))
        {
            foreach(const QVariant& rawYear, mVideo.value("years").toList())
            {
                if(!video.sYear.isEmpty())
                {
                    video.sYear += ", ";
                }
                video.sYear += QString::number(rawYear.toInt());
            }
        }

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

        if (req == ivi::IVI_VIDEOINFO)
        {
            mCurrentVideo    = video;
        }
        else
        {
            mCompilationInfo = video;
        }

        // fill content data ...
        mContData.mEndCredits = mVideo.value("credits_begin_time").toInt();
        if (mContData.mEndCredits == 0)
        {
            mContData.mEndCredits = video.uiLength * 60;
        }
        mContData.mContentId = (int)video.uiVidId;

        // request links ...
        getVideoPersons(video.uiVidId, (ivi::eKind)video.iKind);
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

        // fill content data ...

        // pixel audit ...
        mContData.mPixAudit.clear();
        foreach(QVariant pixelRaw, mVideo.value("pixelaudit").toList())
        {
            QVariantMap pixel = pixelRaw.toMap();
            mContData.mPixAudit.insert(pixel.value("title").toString(), pixel.value("link").toString());
        }

        // watch id ...
        mContData.mWatchId = mVideo.value("watchid").toString();

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

        if ((mCurrentVideo.iCompId != -1) && (mCurrentVideo.iCompId == (int)mCompilationInfo.uiVidId))
        {
            combineInfo();
        }
        else
        {
            emit sigVideoInfo(mCurrentVideo);
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
int CIviApi::parseVideoPersons(const QString &resp, ivi::eIviReq req)
{
    mInfo(tr("We've got persons response ..."));
    int              iRV = 0;
    bool             bOk;
    QVariantMap      contentMap;

    contentMap = QtJson::parse(resp, bOk).toMap();

    if (bOk)
    {
        QString& director = (req == ivi::IVI_VIDEO_PERSONS)
                            ? mCurrentVideo.sDirector
                            : mCompilationInfo.sDirector;

        // ivi categories ...
        foreach (const QVariant& varPerson, contentMap.value("result").toList())
        {
            QVariantMap mPerson = varPerson.toMap();

            if (mPerson.value("id").toInt() == 3) // director
            {
                foreach (const QVariant& varDir, mPerson.value("persons").toList())
                {
                    QVariantMap mDirector = varDir.toMap();
                    if (!director.isEmpty())
                    {
                        director += ", ";
                    }

                    director += mDirector.value("name").toString();
                }

                break;
            }
        }

        if (req == ivi::IVI_VIDEO_PERSONS)
        {
            getFiles(mCurrentVideo.uiVidId);
        }
        else
        {
            // compilation info filled -> request compilation videos ...
            iRV = getVideoFromCompilation(mCompFilter);
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
//! @brief      parse app version response
//!
//! @param[in]  resp  ivi response to parse
//!
//! @return     0 -> ok; -1 -> error
//------------------------------------------------------------------------------
int CIviApi::parseRealAppVer(const QString &resp)
{
    mInfo(tr("parse real app version ... "));

    int              iRV = 0;
    bool             bOk;
    QVariantMap      contentMap;

    contentMap = QtJson::parse(resp, bOk).toMap();

    if (bOk)
    {
        contentMap  = contentMap.value("result").toMap();

        if (contentMap.contains("actual_app_version"))
        {
            mRealAppVer = contentMap.value("actual_app_version").toInt();
        }
        else
        {
            mRealAppVer = IVI_APP_VERSION;
            mWarn(tr("Can't get real app version, using default: %1").arg(IVI_APP_VERSION));
        }

        // for content data ...
        mContData.mAppVersion = mRealAppVer;

        login();
    }
    else
    {
        emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
            tr("QtJson parser error in %1 %2():%3")
                .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__));

        iRV = -1;
    }

    return iRV;



    // login() ...
    return 0;
}

//------------------------------------------------------------------------------
//! @brief      combine video info with compilation info
//------------------------------------------------------------------------------
void CIviApi::combineInfo()
{
    // prepare strings ...
    mCurrentVideo.sDescr           = mCurrentVideo.sDescr.simplified();
    mCurrentVideo.sActors          = mCurrentVideo.sActors.simplified();
    mCurrentVideo.sDirector        = mCurrentVideo.sDirector.simplified();
    mCurrentVideo.sCountry         = mCurrentVideo.sCountry.simplified();
    mCurrentVideo.sYear            = mCurrentVideo.sYear.simplified();
    mCurrentVideo.sGenres          = mCurrentVideo.sGenres.simplified();
    mCurrentVideo.sImdbRating      = mCurrentVideo.sImdbRating.simplified();
    mCurrentVideo.sKinopoiskRating = mCurrentVideo.sKinopoiskRating.simplified();
    mCurrentVideo.sName            = mCurrentVideo.sName.simplified();

    // check for more detailed info ...
    if (mCurrentVideo.sDescr.isEmpty())
    {
        mCurrentVideo.sDescr = mCompilationInfo.sDescr;
    }

    if (mCurrentVideo.sActors.isEmpty())
    {
        mCurrentVideo.sActors = mCompilationInfo.sActors;
    }

    if (mCurrentVideo.sDirector.isEmpty())
    {
        mCurrentVideo.sDirector = mCompilationInfo.sDirector;
    }

    if (mCurrentVideo.sCountry.isEmpty())
    {
        mCurrentVideo.sCountry = mCompilationInfo.sCountry;
    }

    if (mCurrentVideo.sYear.isEmpty() || (mCurrentVideo.sYear == "0"))
    {
        mCurrentVideo.sYear = mCompilationInfo.sYear;
    }

    if (mCurrentVideo.sGenres.isEmpty())
    {
        mCurrentVideo.sGenres = mCompilationInfo.sGenres;
    }

    if (mCurrentVideo.sImdbRating.isEmpty() || (mCurrentVideo.sImdbRating == "0"))
    {
        mCurrentVideo.sImdbRating = mCompilationInfo.sImdbRating;
    }

    if (mCurrentVideo.sKinopoiskRating.isEmpty() || (mCurrentVideo.sKinopoiskRating == "0"))
    {
        mCurrentVideo.sKinopoiskRating = mCompilationInfo.sKinopoiskRating;
    }

    // override favourites stuff ...
    mCurrentVideo.bFavourit = mCompilationInfo.bFavourit;

    // title ...
    mCurrentVideo.sName = mCompilationInfo.sName + " - " + mCurrentVideo.sName;

    emit sigVideoInfo(mCurrentVideo);
}

//------------------------------------------------------------------------------
//! @brief      get content data
//!
//! @returns    ivistats::SContentData
//! @sa         ivistats::SContentData
//------------------------------------------------------------------------------
ivistats::SContentData CIviApi::getContentData()
{
    return mContData;
}

//------------------------------------------------------------------------------
//! @brief      check response for errors
//!
//! @param      resp string response
//! @param      errText string reference for error text
//!
//! @returns    error number (o if all is well)
//------------------------------------------------------------------------------
int CIviApi::hasError(const QString &resp, QString &errText)
{
    int iRV = 0;

    // simple pre check ...
    if (resp.contains("error"))
    {
        bool             bOk;
        QVariantMap      contentMap;

        contentMap = QtJson::parse(resp, bOk).toMap();

        if (bOk)
        {
            if (contentMap.contains("error"))
            {
                contentMap  = contentMap.value("error").toMap();

                if (contentMap.contains("message"))
                {
                    errText = contentMap.value("message").toString();
                }

                if (contentMap.contains("type"))
                {
                    iRV = contentMap.value("type").toInt();
                }

                if (contentMap.contains("code"))
                {
                    iRV = contentMap.value("code").toInt();
                }

                if (contentMap.contains("error_code"))
                {
                    iRV = contentMap.value("error_code").toInt();
                }
            }
        }
        else
        {
            emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                          tr("QtJson parser error in %1 %2():%3")
                              .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__));
        }

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
    QString errText;
    int     errCode;

    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray ba   = reply->readAll();
        QString    resp = QString::fromUtf8(ba.constData());

#ifdef __TRACE_IVI
        if (!resp.isEmpty())
        {
            mInfo(tr("IVI Response:\n ==8<==8<==8<==\n%1\n ==>8==>8==>8==")
                     .arg(resp));
        }
        else
        {
            mInfo(tr("Empty IVI response ..."));
        }
#endif // __TRACE

        errCode = hasError(resp, errText);

        // where are errors allowed ... ?
        if (errCode && ((req == ivi::IVI_APP_VER) || (req == ivi::IVI_FILES) || (req == ivi::IVI_STATS)))
        {
            errCode = 0;
        }

        if (!errCode)
        {
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
            case ivi::IVI_COMPINFO:
                parseVideoInfo(resp, req);
                break;
            case ivi::IVI_FILES:
                parseFiles(resp);
                break;
            case ivi::IVI_TIMESTAMP:
                parseTimeStamp(resp);
                break;
            case ivi::IVI_VIDEO_PERSONS:
            case ivi::IVI_COMP_PERSONS:
                parseVideoPersons(resp, req);
                break;
            case ivi::IVI_ADD_FAV:
                {
                    if ((mCurrentVideo.iCompId != -1) && (mCurrentVideo.iCompId == (int)mCompilationInfo.uiVidId))
                    {
                        // compilation added to favourites ...
                        mCompilationInfo.bFavourit = true;
                        mFavourites.append((int)mCurrentVideo.iCompId);
                    }
                    else
                    {
                        mFavourites.append((int)mCurrentVideo.uiVidId);
                    }

                    mCurrentVideo.bFavourit = true;
                    mFavCount ++;
                    emit sigVideoInfo(mCurrentVideo);
                }
                break;
            case ivi::IVI_DEL_FAV:
                {
                    int idx;
                    if ((mCurrentVideo.iCompId != -1) && (mCurrentVideo.iCompId == (int)mCompilationInfo.uiVidId))
                    {
                        // compilation removed from favourites ...
                        mCompilationInfo.bFavourit = false;
                        idx = mFavourites.indexOf((int)mCurrentVideo.iCompId);
                    }
                    else
                    {
                        idx = mFavourites.indexOf((int)mCurrentVideo.uiVidId);
                    }

                    mFavCount --;
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
            case ivi::IVI_APP_VER:
                parseRealAppVer(resp);
                break;
            case ivi::IVI_STATS:
                // nothing to note here ...
                break;
            default:
                break;
            }
        }
        else
        {
            emit sigError((int)Msg::Error, tr("Error %1").arg(errCode), errText);
        }
    }
    else
    {
        mInfo(tr("Network error: #%1: %2").arg((int)reply->error()).arg(reply->errorString()));
    }

    // mark for deletion ...
    reply->deleteLater();
}

//------------------------------------------------------------------------------
//! @brief      ssl error occured
//!
//! @param      pReply  pointer to network reply
//! @param      errors  list of network errors
//------------------------------------------------------------------------------
void CIviApi::slotSslError(QNetworkReply *pReply, QList<QSslError> errors)
{
    Q_UNUSED(pReply)

    foreach (const QSslError &error, errors)
    {
        mInfo(tr("SSL Error #%1: '%2'").arg((int)error.error()).arg(error.errorString()));
    }
}
