/*------------------------------ Infor mation ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/qftsettings.cpp $
 *
 *  @file     qftsettings.cpp
 *
 *  @author   Jo2003
 *
 *  @date     13.09.2011
 *
 *  $Id: qftsettings.cpp 1148 2013-07-24 14:27:14Z Olenka.Joerg $
 *
 *///------------------------- (c) 2011 by Jo2003  --------------------------
#include "qftsettings.h"
#include "ui_qftsettings.h"
#include "qcustparser.h"
#include <QRegExp>

// global customization class ...
extern QCustParser *pCustomization;

// for folders ...
extern CDirStuff *pFolders;

// storage db ...
extern CVlcRecDB *pDb;

// global translaters ...
extern QTranslator *pAppTransl;
extern QTranslator *pQtTransl;

//---------------------------------------------------------------------------
//
//! \brief   constructs QFTSettings object
//
//! \author  Jo2003
//! \date    13.09.2011 / 10:00
//
//! \param   parent pointer to parent widget
//
//! \return  --
//---------------------------------------------------------------------------
QFTSettings::QFTSettings(QWidget *parent) :
   QDialog(parent),
   ui(new Ui::QFTSettings)
{
    ui->setupUi(this);

    // set company name ...
    QString s = ui->groupAccount->title();
    ui->groupAccount->setTitle(s.arg(pCustomization->strVal("COMPANY_NAME")));

    s = windowTitle();
    setWindowTitle(s.arg(pCustomization->strVal("COMPANY_NAME")));

    // fill language box ...
    QDir        folder(pFolders->getLangDir());
    QStringList sl = folder.entryList(QStringList("lang_*.qm"), QDir::Files, QDir::Name);

    // make sure english is part of list ...
    sl.push_front("lang_en.qm");

    QRegExp rx("^lang_([a-zA-Z]+).qm$");
    for (int i = 0; i < sl.size(); i++)
    {
       // get out language from file name ...
       if (sl.at(i).indexOf(rx) > -1)
       {
          ui->cbxLanguage->addItem(QIcon(QString(":/flags/%1").arg(rx.cap(1))), rx.cap(1));
       }
    }

    resize(sizeHint());
}

//---------------------------------------------------------------------------
//
//! \brief   destroys QFTSettings object
//
//! \author  Jo2003
//! \date    13.09.2011 / 10:00
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
QFTSettings::~QFTSettings()
{
    delete ui;
}

//---------------------------------------------------------------------------
//
//! \brief   catch incoming event
//
//! \author  Jo2003
//! \date    13.09.2011 / 11:00
//
//! \param   e pointer to incoming event
//
//! \return  --
//---------------------------------------------------------------------------
void QFTSettings::changeEvent(QEvent *e)
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

         // set company name ...
         QString s = ui->groupAccount->title();
         ui->groupAccount->setTitle(s.arg(pCustomization->strVal("COMPANY_NAME")));

         s = windowTitle();
         setWindowTitle(s.arg(pCustomization->strVal("COMPANY_NAME")));
      }
      break;

   default:
       break;
   }
}

//---------------------------------------------------------------------------
//
//! \brief   language changed
//
//! \author  Jo2003
//! \date    13.09.2011 / 10:05
//
//! \param   str text label for new language
//
//! \return  --
//---------------------------------------------------------------------------
void QFTSettings::on_cbxLanguage_currentIndexChanged(QString str)
{
   // set language as read ...
   pAppTransl->load(QString("lang_%1").arg(str), pFolders->getLangDir());
   pQtTransl->load(QString("qt_%1").arg(str), pFolders->getQtLangDir());
}

//---------------------------------------------------------------------------
//
//! \brief   save first time settings
//
//! \author  Jo2003
//! \date    13.09.2011 / 11:40
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QFTSettings::saveFTSettings()
{
   // use settings from form ...
   pDb->setValue("User",             ui->lineUsr->text());
   pDb->setValue("AllowAdult",  (int)ui->checkAdult->checkState());
   pDb->setValue("Language",         ui->cbxLanguage->currentText());
   pDb->setPassword("PasswdEnc",     ui->linePass->text());
   pDb->setPassword("ErosPasswdEnc", ui->lineErosPass->text());

   // set commonly used settings ...
#ifdef Q_OS_MAC
   pDb->setValue("TargetDir",    tr("%1/Movies").arg(QDir::homePath()));
#else
   pDb->setValue("TargetDir",    tr("%1/Videos").arg(QDir::homePath()));
#endif
   pDb->setValue("FixTime",      (int)Qt::Checked);  // fix time
   pDb->setValue("PlayerModule", "5_libvlc.mod");    // default player module
#ifdef _IS_OEM
   pDb->setValue("HttpCache",    3000);              // 3 sec. cache
#else
   pDb->setValue("HttpCache",    8000);              // 8 sec. cache
#endif // _IS_OEM
   pDb->setValue("UpdateCheck",  (int)Qt::Checked);  // check for updates
   pDb->setValue("2ClickPlay",   (int)Qt::Checked);  // double click on channel list starts player
   pDb->setValue("LogLevel",     3);                 // log everything
   pDb->setValue("NoSplash",     1);                 // no splash screen at startup ...

   // set windows size and position ...
   pDb->setValue("WndGeometry",    "01:d9:d0:cb:00:01:00:00:00:00:01:71:00:00:00:9d:00:00:06:0e:00:00:03:98:00:00:01:79:00:00:00:bb:00:00:06:06:00:00:03:90:00:00:00:01:00:00");
   pDb->setValue("spVChanEpg",     "270;394;");
   pDb->setValue("spVChanEpgPlay", "693;500;");
   pDb->setValue("spHPlay",        "450;185;");

#ifdef Q_OS_WIN32
   pDb->setValue("ShutdwnCmd", "shutdown.exe -s -f -t 5");
#endif
}

//---------------------------------------------------------------------------
//
//! \brief   button "reset" was pressed
//
//! \author  Jo2003
//! \date    13.09.2011 / 12:50
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QFTSettings::on_pushReset_clicked()
{
   ui->checkAdult->setCheckState(Qt::Unchecked);
   ui->lineErosPass->setText("");
}

//---------------------------------------------------------------------------
//
//! \brief   button "save" was pressed
//
//! \author  Jo2003
//! \date    13.09.2011 / 12:50
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QFTSettings::on_pushSave_clicked()
{
   // save content ...
   saveFTSettings();
   accept();
}
