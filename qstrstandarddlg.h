/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qstrstandarddlg.h
 *
 *  @author   Jo2003
 *
 *  @date     29.08.2015
 *
 *  $Id$
 *
 *///------------------------- (c) 2015 by Jo2003  --------------------------
#ifndef __20150829_QSTRSTANDARDDLG_H
   #define __20150829_QSTRSTANDARDDLG_H

#include <QDialog>
#include <QMap>
#include <QListWidgetItem>

#include "cparser.h"

namespace Ui {
    class QStrStandardDlg;
}

//---------------------------------------------------------------------------
//! \class   QStrStandardDlg
//! \date    29.08.2015
//! \author  Jo2003
//! \brief   dialog to chose stream standard to use
//---------------------------------------------------------------------------
class QStrStandardDlg : public QDialog
{
    Q_OBJECT

public:
    QStrStandardDlg(QWidget * parent = 0, Qt::WindowFlags f = 0);
    ~QStrStandardDlg();
    void setStrStdData(const cparser::QStrStdMap& data, const QString& curr);
    void setCurrName (const QString& s);
    QString getCurrVal();
    QString getCurrName();

private slots:
    void on_listStrStandards_itemSelectionChanged();

private:
    Ui::QStrStandardDlg *ui;
    cparser::QStrStdMap  mStrStdMap;
    QString              mStrCurVal;
};

#endif // __20150829_QSTRSTANDARDDLG_H
