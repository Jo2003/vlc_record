/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qftsettings.h
 *
 *  @author   Jo2003
 *
 *  @date     13.09.2011
 *
 *  $Id$
 *
 *///------------------------- (c) 2011 by Jo2003  --------------------------
#ifndef __20110913_QFTSETTINGS_H
   #define __20110913_QFTSETTINGS_H

#include <QDialog>
#include <QTranslator>
#include <QVector>

#include "defdef.h"

namespace Ui {
    class QFTSettings;
}

//---------------------------------------------------------------------------
//! \class   QFTSettings
//! \date    13.09.2011 / 10:00
//! \author  Jo2003
//! \brief   dialog for first time settings
//---------------------------------------------------------------------------
class QFTSettings : public QDialog
{
    Q_OBJECT

public:
   QFTSettings(QWidget *parent = 0);
    ~QFTSettings();

private slots:
   void on_cbxLanguage_currentIndexChanged(QString str);
   void on_pushReset_clicked();
   void on_pushSave_clicked();

protected:
   void saveFTSettings();
   virtual void changeEvent(QEvent *e);

private:
    Ui::QFTSettings *ui;
};

#endif // __20110913_QFTSETTINGS_H
