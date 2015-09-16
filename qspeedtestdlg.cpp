/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qspeedtestdlg.cpp
 *
 *  @author   Jo2003
 *
 *  @date     16.09.2015
 *
 *  $Id$
 *
 *///------------------------- (c) 2015 by Jo2003  --------------------------
#include "qspeedtestdlg.h"
#include "ui_qspeedtestdlg.h"

//---------------------------------------------------------------------------
//! \brief   create dialg
//
//! \author  Jo2003
//! \date    16.09.2015
//
//! \param   parent [in ] (QWidget*) pointer to parent widget
//! \param   f [in] (Qt::WindowFlag) windows flags
//---------------------------------------------------------------------------
QSpeedTestDlg::QSpeedTestDlg(QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent, f),
    ui(new Ui::QSpeedTestDlg)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose, false);
}

//---------------------------------------------------------------------------
//! \brief   destroy dialog
//
//! \author  Jo2003
//! \date    16.09.2015
//---------------------------------------------------------------------------
QSpeedTestDlg::~QSpeedTestDlg()
{
    delete ui;
}

//---------------------------------------------------------------------------
//! \brief   set speed test data
//
//! \author  Jo2003
//! \date    16.09.2015
//
//! \param   spdData [in] (const QSpeedDataVector &) speed test data
//---------------------------------------------------------------------------
void QSpeedTestDlg::setData(const QSpeedDataVector &spdData)
{
    m_spdData = spdData;

    ui->cbxServer->clear();

    foreach (cparser::SSpeedTest dset, m_spdData)
    {
        ui->cbxServer->addItem(dset.descr);
    }

    ui->cbxServer->setCurrentIndex(0);
}

//---------------------------------------------------------------------------
//! \brief   get the chosen stream server ip
//
//! \author  Jo2003
//! \date    16.09.2015
//
//! \return  ip as string
//---------------------------------------------------------------------------
const QString &QSpeedTestDlg::chosenOne()
{
    /// \todo return correct one
    return m_chosenOne;
}

//---------------------------------------------------------------------------
//! \brief   return the chosen speed data
//
//! \author  Jo2003
//! \date    16.09.2015
//
//! \return  QSpeedDataVector
//---------------------------------------------------------------------------
const QSpeedDataVector &QSpeedTestDlg::data()
{
    return m_spdData;
}

