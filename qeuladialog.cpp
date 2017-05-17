/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qeuladlg.cpp
 *
 *  @author   Jo2003
 *
 *  @date     16.05.2017
 *
 *  $Id$
 *
 *///------------------------- (c) 2017 by Jo2003  --------------------------
#include "qeuladialog.h"
#include "ui_qeuladialog.h"

#include "externals_inc.h"

//---------------------------------------------------------------------------
//! @brief      create dialog
//! @param[in]  parent pointer to parent widget
//---------------------------------------------------------------------------
QEulaDialog::QEulaDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QEulaDialog)
{
    ui->setupUi(this);
    pBtnOk = ui->buttonBox->button(QDialogButtonBox::Ok);

    if (pBtnOk != NULL)
    {
        pBtnOk->setDisabled(true);
    }

    // fill language box ...
    QDir        folder(pFolders->getLangDir());
    QStringList sl = folder.entryList(QStringList("eula_*.txt"), QDir::Files, QDir::Name);

    QRegExp rx("^eula_([a-zA-Z]+).txt$");
    for (int i = 0; i < sl.size(); i++)
    {
       // get out language from file name ...
       if (sl.at(i).indexOf(rx) > -1)
       {
          ui->cbxLanguage->addItem(QIcon(QString(":/flags/%1").arg(rx.cap(1))), rx.cap(1));
       }
    }

    setWindowTitle(tr("EULA"));

    on_cbxLanguage_currentIndexChanged(ui->cbxLanguage->currentText());
}

//---------------------------------------------------------------------------
//! @brief      destroy dialog
//---------------------------------------------------------------------------
QEulaDialog::~QEulaDialog()
{
    delete ui;
}

//---------------------------------------------------------------------------
//! @brief      accept checkbox was clicked
//! @param[in]  checked checked if true
//---------------------------------------------------------------------------
void QEulaDialog::on_checkBox_clicked(bool checked)
{
    if (checked && pBtnOk)
    {
        pBtnOk->setEnabled(true);
    }
    else if (!checked && pBtnOk)
    {
        pBtnOk->setEnabled(false);
    }
}

//---------------------------------------------------------------------------
//! \brief   catch incoming event
//! \param   e pointer to incoming event
//---------------------------------------------------------------------------
void QEulaDialog::changeEvent(QEvent *e)
{
   QDialog::changeEvent(e);

   switch (e->type())
   {
   case QEvent::LanguageChange:
      {
         // save current index from comboboxes ...
         int iLanIdx = ui->cbxLanguage->currentIndex();

         ui->retranslateUi(this);

         // re-set index to comboboxes ...
         ui->cbxLanguage->setCurrentIndex(iLanIdx);

         setWindowTitle(tr("EULA"));
      }
      break;

   default:
       break;
   }
}

//---------------------------------------------------------------------------
//! \brief   language changed
//! \param   str text label for new language
//---------------------------------------------------------------------------
void QEulaDialog::on_cbxLanguage_currentIndexChanged(QString str)
{
    // set language as read ...
    pAppTransl->load(QString("lang_%1").arg(str), pFolders->getLangDir());
    pQtTransl->load(QString("qt_%1").arg(str), pFolders->getQtLangDir());

    QFile fEula(QString("%1/eula_%2.txt").arg(pFolders->getLangDir()).arg(str));

    if (fEula.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString sEula = QString::fromUtf8(fEula.readAll().constData());
        ui->eulaBrowser->setPlainText(sEula);
    }
}
