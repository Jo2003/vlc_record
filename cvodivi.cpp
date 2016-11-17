#include "cvodivi.h"
#include "ui_cvodivi.h"
#include "externals_inc.h"

CVodIvi::CVodIvi(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CVodIvi)
{
    ui->setupUi(this);
    connect (&iviApi, SIGNAL(sigCategories(ivi::CategoryMap)), this, SLOT(slotCatchCategories(ivi::CategoryMap)));
    connect (&iviApi, SIGNAL(sigCountries(ivi::CountryMap)), this, SLOT(slotCatchCountries(ivi::CountryMap)));
}

CVodIvi::~CVodIvi()
{
    delete ui;
}

void CVodIvi::setIviSession(const QString &str)
{
    iviApi.setSessionKey(str);
    fillSortCbx();
    iviApi.getCountries();
}

void CVodIvi::on_cbxIviGenre_activated(int index)
{
    int         catId = ui->cbxIviCategory->itemData(ui->cbxIviCategory->currentIndex()).toInt();
    int         genId = ui->cbxIviGenre->itemData(index).toInt();
    ivi::SCat   cat   = mIviCats.value(catId);
    ivi::SGenre genre = cat.mGenres.value(genId);

    fillSitesCbx(genre.mNoContent);
}

void CVodIvi::on_cbxIviLastOrBest_activated(int index)
{
    (void)index;
    getVideos();
}

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

void CVodIvi::on_cbxIviSites_activated(int index)
{
    int siteCount = ui->cbxIviSites->count();
    ui->btnIviPrevSite->setDisabled(index == 0);
    ui->btnIviNextSite->setDisabled((index + 1) == siteCount);
    getVideos();
}

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

void CVodIvi::slotCatchCategories(ivi::CategoryMap cats)
{
    mIviCats = cats;
    mIviGenres.clear();
    ui->cbxIviCategory->clear();

    foreach(ivi::SCat oneCat, mIviCats)
    {
#ifdef __TRACE
        mInfo(tr("Found category '%1'(%2) ...").arg(oneCat.mTitle).arg(oneCat.mId));
#endif // __TRACE

        ui->cbxIviCategory->addItem(oneCat.mTitle, oneCat.mId);


        foreach(ivi::SGenre oneGenre, oneCat.mGenres)
        {
#ifdef __TRACE
            mInfo(tr("  \\_Genre '%1'(%2) ...").arg(oneGenre.mTitle).arg(oneGenre.mId));
#endif // __TRACE

            mIviGenres.insert(oneGenre.mId, oneGenre);
        }
    }

    ui->cbxIviCategory->setCurrentIndex(0);
    on_cbxIviCategory_activated(0);
}

void CVodIvi::slotCatchCountries(ivi::CountryMap countr)
{
    mIviCountries = countr;

#ifdef __TRACE
    foreach(ivi::SCountry country, mIviCountries)
    {
        mInfo(tr("Found country #%1 '%2'[%3] ...")
              .arg(country.mId).arg(country.mName).arg(country.mShort));
    }

#endif // __TRACE
    iviApi.getGenres();
}

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

void CVodIvi::getFilterData(ivi::SVideoFilter &filter)
{
    filter.mGenId = ui->cbxIviGenre->itemData(ui->cbxIviGenre->currentIndex()).toInt();
    filter.mSort  = ui->cbxIviLastOrBest->itemData(ui->cbxIviLastOrBest->currentIndex()).toString();
    filter.mFrom  = ui->cbxIviSites->itemData(ui->cbxIviSites->currentIndex()).toInt();
    filter.mTo    = filter.mFrom + (VIDEOS_PER_SITE - 1);
}

void CVodIvi::getVideos()
{
    ivi::SVideoFilter filter;
    getFilterData(filter);
    iviApi.getVideos(filter);
}

