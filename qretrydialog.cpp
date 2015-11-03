/*------------------------------ Information ---------------------------*//**
 *
 *  @file     qretrydialog.cpp
 *
 *  @author   Jo2003
 *
 *  @date     03.11.2015
 *
 *  @brief    dialog to show info and give choise "cancel, retry, re-login"
 *
 *///------------------------- (c) 2015 by Jo2003  --------------------------
#include "qretrydialog.h"
#include "ui_qretrydialog.h"

//---------------------------------------------------------------------------
//
//! \brief   constructor
//
//! \author  Jo2003
//! \date    03.11.2015
//
//! \param   [in] parent (QWidget *) pointer to parent widget
//
//---------------------------------------------------------------------------
QRetryDialog::QRetryDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QRetryDialog)
{
    ui->setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose, false);

    mpBtnReLogin = NULL;
    mLastRole    = QDialogButtonBox::InvalidRole;
}

//---------------------------------------------------------------------------
//
//! \brief   destructor
//
//! \author  Jo2003
//! \date    03.11.2015
//
//---------------------------------------------------------------------------
QRetryDialog::~QRetryDialog()
{
    delete ui;
}

//---------------------------------------------------------------------------
//
//! \brief   (re-)create re-login button
//
//! \author  Jo2003
//! \date    03.11.2015
//
//---------------------------------------------------------------------------
void QRetryDialog::touchButtonBox()
{
    if (mpBtnReLogin != NULL)
    {
        ui->buttonBox->removeButton(mpBtnReLogin);
        mpBtnReLogin = NULL;
    }

    mpBtnReLogin = ui->buttonBox->addButton(tr("Re-Login"), QDialogButtonBox::YesRole);
}

//---------------------------------------------------------------------------
//
//! \brief   set dialog messaage + icon
//
//! \author  Jo2003
//! \date    03.11.2015
//
//! \param   [in] msg (const QString &) message text
//! \param   [in] ico (const QIcon &) optional icon
//! \param   [in] caption (const QString&) optional caption
//
//---------------------------------------------------------------------------
void QRetryDialog::setMessage(const QString &msg, const QIcon &ico, const QString& caption)
{
    QPixmap msgPix;

    if (ico.isNull())
    {
        QIcon tmpIcon = style()->standardIcon(QStyle::SP_MessageBoxInformation);
        msgPix = tmpIcon.pixmap(64, 64);
    }
    else
    {
        msgPix = ico.pixmap(64, 64);
    }

    if (!caption.isEmpty())
    {
        setWindowTitle(caption);
    }

    ui->labIcon->setPixmap(msgPix);
    ui->labMsg->clear();
    ui->labMsg->setText(msg);
}

//---------------------------------------------------------------------------
//
//! \brief   complete gui before showing dialog
//
//! \author  Jo2003
//! \date    03.11.2015
//
//! \param   [in] pEvent (QShowEvent*) pointer to show event
//
//---------------------------------------------------------------------------
void QRetryDialog::showEvent(QShowEvent *pEvent)
{
    if (!mpBtnReLogin)
    {
        touchButtonBox();
    }

    QDialog::showEvent(pEvent);
}

//---------------------------------------------------------------------------
//
//! \brief   language change
//
//! \author  Jo2003
//! \date    03.11.2015
//
//! \param   [in] pEvent (QEvent*) pointer to event
//
//---------------------------------------------------------------------------
void QRetryDialog::changeEvent(QEvent *pEvent)
{
    if (pEvent->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);

        touchButtonBox();
    }

    QDialog::changeEvent(pEvent);
}

//---------------------------------------------------------------------------
//
//! \brief   one button in button box was clicked
//
//! \author  Jo2003
//! \date    03.11.2015
//
//! \param   [in] button (QAbstractButton*) pointer to clicked button
//
//---------------------------------------------------------------------------
void QRetryDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    QDialogButtonBox::ButtonRole role = ui->buttonBox->buttonRole(button);

    switch (role)
    {
    case QDialogButtonBox::AcceptRole:
    case QDialogButtonBox::YesRole:
        mLastRole = role;
        accept();
        break;

    case QDialogButtonBox::RejectRole:
        mLastRole = QDialogButtonBox::InvalidRole;
        reject();
        break;

    default:
        break;
    }
}
