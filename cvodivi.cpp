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

//------------------------------------------------------------------------------
//! @brief      Constructs the object.
//!
//! @param      parent  The parent
//------------------------------------------------------------------------------
CVodIvi::CVodIvi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CVodIvi)
{
    ui->setupUi(this);
    connect (&iviApi, SIGNAL(sigCategories(ivi::CategoryMap)), this, SLOT(slotCatchCategories(ivi::CategoryMap)));
    connect (&iviApi, SIGNAL(sigCountries(ivi::CountryMap)), this, SLOT(slotCatchCountries(ivi::CountryMap)));
    connect (&iviApi, SIGNAL(sigVideoList(cparser::VideoList)), this, SLOT(slotCatchVideos(cparser::VideoList)));
    connect (&iviApi, SIGNAL(sigVideoInfo(cparser::SVodVideo)), this, SLOT(slotCatchVideoInfo(cparser::SVodVideo)));
}

//------------------------------------------------------------------------------
//! @brief      Destroys the object.
//------------------------------------------------------------------------------
CVodIvi::~CVodIvi()
{
    delete ui;
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
    iviApi.setSessionKey(str);
    fillSortCbx();
    iviApi.getCountries();
}

//------------------------------------------------------------------------------
//! @brief      genre box was activated
//!
//! @param[in]  index  The index
//------------------------------------------------------------------------------
void CVodIvi::on_cbxIviGenre_activated(int index)
{
    int         catId = ui->cbxIviCategory->itemData(ui->cbxIviCategory->currentIndex()).toInt();
    int         genId = ui->cbxIviGenre->itemData(index).toInt();
    ivi::SCat   cat   = mIviCats.value(catId);
    ivi::SGenre genre = cat.mGenres.value(genId);

    fillSitesCbx(genre.mNoContent);
}

//------------------------------------------------------------------------------
//! @brief      sort box was activated
//!
//! @param[in]  index  The index
//------------------------------------------------------------------------------
void CVodIvi::on_cbxIviLastOrBest_activated(int index)
{
    (void)index;
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
    ivi::SCat cat = mIviCats.value(catId);

    foreach(ivi::SGenre oneGenre, cat.mGenres)
    {
        ui->cbxIviGenre->addItem(oneGenre.mTitle, oneGenre.mId);
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

    foreach(ivi::SCat oneCat, mIviCats)
    {
#ifdef __TRACE
        mInfo(tr("Found category '%1'(%2) ...").arg(oneCat.mTitle).arg(oneCat.mId));
#endif // __TRACE

        ui->cbxIviCategory->addItem(oneCat.mTitle, oneCat.mId);

#ifdef __TRACE
        foreach(ivi::SGenre oneGenre, oneCat.mGenres)
            mInfo(tr("  \\_Genre '%1'(%2) ...").arg(oneGenre.mTitle).arg(oneGenre.mId));
#endif // __TRACE
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
#ifdef __TRACE
    foreach(ivi::SCountry country, countr)
    {
        mInfo(tr("Found country #%1 '%2'[%3] ...")
              .arg(country.mId).arg(country.mName).arg(country.mShort));
    }
#else
    Q_UNUSED(countr)
#endif // __TRACE
    iviApi.getGenres();
}

//------------------------------------------------------------------------------
//! @brief      get videos sent by api
//!
//! @param[in]  videos  The videos
//------------------------------------------------------------------------------
void CVodIvi::slotCatchVideos(cparser::VideoList videos)
{
#ifdef __TRACE
    foreach(cparser::SVodVideo video, videos)
    {

        mInfo(tr("Found Video #%1 '%2'(%3 %4) ...")
              .arg(video.uiVidId).arg(video.sName)
              .arg(video.sCountry).arg(video.sYear));
    }
#endif // __TRACE
    QString genre = QString("%1: %2")
            .arg(ui->cbxIviCategory->currentText())
            .arg(ui->cbxIviGenre->currentText());

    ui->iviBrowser->displayVodList(videos, genre);
}

//------------------------------------------------------------------------------
//! @brief      get video info sent by api
//!
//! @param[in]  video  The video info
//------------------------------------------------------------------------------
void CVodIvi::slotCatchVideoInfo(cparser::SVodVideo video)
{
    ui->iviBrowser->displayVideoDetails(video);
}

//------------------------------------------------------------------------------
//! @brief      fill sort combo box
//------------------------------------------------------------------------------
void CVodIvi::fillSortCbx()
{
    ui->cbxIviLastOrBest->clear();
    ui->cbxIviLastOrBest->addItem(tr("Popularity"), QString("pop"));
    ui->cbxIviLastOrBest->addItem(tr("Newest"), QString("new"));
    ui->cbxIviLastOrBest->addItem(tr("IVI Rating"), QString("ivi"));
    ui->cbxIviLastOrBest->addItem(tr("KP Rating"), QString("kp"));
    ui->cbxIviLastOrBest->addItem(tr("IMDB Rating"), QString("imdb"));
    ui->cbxIviLastOrBest->addItem(tr("Budget"), QString("budget"));
    ui->cbxIviLastOrBest->setCurrentIndex(0);
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
    filter.mGenId = ui->cbxIviGenre->itemData(ui->cbxIviGenre->currentIndex()).toInt();
    filter.mSort  = ui->cbxIviLastOrBest->itemData(ui->cbxIviLastOrBest->currentIndex()).toString();
    filter.mFrom  = ui->cbxIviSites->itemData(ui->cbxIviSites->currentIndex()).toInt();
    filter.mTo    = filter.mFrom + (VIDEOS_PER_SITE - 1);
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
    int     vodId;

    if (action == "vod_info")
    {
        vodId = arg1.queryItemValue("vodid").toInt();
        iviApi.getVideoInfo(vodId);
    }
    else if (action == "backtolist")
    {
       // re-create last used site and position ...
       ui->iviBrowser->recreateVodList();
    }
}
