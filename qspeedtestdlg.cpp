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

    ui->progressData->setMinimum(0);
    ui->progressData->setMaximum(__DEF_DOWN_SIZE * 1024 * 1024);
    ui->hSliderSpeed->setMinimum(0);
    ui->hSliderSpeed->setMaximum(16000);

    QTableWidgetItem *pItem;
    pItem= new QTableWidgetItem(tr("Stream Server"));
    ui->tableResults->setHorizontalHeaderItem(0, pItem);
    pItem= new QTableWidgetItem(tr("Download Speed"));
    ui->tableResults->setHorizontalHeaderItem(1, pItem);

    connect (&m_strLoader, SIGNAL(sigDwnSpeed(int,int)), this, SLOT(slotSpeedData(int,int)));
    connect (&m_strLoader, SIGNAL(sigSpeedTestEnd()), this, SLOT(slotSpeedTestDone()));
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

//---------------------------------------------------------------------------
//! \brief   fastest server to save?
//
//! \author  Jo2003
//! \date    16.09.2015
//
//! \return  true -> save; false -> ignore
//---------------------------------------------------------------------------
bool QSpeedTestDlg::save()
{
    return ui->chkSaveFastest->isChecked();
}

//---------------------------------------------------------------------------
//! \brief   things to do when the dialog is shown
//
//! \author  Jo2003
//! \date    17.09.2015
//
//! \param   e [in] (QShowEvent*) event pointer
//---------------------------------------------------------------------------
void QSpeedTestDlg::showEvent(QShowEvent *e)
{
    QDialog::showEvent(e);

    ui->tableResults->clearContents();
    ui->tableResults->setRowCount(0);
    int w = ui->tableResults->width();
    ui->tableResults->setColumnWidth(0, w / 2);
    ui->chkSaveFastest->setChecked(false);
}

//---------------------------------------------------------------------------
//! \brief   catch language change
//
//! \author  Jo2003
//! \date    17.09.2015
//
//! \param   e [in] (QEvent*) change event
//---------------------------------------------------------------------------
void QSpeedTestDlg::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);

        QTableWidgetItem *pItem;
        pItem= new QTableWidgetItem(tr("Stream Server"));
        ui->tableResults->setHorizontalHeaderItem(0, pItem);
        pItem= new QTableWidgetItem(tr("Download Speed"));
        ui->tableResults->setHorizontalHeaderItem(1, pItem);
    }

    QDialog::changeEvent(e);
}

//---------------------------------------------------------------------------
//! \brief   speed data recieved
//
//! \author  Jo2003
//! \date    16.09.2015
//
//! \param   ms [in] (int) time in milli seconds
//! \param   bytes [in] (int) bytes downloaded
//---------------------------------------------------------------------------
void QSpeedTestDlg::slotSpeedData(int ms, int bytes)
{
    if ((ms > 0) && (bytes > 0))
    {
        m_dSpeed = (double)(bytes * 8.0 / 1024.0 / 1024.0) / (double)(ms / 1000.0);
        double mb = ((double)bytes / 1024.0 / 1024.0);

        if (m_dSpeed > 0.2)
        {
            QString s = tr("%1 Mbit/s").arg(m_dSpeed, 0, 'f', 3);
            ui->labSpeed->setText(s);

            s = tr("%1 MB").arg(mb, 0, 'f', 3);
            ui->labData->setText(s);

            if (mb >= __DEF_DOWN_SIZE)
            {
                ui->progressData->setValue(ui->progressData->maximum());
            }
            else
            {
                ui->progressData->setValue(bytes);
            }
            ui->hSliderSpeed->setValue(1024 * m_dSpeed);
        }
    }
}

//---------------------------------------------------------------------------
//! \brief   test done, insert table row, enable dialog items
//
//! \author  Jo2003
//! \date    17.09.2015
//---------------------------------------------------------------------------
void QSpeedTestDlg::slotSpeedTestDone()
{
    int row = ui->tableResults->rowCount();
    ui->tableResults->insertRow(row);

    QTableWidgetItem *pItem;
    pItem = new QTableWidgetItem(ui->cbxServer->currentText());
    pItem->setData(Qt::UserRole, m_spdData.at(ui->cbxServer->currentIndex()).ip);
    ui->tableResults->setItem(row, 0, pItem);
    pItem = new QTableWidgetItem(tr("%1 MBit/s").arg(m_dSpeed, 0, 'f', 3));
    pItem->setData(Qt::UserRole, m_dSpeed);
    ui->tableResults->setItem(row, 1, pItem);

    ui->btnStart->setEnabled(true);
    ui->btnStop->setEnabled(false);
    ui->cbxServer->setEnabled(true);
}

//---------------------------------------------------------------------------
//! \brief   start speed test for chosen server
//
//! \author  Jo2003
//! \date    17.09.2015
//---------------------------------------------------------------------------
void QSpeedTestDlg::on_btnStart_clicked()
{
    m_dSpeed = 0;
    m_strLoader.speedTest(m_spdData.at(ui->cbxServer->currentIndex()).url, __DEF_DOWN_SIZE);
    ui->progressData->setValue(0);
    ui->hSliderSpeed->setValue(0);

    ui->btnStart->setEnabled(false);
    ui->btnStop->setEnabled(true);
    ui->cbxServer->setEnabled(false);
}

//---------------------------------------------------------------------------
//! \brief   stop running speed test
//
//! \author  Jo2003
//! \date    17.09.2015
//---------------------------------------------------------------------------
void QSpeedTestDlg::on_btnStop_clicked()
{
    m_strLoader.endSpeedTest();
}

//---------------------------------------------------------------------------
//! \brief   store ip of fastest server
//
//! \author  Jo2003
//! \date    18.09.2015
//---------------------------------------------------------------------------
void QSpeedTestDlg::on_buttonBox_accepted()
{
    m_chosenOne.clear();

    if (save())
    {
        double  val = 0.0, tmpVal;
        for (int i = 0; i < ui->tableResults->rowCount(); i++)
        {
            tmpVal = ui->tableResults->item(i, 1)->data(Qt::UserRole).toDouble();

            if (tmpVal > val)
            {
                val         = tmpVal;
                m_chosenOne = ui->tableResults->item(i, 0)->data(Qt::UserRole).toString();
            }
        }
    }
}
