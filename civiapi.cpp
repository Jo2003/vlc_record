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

int CIviApi::parseGenres(const QString &resp)
{
    mInfo(resp);
}

void CIviApi::getReply(QNetworkReply *reply)
{
    ivi::eIviReq req = (ivi::eIviReq)reply->property(IVI_REQ_ID).toInt();

    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray ba = reply->readAll();

        switch(req)
        {
        case ivi::IVI_GENRES:
            parseGenres(QString::fromUtf8(ba.constData()));
            break;
        default:
            break;
        }
    }
    else
    {
        mInfo(tr("Network error: %1").arg(reply->errorString()));
    }
}


