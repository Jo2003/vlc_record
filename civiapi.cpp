#include "civiapi.h"
#include <QtJson>
#include "externals_inc.h"

CIviApi::CIviApi(QObject *parent) :
    QNetworkAccessManager(parent)
{
    mProtocol    = "https";
    mHost        = "api.ivi.ru";
    mQueryPrefix = "mobileapi";
    connect(this, SIGNAL(finished(QNetworkReply*)), this, SLOT(getReply(QNetworkReply*)));
}

void CIviApi::setSessionKey(const QString &key)
{
    mSessionKey = key;
}

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
}

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
}

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
}

int CIviApi::parseGenres(const QString &resp)
{
    mInfo(tr("We've got genres response ..."));

    int              iRV = 0;
    bool             bOk;
    QVariantMap      contentMap;
    ivi::CategoryMap cats;
    ivi::SCat        cat;
    ivi::SGenre      genre;

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

int CIviApi::parseCountries(const QString &resp)
{
    mInfo(tr("Parse IVI countries ..."));
    int              iRV = 0;
    bool             bOk;
    QVariantMap      contentMap;
    ivi::CountryMap  countries;
    ivi::SCountry    country;

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

            countries.insert(country.mId, country);
        }

        emit sigCountries(countries);
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

int CIviApi::parseVideos(const QString &resp)
{
    mInfo(tr("Parse IVI videos ..."));
    return 0;
}

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
        default:
            break;
        }
    }
    else
    {
        mInfo(tr("Network error: %1").arg(reply->errorString()));
    }

    // mark for deletion ...
    reply->deleteLater();
}
