/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qspeedtestdlg.h
 *
 *  @author   Jo2003
 *
 *  @date     16.09.2015
 *
 *  $Id$
 *
 *///------------------------- (c) 2015 by Jo2003  --------------------------
#ifndef __20150916_QSPEEDTESTDLG_H
    #define __20150916_QSPEEDTESTDLG_H

#include <QDialog>
#include "cparser.h"
#include "cstreamloader.h"
#include <QShowEvent>


#define __DEF_DOWN_SIZE 15 ///< 15 MB

//---------------------------------------------------------------------------
// Ui namespace
//---------------------------------------------------------------------------
namespace Ui {
    class QSpeedTestDlg;
}

//---------------------------------------------------------------------------
//! \class   QSpeedTestDlg
//! \date    16.09.2015
//! \author  Jo2003
//! \brief   dialog to check and chose stream server with fastest speed
//---------------------------------------------------------------------------
class QSpeedTestDlg : public QDialog
{
    Q_OBJECT

public:
    QSpeedTestDlg(QWidget * parent = 0, Qt::WindowFlags f = 0);
    ~QSpeedTestDlg();
    void setData (const QSpeedDataVector& spdData);
    const QString& chosenOne();
    const QSpeedDataVector& data();
    bool  save();

protected:
    CStreamLoader m_strLoader;
    virtual void showEvent(QShowEvent *e);
    virtual void changeEvent(QEvent *e);

private:
    Ui::QSpeedTestDlg *ui;
    QSpeedDataVector m_spdData;
    QString          m_chosenOne;
    double           m_dSpeed;

public slots:
    void on_btnStop_clicked();

private slots:
    void slotSpeedData(int ms, int bytes);
    void slotSpeedTestDone();
    void on_btnStart_clicked();
    void on_buttonBox_accepted();
};

#endif // __20150916_QSPEEDTESTDLG_H
