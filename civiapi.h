#ifndef CIVIAPI_H
#define CIVIAPI_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QMap>
#define IVI_REQ_ID "reqid"

namespace ivi {

    // looking forward ...
    struct SGenre;
    struct SCat;
    struct SCountry;

    typedef QMap<int, SCat>     CategoryMap;
    typedef QMap<int, SGenre>   GenreMap;
    typedef QMap<int, SCountry> CountryMap;

    enum eIviReq {
        IVI_GENRES,
        IVI_VIDEOS,
        IVI_VIDEOINFO,
        IVI_COUNTRIES,
        IVI_ADD_FAV,
        IVI_DEL_FAV,
        IVI_UNKNOWN=256
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
        int     mGenId;
        int     mFrom;
        int     mTo;
        QString mSort;
    };

    struct SCountry {
        int     mId;
        QString mName;
        QString mShort;
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
    int getCountries();
    int getVideos(const ivi::SVideoFilter& filter);

    // parse
    int parseGenres(const QString& resp);
    int parseCountries(const QString& resp);
    int parseVideos(const QString& resp);

signals:
    void sigCategories(ivi::CategoryMap cats);
    void sigCountries(ivi::CountryMap countr);
    void sigError(int iType, const QString& cap, const QString& descr);

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
