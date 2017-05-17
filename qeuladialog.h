/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qeuladlg.h
 *
 *  @author   Jo2003
 *
 *  @date     16.05.2017
 *
 *  $Id$
 *
 *///------------------------- (c) 2017 by Jo2003  --------------------------
#ifndef __20170516_QEULADIALOG_H
    #define __20170516_QEULADIALOG_H

#include <QDialog>
#include <QPushButton>

namespace Ui {
    class QEulaDialog;
}

///
/// \brief The QEulaDialog class
///
class QEulaDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QEulaDialog(QWidget *parent = 0);
    ~QEulaDialog();

protected:
   virtual void changeEvent(QEvent *e);

private slots:
    void on_cbxLanguage_currentIndexChanged(QString str);
    void on_checkBox_clicked(bool checked);

private:
    Ui::QEulaDialog *ui;
    QPushButton *pBtnOk;
};

#endif // __20170516_QEULADIALOG_H
