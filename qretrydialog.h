/*------------------------------ Information ---------------------------*//**
 *
 *  @file     qretrydialog.h
 *
 *  @author   Jo2003
 *
 *  @date     03.11.2015
 *
 *  @brief    dialog to show info and give choise "cancel, retry, re-login"
 *
 *///------------------------- (c) 2015 by Jo2003  --------------------------
#ifndef __20151103_QRETRYDIALOG_H
    #define __20151103_QRETRYDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QEvent>
#include <QDialogButtonBox>
#include <QShortcut>

namespace Ui {
    class QRetryDialog;
}

//---------------------------------------------------------------------------
//! \class   QRetryDialog
//! \date    03.11.2015
//! \author  Jo2003
//! \brief   dialog to show info and give choise "cancel, retry, re-login"
//---------------------------------------------------------------------------
class QRetryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QRetryDialog(QWidget *parent = 0);
    ~QRetryDialog();
    void touchButtonBox();
    void setMessage (const QString& msg, const QIcon& ico = QIcon(), const QString& caption = QString());

    QDialogButtonBox::ButtonRole lastRole() const { return mLastRole; }

protected:
    virtual void showEvent(QShowEvent* pEvent);
    virtual void changeEvent(QEvent* pEvent);

private:
    Ui::QRetryDialog*            ui;
    QPushButton*                 mpBtnReLogin;
    QDialogButtonBox::ButtonRole mLastRole;
    QShortcut                   *mpCopyText;

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);
    void copyToClipBoard();
};

#endif // __20151103_QRETRYDIALOG_H
