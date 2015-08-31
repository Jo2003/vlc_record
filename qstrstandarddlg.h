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

namespace Ui {
    class QStrStandardDlg;
}

/// \brief hold description data for stream standards
struct StrStdDescr {
    QString sName;
    QString sDescr;
};

/// \brief define type for stream standards
typedef QMap<QString, StrStdDescr> QStrStdMap;

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
    void setStrStdData(const QStrStdMap& data, const QString& curr);
    QString getCurrVal();
    QString getCurrName();

private slots:
    void on_listStrStandards_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);

private:
    Ui::QStrStandardDlg *ui;
    QStrStdMap mStrStdMap;
    QString    mStrCurVal;
};

#endif // __20150829_QSTRSTANDARDDLG_H
