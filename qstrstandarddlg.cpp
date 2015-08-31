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
//! \param   data [in] (const QStrStdMap &) stream std data
//! \param   curr [in] (const QString &) current standard
//---------------------------------------------------------------------------
void QStrStandardDlg::setStrStdData(const QStrStdMap &data, const QString &curr)
{
    QListWidgetItem *pItem;
    QStrStdMap::ConstIterator cit;
    mStrStdMap  = data;
    mStrCurVal  = curr;
    int    idx  = 0;
    int    cidx = 0;

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
            cidx = idx;
        }

        idx++;
    }

    // mark current selection ...
    ui->listStrStandards->setCurrentRow(cidx);
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
//
//! \param current [in] (QListWidgetItem *) pointer to current list item
//! \param previous [in] (QListWidgetItem *) pointer to previous list item
//---------------------------------------------------------------------------
void QStrStandardDlg::on_listStrStandards_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    Q_UNUSED(previous)
    QString currSel = current->data(Qt::UserRole).toString();
    ui->txtStrStdDescr->setPlainText(mStrStdMap.value(currSel).sDescr);
}
