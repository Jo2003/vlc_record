#include "cvodivi.h"
#include "ui_cvodivi.h"

CVodIvi::CVodIvi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CVodIvi)
{
    ui->setupUi(this);
}

CVodIvi::~CVodIvi()
{
    delete ui;
}

void CVodIvi::setIviSession(const QString &str)
{
    iviApi.setSessionKey(str);
    iviApi.getGenres();
}

void CVodIvi::on_cbxVodLang_activated(int index)
{
    /*
    int  iGid  = ui->cbxGenre->itemData(ui->cbxGenre->currentIndex()).toInt();

    QUrlEx url;
    url.addQueryItem("type", ui->cbxLastOrBest->itemData(ui->cbxLastOrBest->currentIndex()).toString());
    url.addQueryItem("lang", ui->cbxVodLang->itemData(index).toString());
    url.addQueryItem("nums", "20");
    if (iGid != -1)
    {
       url.addQueryItem("genre", QString::number(iGid));
    }
    pApiClient->queueRequest(CIptvDefs::REQ_GETVIDEOS, url.query());
    */
}

void CVodIvi::on_cbxGenre_activated(int index)
{
    /*
    // check for vod favourites ...
    QString   sType = ui->cbxLastOrBest->itemData(ui->cbxLastOrBest->currentIndex()).toString();
    int       iGid  = ui->cbxGenre->itemData(index).toInt();
    QUrlEx    url;

    if (sType == "vodfav")
    {
       // set filter cbx to "last"  ...
       ui->cbxLastOrBest->setCurrentIndex(0);
       sType = "last";
    }

    url.addQueryItem("type", sType);

 #ifdef _HAS_VOD_LANG
    url.addQueryItem("lang", ui->cbxVodLang->itemData(ui->cbxVodLang->currentIndex()).toString());
 #endif // _HAS_VOD_LANG

    if (iGid != -1)
    {
       url.addQueryItem("genre", QString::number(iGid));
    }

    pApiClient->queueRequest(CIptvDefs::REQ_GETVIDEOS, url.query());
    */
}

void CVodIvi::on_cbxLastOrBest_activated(int index)
{
    /*
    QString sType = ui->cbxLastOrBest->itemData(index).toString();

    if (sType == "vodfav")
    {
       pApiClient->queueRequest(CIptvDefs::REQ_GET_VOD_FAV);
    }
    else
    {
       int    iGid  = ui->cbxGenre->itemData(ui->cbxGenre->currentIndex()).toInt();
       QUrlEx url;

       url.addQueryItem("type", sType);

       if (iGid != -1)
       {
          url.addQueryItem("genre", QString::number(iGid));
       }

 #ifdef _HAS_VOD_LANG
       url.addQueryItem("lang", ui->cbxVodLang->itemData(ui->cbxVodLang->currentIndex()).toString());
 #endif // _HAS_VOD_LANG

       pApiClient->queueRequest(CIptvDefs::REQ_GETVIDEOS, url.query());
    }
    */
}

void CVodIvi::on_btnPrevSite_clicked()
{
    /*
    QUrlEx  url;
    QString sType  = ui->cbxLastOrBest->itemData(ui->cbxLastOrBest->currentIndex()).toString();
    int     iGenre = ui->cbxGenre->itemData(ui->cbxGenre->currentIndex()).toInt();
    int     iPage  = ui->cbxSites->currentIndex() + 1;

    url.addQueryItem("page", QString::number(iPage - 1));

    if (sType == "vodfav")
    {
        pApiClient->queueRequest(CIptvDefs::REQ_GET_VOD_FAV, url.query());
    }
    else
    {
        url.addQueryItem("type", sType);

#ifdef _HAS_VOD_LANG
        url.addQueryItem("lang", ui->cbxVodLang->itemData(ui->cbxVodLang->currentIndex()).toString());
#endif // _HAS_VOD_LANG

        if (iGenre != -1)
        {
            url.addQueryItem("genre", QString::number(iGenre));
        }

        pApiClient->queueRequest(CIptvDefs::REQ_GETVIDEOS, url.query());
    }
    */
}

void CVodIvi::on_btnNextSite_clicked()
{
    /*
    QUrlEx  url;
    QString sType  = ui->cbxLastOrBest->itemData(ui->cbxLastOrBest->currentIndex()).toString();
    int     iGenre = ui->cbxGenre->itemData(ui->cbxGenre->currentIndex()).toInt();
    int     iPage  = ui->cbxSites->currentIndex() + 1;

    url.addQueryItem("page", QString::number(iPage + 1));

    if (sType == "vodfav")
    {
        pApiClient->queueRequest(CIptvDefs::REQ_GET_VOD_FAV, url.query());
    }
    else
    {
        url.addQueryItem("type", sType);

#ifdef _HAS_VOD_LANG
        url.addQueryItem("lang", ui->cbxVodLang->itemData(ui->cbxVodLang->currentIndex()).toString());
#endif // _HAS_VOD_LANG

        if (iGenre != -1)
        {
            url.addQueryItem("genre", QString::number(iGenre));
        }

        pApiClient->queueRequest(CIptvDefs::REQ_GETVIDEOS, url.query());
    }
    */
}

void CVodIvi::on_cbxSites_activated(int index)
{
    /*
    // something changed ... ?
    if ((index + 1) != genreInfo.iPage)
    {
        QUrlEx  url;
        url.addQueryItem("page", QString::number(index + 1));

        QString sType  = ui->cbxLastOrBest->itemData(ui->cbxLastOrBest->currentIndex()).toString();

        if (sType == "vodfav")
        {
            pApiClient->queueRequest(CIptvDefs::REQ_GET_VOD_FAV, url.query());
        }
        else
        {
            int iGenre = ui->cbxGenre->itemData(ui->cbxGenre->currentIndex()).toInt();

            url.addQueryItem("type", sType);

#ifdef _HAS_VOD_LANG
            url.addQueryItem("lang", ui->cbxVodLang->itemData(ui->cbxVodLang->currentIndex()).toString());
#endif // _HAS_VOD_LANG

            if (iGenre != -1)
            {
                url.addQueryItem("genre", QString::number(iGenre));
            }

            pApiClient->queueRequest(CIptvDefs::REQ_GETVIDEOS, url.query());
        }
    }
    */
}
