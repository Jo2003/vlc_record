/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qstrstandarddlg.cpp
 *
 *  @author   Jo2003
 *
 *  @date     29.08.2015
 *
 *  $Id$
 *
 *///------------------------- (c) 2015 by Jo2003  --------------------------
#include "qstrstandarddlg.h"
#include "ui_qstrstandarddlg.h"
#include <QMessageBox>

//---------------------------------------------------------------------------
//! \brief   create dialg
//
//! \author  Jo2003
//! \date    29.08.2015
//
//! \param   parent [in ] (QWidget*) pointer to parent widget
//---------------------------------------------------------------------------
QStrStandardDlg::QStrStandardDlg(QWidget *parent, Qt::WindowFlags f) :
   QDialog(parent, f),
   ui(new Ui::QStrStandardDlg)
{
   ui->setupUi(this);
   setAttribute(Qt::WA_DeleteOnClose, false);
}

//---------------------------------------------------------------------------
//! \brief   destroy dialog
//
//! \author  Jo2003
//! \date    29.08.2015
//---------------------------------------------------------------------------
QStrStandardDlg::~QStrStandardDlg()
{
    delete ui;
}

//---------------------------------------------------------------------------
//! \brief   set stream standard data
//
//! \author  Jo2003
//! \date    29.08.2015
//
//! \param   data [in] (const cparser::QStrStdMap &) stream std data
//! \param   curr [in] (const QString &) current standard
//---------------------------------------------------------------------------
void QStrStandardDlg::setStrStdData(const cparser::QStrStdMap &data, const QString &curr)
{
    QListWidgetItem *pItem;
    cparser::QStrStdMap::ConstIterator cit;
    mStrStdMap   = data;
    mStrCurVal   = curr;
    int     idx  = 0;
    int     cidx = 0;
    QString descr;

    // clear list ...
    ui->listStrStandards->clear();

    // fill list ...
    for (cit = mStrStdMap.constBegin(); cit != mStrStdMap.constEnd(); cit ++)
    {
        pItem = new QListWidgetItem(cit.value().sName);
        pItem->setData(Qt::UserRole, cit.key());

        ui->listStrStandards->addItem(pItem);

        if (cit.key() == mStrCurVal)
        {
            cidx  = idx;
            descr = cit.value().sDescr;
        }

        idx++;
    }

    // mark current selection ...
    ui->listStrStandards->setCurrentRow(cidx);
    ui->txtStrStdDescr->setPlainText(descr);
}

//---------------------------------------------------------------------------
//! \brief   set current name
//
//! \author  Jo2003
//! \date    31.08.2015
//
//! \param   s [in] (const QString&) name to set as current
//---------------------------------------------------------------------------
void QStrStandardDlg::setCurrName(const QString &s)
{
    QListWidgetItem* pItem;

    for (int i = 0; i < ui->listStrStandards->count(); i++)
    {
        pItem = ui->listStrStandards->item(i);

        if (pItem->text() == s)
        {
            ui->listStrStandards->setCurrentRow(i);
            break;
        }
    }
}

//---------------------------------------------------------------------------
//! \brief   get current value
//
//! \author  Jo2003
//! \date    29.08.2015
//
//! \return  current value as string
//---------------------------------------------------------------------------
QString QStrStandardDlg::getCurrVal()
{
    QString sRet;
    QListWidgetItem *pCurr = ui->listStrStandards->currentItem();

    if (pCurr != NULL)
    {
        sRet = pCurr->data(Qt::UserRole).toString();
    }

    return sRet;
}

//---------------------------------------------------------------------------
//! \brief   get current name
//
//! \author  Jo2003
//! \date    29.08.2015
//
//! \return  current name as string
//---------------------------------------------------------------------------
QString QStrStandardDlg::getCurrName()
{
    QString sRet;
    QString key = getCurrVal();

    if (key.isEmpty())
    {
        sRet = tr("N/A");
    }
    else
    {
        sRet = mStrStdMap.value(key).sName;
    }

    return sRet;
}

//---------------------------------------------------------------------------
//! \brief   list item was changed
//
//! \author  Jo2003
//! \date    29.08.2015
//---------------------------------------------------------------------------
void QStrStandardDlg::on_listStrStandards_itemSelectionChanged()
{
    QString key = getCurrVal();

    if (!key.isEmpty() && !mStrStdMap.isEmpty())
    {
        ui->txtStrStdDescr->setPlainText(mStrStdMap.value(key).sDescr);
    }
}
