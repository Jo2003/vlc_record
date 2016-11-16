#ifndef CIVIAPI_H
#define CIVIAPI_H

#include <QObject>
#include <QNetworkAccessManager>

#define IVI_REQ_ID "reqid"

namespace ivi {

    enum eIviReq {
        IVI_GENRES,
        IVI_VIDEOS,
        IVI_VIDEOINFO,
        IVI_ADD_FAV,
        IVI_DEL_FAV,
        IVI_UNKNOWN=256
    };
}

class CIviApi : public QNetworkAccessManager
{
    Q_OBJECT

public:
    CIviApi(QObject *parent = 0);

    void setSessionKey(const QString& key);

    // get / set
    int getGenres();

    // parse
    int parseGenres(const QString& resp);

signals:

private slots:
    void getReply(QNetworkReply* reply);

public slots:

private:
    QString mProtocol;
    QString mHost;
    QString mQueryPrefix;
    QString mSessionKey;
};

#endif // CIVIAPI_H
