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
    void on_cbxVodLang_activated(int index);
    void on_cbxGenre_activated(int index);
    void on_cbxLastOrBest_activated(int index);
    void on_btnPrevSite_clicked();
    void on_btnNextSite_clicked();
    void on_cbxSites_activated(int index);

protected:
    CIviApi iviApi;

private:
    Ui::CVodIvi *ui;
};

#endif // CVODIVI_H
