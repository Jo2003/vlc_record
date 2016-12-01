/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     cvodivi.h
 *
 *  @author   Jo2003
 *
 *  @date     18.11.2016
 *
 *  $Id$
 *
 *///------------------------- (c) 2016 by Jo2003  --------------------------
#ifndef __20161118_CVODIVI_H
    #define __20161118_CVODIVI_H

#include <QWidget>
#include <QTimer>
#include <QEvent>
#include "cvodbrowser.h"
#include "civiapi.h"
#include "cpixloader.h"

namespace Ui {
    class CVodIvi;
}

///
/// \brief The CVodIvi class
///
class CVodIvi : public QWidget
{
    Q_OBJECT

public:
    CVodIvi(QWidget *parent = 0);
    virtual ~CVodIvi();
    void setPixCache(CPixLoader* cache);
    void setIviSession(const QString& str);
    CVodBrowser *iviBrowser();

private slots:
    void on_cbxIviGenre_activated(int index);
    void on_cbxIviLastOrBest_activated(int index);
    void on_btnIviPrevSite_clicked();
    void on_btnIviNextSite_clicked();
    void on_cbxIviSites_activated(int index);
    void on_cbxIviCategory_activated(int index);
    void slotCatchCategories(ivi::CategoryMap cats);
    void slotCatchCountries(ivi::CountryMap countr);
    void slotCatchVideos(cparser::VideoList videos);
    void slotCatchVideoInfo(cparser::SVodVideo video);
    void on_iviBrowser_anchorClicked(const QUrl &arg1);
    void on_lineIviSearch_textEdited(const QString &arg1);
    void slotTimerIviSearch();

    void on_btnCleanIviSearch_clicked();

protected:
    virtual void changeEvent(QEvent *e);
    void fillSortCbx(int idx = 0);
    void fillSitesCbx(int count);
    void getFilterData(ivi::SVideoFilter& filter);
    void getVideos();

    CIviApi          iviApi;
    ivi::CategoryMap mIviCats;
    QTimer           tIviSearch;
    int              mCompId;

private:
    Ui::CVodIvi *ui;

signals:
    void sigPlay(QString url);
    void sigRecord(QString url);
};

#endif // __20161118_CVODIVI_H
