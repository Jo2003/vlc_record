/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     cvodivi.cpp
 *
 *  @author   Jo2003
 *
 *  @date     18.11.2016
 *
 *  $Id$
 *
 *///------------------------- (c) 2016 by Jo2003  --------------------------
#include "cvodivi.h"
#include "ui_cvodivi.h"
#include "externals_inc.h"

#ifdef __TRACE
    #define __TRACE_IVI
#endif // __TRACE

//------------------------------------------------------------------------------
//! @brief      Constructs the object.
//!
//! @param      parent  The parent
//------------------------------------------------------------------------------
CVodIvi::CVodIvi(QWidget *parent) :
    QWidget(parent), mCompId(-1),
    ui(new Ui::CVodIvi)
{
    ui->setupUi(this);

    mpPlayer = NULL;

    tIviSearch.setSingleShot(true);
    tIviSearch.setInterval(500);

    ui->iviBrowser->setBranding(":/vod/ivi");

    connect (&iviApi, SIGNAL(sigCategories(ivi::CategoryMap)), this, SLOT(slotCatchCategories(ivi::CategoryMap)));
    connect (&iviApi, SIGNAL(sigCountries(ivi::CountryMap)), this, SLOT(slotCatchCountries(ivi::CountryMap)));
    connect (&iviApi, SIGNAL(sigVideoList(cparser::VideoList)), this, SLOT(slotCatchVideos(cparser::VideoList)));
    connect (&iviApi, SIGNAL(sigVideoInfo(cparser::SVodVideo)), this, SLOT(slotCatchVideoInfo(cparser::SVodVideo)));
    connect (&tIviSearch, SIGNAL(timeout()), this, SLOT(slotTimerIviSearch()));

    connect (&iviStats,   SIGNAL(sigStats(ivistats::SIviStats)), &iviApi, SLOT(sendIviStats(ivistats::SIviStats)));
    connect (&iviStats,   SIGNAL(sigWatched(int,QString))      , &iviApi, SLOT(sendWatched(int,QString)));
}

//------------------------------------------------------------------------------
//! @brief      Destroys the object.
//------------------------------------------------------------------------------
CVodIvi::~CVodIvi()
{
    delete ui;
}

//------------------------------------------------------------------------------
//! @brief      re-translate ui
//!
//! @param      e event pointer
//------------------------------------------------------------------------------
void CVodIvi::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    if (e->type() == QEvent::LanguageChange)
    {
        int idx = ui->cbxIviLastOrBest->currentIndex();
        ui->retranslateUi(this);
        fillSortCbx(idx);
    }
}

//------------------------------------------------------------------------------
//! @brief      Sets the pix cache.
//!
//! @param      cache  The cache
//------------------------------------------------------------------------------
void CVodIvi::setPixCache(CPixLoader *cache)
{
    ui->iviBrowser->setPixCache(cache);
}

//------------------------------------------------------------------------------
//! @brief      Sets the ivi session.
//!
//! @param[in]  str   The session string
//------------------------------------------------------------------------------
void CVodIvi::setIviSession(const QString &str)
{
    iviApi.setVerimatrixKey(str);
    fillSortCbx();
}

//------------------------------------------------------------------------------
//! @brief      store player instance
//!
//! @param[in]  pPlayer pointer to player instance
//------------------------------------------------------------------------------
void CVodIvi::setPlayer(CPlayer *pPlayer)
{
    mpPlayer = pPlayer;
    iviStats.setPlayer(pPlayer);

    // catch pseudo start ...
    connect (mpPlayer, SIGNAL(sigIviMainFeature()), this, SLOT(slotIviMainFeature()));
}

//------------------------------------------------------------------------------
//! @brief      get pointer to vod browser
//!
//! @return     pointer to vod browser
//------------------------------------------------------------------------------
CVodBrowser *CVodIvi::iviBrowser()
{
    return ui->iviBrowser;
}

//------------------------------------------------------------------------------
//! @brief      set proxy server
//
//! @param[in]  proxy ref. to proxy confguration
//------------------------------------------------------------------------------
void CVodIvi::setProxy(const QNetworkProxy &proxy)
{
    iviApi.setProxy(proxy);
}

//------------------------------------------------------------------------------
//! @brief      genre box was activated
//!
//! @param[in]  index  The index
//------------------------------------------------------------------------------
void CVodIvi::on_cbxIviGenre_activated(int index)
{
    mCompId   = -1;
    int catId = ui->cbxIviCategory->itemData(ui->cbxIviCategory->currentIndex()).toInt();
    int genId = ui->cbxIviGenre->itemData(index).toInt();
    int count = 0;

    if ((catId == -1) && (genId == -1))
    {
        foreach (const ivi::SCat& cat, mIviCats)
        {
            foreach (const ivi::SGenre genre, cat.mGenres)
            {
                count += genre.mCount;
            }
        }
    }
    else if (genId == -1)
    {
        foreach (const ivi::SGenre genre, mIviCats.value(catId).mGenres)
        {
            count += genre.mCount;
        }
    }
    else
    {
        count = mIviCats.value(catId).mGenres.value(genId).mCount;
    }

    fillSitesCbx(count);
}

//------------------------------------------------------------------------------
//! @brief      sort box was activated
//!
//! @param[in]  index  The index
//------------------------------------------------------------------------------
void CVodIvi::on_cbxIviLastOrBest_activated(int index)
{
    mCompId = -1;
    if (ui->cbxIviLastOrBest->itemData(index).toString() == "favorites")
    {
        fillSitesCbx(iviApi.favCount());
    }
    else
    {
        int catId = ui->cbxIviCategory->itemData(ui->cbxIviCategory->currentIndex()).toInt();
        int genId = ui->cbxIviGenre->itemData(ui->cbxIviGenre->currentIndex()).toInt();
        int count = 0;

        if ((catId == -1) && (genId == -1))
        {
            foreach (const ivi::SCat& cat, mIviCats)
            {
                foreach (const ivi::SGenre genre, cat.mGenres)
                {
                    count += genre.mCount;
                }
            }
        }
        else if (genId == -1)
        {
            foreach (const ivi::SGenre genre, mIviCats.value(catId).mGenres)
            {
                count += genre.mCount;
            }
        }
        else
        {
            count = mIviCats.value(catId).mGenres.value(genId).mCount;
        }

        fillSitesCbx(count);
    }

    getVideos();
}

//------------------------------------------------------------------------------
//! @brief      previous button was pressed
//------------------------------------------------------------------------------
void CVodIvi::on_btnIviPrevSite_clicked()
{
    int siteIdx   = ui->cbxIviSites->currentIndex();

    if (siteIdx > 0)
    {
        siteIdx--;
        ui->cbxIviSites->setCurrentIndex(siteIdx);
        on_cbxIviSites_activated(siteIdx);
    }
}

//------------------------------------------------------------------------------
//! @brief      next button was pressed
//------------------------------------------------------------------------------
void CVodIvi::on_btnIviNextSite_clicked()
{
    int siteIdx   = ui->cbxIviSites->currentIndex();
    int siteCount = ui->cbxIviSites->count();

    if (siteIdx < (siteCount -1))
    {
        siteIdx++;
        ui->cbxIviSites->setCurrentIndex(siteIdx);
        on_cbxIviSites_activated(siteIdx);
    }
}

//------------------------------------------------------------------------------
//! @brief      sites box was activated
//!
//! @param[in]  index  The index
//------------------------------------------------------------------------------
void CVodIvi::on_cbxIviSites_activated(int index)
{
    int siteCount = ui->cbxIviSites->count();
    ui->btnIviPrevSite->setDisabled(index == 0);
    ui->btnIviNextSite->setDisabled((index + 1) == siteCount);
    getVideos();
}

//------------------------------------------------------------------------------
//! @brief      categories box was activated
//!
//! @param[in]  index  The index
//------------------------------------------------------------------------------
void CVodIvi::on_cbxIviCategory_activated(int index)
{
    ui->cbxIviGenre->clear();
    int catId     = ui->cbxIviCategory->itemData(index).toInt();

    // add "all" ...
    ui->cbxIviGenre->addItem(tr("All"), -1);

/*
    if (catId == -1)
    {
        foreach(const ivi::SCat& cat, mIviCats)
        {
            foreach(const ivi::SGenre& genre, cat.mGenres)
            {
                ui->cbxIviGenre->addItem(cat.mTitle + ": " + genre.mTitle, genre.mId);
            }
        }
    }
    else
*/
    if (catId != -1)
    {
        foreach(ivi::SGenre oneGenre, mIviCats.value(catId).mGenres)
        {
            ui->cbxIviGenre->addItem(oneGenre.mTitle, oneGenre.mId);
        }
    }

    ui->cbxIviGenre->setCurrentIndex(0);
    on_cbxIviGenre_activated(0);
}

//------------------------------------------------------------------------------
//! @brief      get categories sent by api
//!
//! @param[in]  cats  The categories
//------------------------------------------------------------------------------
void CVodIvi::slotCatchCategories(ivi::CategoryMap cats)
{
    mIviCats = cats;
    ui->cbxIviCategory->clear();

    // add "all" entry ...
    ui->cbxIviCategory->addItem(tr("All"), -1);

    foreach(ivi::SCat oneCat, mIviCats)
    {
#ifdef __TRACE_IVI
        mInfo(tr("Found category '%1'(%2) ...").arg(oneCat.mTitle).arg(oneCat.mId));
#endif // __TRACE_IVI

        ui->cbxIviCategory->addItem(oneCat.mTitle, oneCat.mId);

#ifdef __TRACE_IVI
        foreach(ivi::SGenre oneGenre, oneCat.mGenres)
            mInfo(tr("  \\_Genre '%1'(%2) (#%3) ...")
                  .arg(oneGenre.mTitle).arg(oneGenre.mId)
                  .arg(oneGenre.mCount));
#endif // __TRACE_IVI
    }

    ui->cbxIviCategory->setCurrentIndex(0);
    on_cbxIviCategory_activated(0);
}

//------------------------------------------------------------------------------
//! @brief      get countries sent by api
//!
//! @param[in]  countr  The countries
//------------------------------------------------------------------------------
void CVodIvi::slotCatchCountries(ivi::CountryMap countr)
{
#ifdef __TRACE_IVI
    foreach(ivi::SCountry country, countr)
    {
        mInfo(tr("Found country #%1 '%2'[%3] ...")
              .arg(country.mId).arg(country.mName).arg(country.mShort));
    }
#else
    Q_UNUSED(countr)
#endif // __TRACE_IVI
    iviApi.getGenres();
}

//------------------------------------------------------------------------------
//! @brief      get videos sent by api
//!
//! @param[in]  videos  The videos
//------------------------------------------------------------------------------
void CVodIvi::slotCatchVideos(cparser::VideoList videos)
{
#ifdef __TRACE_IVI
    foreach(cparser::SVodVideo video, videos)
    {

        mInfo(tr("Found Video #%1 '%2'(%3 %4) ...")
              .arg(video.uiVidId).arg(video.sName)
              .arg(video.sCountry).arg(video.sYear));
    }
#endif // __TRACE_IVI

    QString genre;
    if (ui->cbxIviLastOrBest->itemData(ui->cbxIviLastOrBest->currentIndex()) == "favorites")
    {
        genre = ui->cbxIviLastOrBest->currentText();
    }
    else
    {
        genre = QString("%1: %2")
                .arg(ui->cbxIviCategory->currentText())
                .arg(ui->cbxIviGenre->currentText());
    }

    if ((videos.count() > 0) && !videos.at(0).sCompName.isEmpty())
    {
        genre += QString(": %1").arg(videos.at(0).sCompName);
    }

    ui->iviBrowser->displayVodList(videos, genre, mCompId != -1);
}

//------------------------------------------------------------------------------
//! @brief      get video info sent by api
//!
//! @param[in]  video  The video info
//------------------------------------------------------------------------------
void CVodIvi::slotCatchVideoInfo(cparser::SVodVideo video)
{
    ui->iviBrowser->displayVideoDetails(video);

    if (video.vVodFiles.isEmpty())
    {
        QMessageBox::critical(this,
          tr("Error"),
          tr("Sorry, content '%1' is currently not available in your country!").arg(video.sName));
    }
}

//------------------------------------------------------------------------------
//! @brief      fill sort combo box
//!
//! @param      idx default index
//------------------------------------------------------------------------------
void CVodIvi::fillSortCbx(int idx)
{
    ui->cbxIviLastOrBest->clear();
    ui->cbxIviLastOrBest->addItem(tr("Popularity"), QString("pop"));
    ui->cbxIviLastOrBest->addItem(tr("Newest"), QString("new"));
    ui->cbxIviLastOrBest->addItem(tr("IVI Rating"), QString("ivi"));
    ui->cbxIviLastOrBest->addItem(tr("KP Rating"), QString("kp"));
    ui->cbxIviLastOrBest->addItem(tr("IMDB Rating"), QString("imdb"));
    ui->cbxIviLastOrBest->addItem(tr("Budget"), QString("budget"));
    ui->cbxIviLastOrBest->addItem(tr("My Favourites"), QString("favorites"));
    ui->cbxIviLastOrBest->setCurrentIndex(idx);
}

//------------------------------------------------------------------------------
//! @brief      fill sites combo box
//!
//! @param[in]  count  The count
//------------------------------------------------------------------------------
void CVodIvi::fillSitesCbx(int count)
{
    int sites = count / VIDEOS_PER_SITE;

    ui->cbxIviSites->clear();

    if (count % VIDEOS_PER_SITE)
    {
        sites ++;
    }

    for (int i = 0; i < sites; i++)
    {
        ui->cbxIviSites->addItem(QString::number(i + 1), i * VIDEOS_PER_SITE);
    }

    ui->cbxIviSites->setCurrentIndex(0);
    on_cbxIviSites_activated(0);
}

//------------------------------------------------------------------------------
//! @brief      Gets the filter data from GUI
//!
//! @param[out] filter  The filter data
//------------------------------------------------------------------------------
void CVodIvi::getFilterData(ivi::SVideoFilter &filter)
{
    filter.mCatId  = ui->cbxIviCategory->itemData(ui->cbxIviCategory->currentIndex()).toInt();
    filter.mGenId  = ui->cbxIviGenre->itemData(ui->cbxIviGenre->currentIndex()).toInt();
    filter.mSort   = ui->cbxIviLastOrBest->itemData(ui->cbxIviLastOrBest->currentIndex()).toString();
    filter.mFrom   = ui->cbxIviSites->itemData(ui->cbxIviSites->currentIndex()).toInt();
    filter.mTo     = filter.mFrom + (VIDEOS_PER_SITE - 1);
    filter.mSearch = ui->lineIviSearch->text();
    filter.mCompId = mCompId;
}

//------------------------------------------------------------------------------
//! @brief      request videos from api
//------------------------------------------------------------------------------
void CVodIvi::getVideos()
{
    ivi::SVideoFilter filter;
    getFilterData(filter);
    iviApi.getVideos(filter);
}

//------------------------------------------------------------------------------
//! @brief      link in ivi browser was clicked
//!
//! @param[in]  arg1  link url
//------------------------------------------------------------------------------
void CVodIvi::on_iviBrowser_anchorClicked(const QUrl &arg1)
{
    QString action = arg1.queryItemValue("action");
    int     vodId, kind, count = -1;
    QString sUrl;

    if (action == "vod_info")
    {
        ui->iviBrowser->saveScrollPos();
        vodId = arg1.queryItemValue("vodid").toInt();
        kind  = arg1.queryItemValue("kind").toInt();

        if (arg1.hasQueryItem("count"))
        {
            count = arg1.queryItemValue("count").toInt();
        }

        if ((ivi::eKind)kind == ivi::KIND_VIDEO)
        {
            iviApi.getVideoInfo(vodId, (ivi::eKind)kind);
        }
        else
        {
            mCompId = vodId;
            fillSitesCbx(count);
        }
    }
    else if (action == "backHome")
    {
        mCompId = -1;

        // force reload ...
        on_cbxIviLastOrBest_activated(ui->cbxIviLastOrBest->currentIndex());
    }
    else if (action == "backtolist")
    {
        if (ui->cbxIviLastOrBest->itemData(ui->cbxIviLastOrBest->currentIndex()).toString() == "favorites")
        {
            getVideos();
        }
        else
        {
            // re-create last used site and position ...
            ui->iviBrowser->recreateVodList();
        }
    }
    else if (action == "play")
    {
        // ivi should have set video_url ...
        sUrl = QUrlEx::fromPercentEncoding(arg1.queryItemValue("video_url").toUtf8());
        mInfo(tr("Start ivi play with URL %1 ...").arg(sUrl));
        emit sigPlay(sUrl);

        mPrepContData         = iviApi.getContentData();
        mPrepContData.mFormat = arg1.queryItemValue("content_format").toUtf8();
    }
    else if (action == "record")
    {
        // ivi should have set video_url ...
        sUrl = QUrlEx::fromPercentEncoding(arg1.queryItemValue("video_url").toUtf8());
        mInfo(tr("Start ivi record with URL %1 ...").arg(sUrl));
        emit sigRecord(sUrl);

        mPrepContData         = iviApi.getContentData();
        mPrepContData.mFormat = arg1.queryItemValue("content_format").toUtf8();
    }
    else if (action == "add_fav")
    {
        vodId = arg1.queryItemValue("vodid").toInt();
        kind  = arg1.queryItemValue("kind").toInt();
        iviApi.addFav(vodId, (ivi::eKind)kind);
    }
    else if (action == "del_fav")
    {
        vodId = arg1.queryItemValue("vodid").toInt();
        kind  = arg1.queryItemValue("kind").toInt();
        iviApi.delFav(vodId, (ivi::eKind)kind);
    }
}

//------------------------------------------------------------------------------
//! @brief      search string was edited
//!
//! @param[in]  arg1  search string
//------------------------------------------------------------------------------
void CVodIvi::on_lineIviSearch_textEdited(const QString &arg1)
{
    if (arg1.length() > 2)
    {
        tIviSearch.start();
    }
    else if (arg1.length() == 0)
    {
        getVideos();
    }
}

//------------------------------------------------------------------------------
//! @brief      0.5 seconds no edit in ivi search line
//------------------------------------------------------------------------------
void CVodIvi::slotTimerIviSearch()
{
    getVideos();
}

//------------------------------------------------------------------------------
//! @brief      clear ivi search line
//------------------------------------------------------------------------------
void CVodIvi::on_btnCleanIviSearch_clicked()
{
    ui->lineIviSearch->setText("");
    on_lineIviSearch_textEdited("");
}

//------------------------------------------------------------------------------
//! @brief      ivi main feature reached, start statistics ...
//------------------------------------------------------------------------------
void CVodIvi::slotIviMainFeature()
{
    iviStats.start(mPrepContData);
}
