#ifndef CVODIVI_H
#define CVODIVI_H

#include <QWidget>
#include "cvodbrowser.h"
#include "civiapi.h"

namespace Ui {
    class CVodIvi;
}

class CVodIvi : public QWidget
{
    Q_OBJECT

public:
    CVodIvi(QWidget *parent = 0);
    virtual ~CVodIvi();
    void setIviSession(const QString& str);

private slots:
    void on_cbxIviGenre_activated(int index);
    void on_cbxIviLastOrBest_activated(int index);
    void on_btnIviPrevSite_clicked();
    void on_btnIviNextSite_clicked();
    void on_cbxIviSites_activated(int index);
    void on_cbxIviCategory_activated(int index);
    void slotCatchCategories(ivi::CategoryMap cats);
    void slotCatchCountries(ivi::CountryMap countr);

protected:
    void fillSortCbx();
    void fillSitesCbx(int count);
    void getFilterData(ivi::SVideoFilter& filter);
    void getVideos();

    CIviApi          iviApi;
    ivi::CategoryMap mIviCats;
    ivi::GenreMap    mIviGenres;
    ivi::CountryMap  mIviCountries;

private:
    Ui::CVodIvi *ui;
};

#endif // CVODIVI_H
