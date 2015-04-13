/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/sunduk.tv/qhelpdialog.cpp $
 *
 *  @file     qhelpdialog.cpp
 *
 *  @author   Jo2003
 *
 *  @date     06.07.2012
 *
 *  $Id: qhelpdialog.cpp 847 2012-07-26 12:46:06Z Olenka.Joerg $
 *
 *///------------------------- (c) 2012 by Jo2003  --------------------------

#include "qhelpdialog.h"
#include "ui_qhelpdialog.h"

// for logging ...
// extern CLogFile VlcLog;

//---------------------------------------------------------------------------
//
//! \brief   constructs QHelpDialog object
//
//! \author  Jo2003
//! \date    06.07.2012
//
//! \param   parent pointer to parent widget
//! \param   helpFile string of help file name
//
//---------------------------------------------------------------------------
QHelpDialog::QHelpDialog (QWidget *parent, const QString &helpFile)
   : QDialog(parent, Qt::Window),
     ui(new Ui::QHelpDialog), pHe(NULL)
{
   ui->setupUi(this);

   if (helpFile != "")
   {
      setHelpFile(helpFile);
   }
}

//---------------------------------------------------------------------------
//
//! \brief   destroys QHelpDialog object
//
//! \author  Jo2003
//! \date    06.07.2012
//
//---------------------------------------------------------------------------
QHelpDialog::~QHelpDialog()
{
   delete ui;
}

//---------------------------------------------------------------------------
//
//! \brief   dialog should be shown
//
//! \author  Jo2003
//! \date    09.07.2012
//
//! \param   event pointer to show event
//
//! \return  --
//---------------------------------------------------------------------------
void QHelpDialog::showEvent(QShowEvent *event)
{
   Q_UNUSED(event)
   QTimer::singleShot(10, this, SLOT(adjustSplitter()));
}

//---------------------------------------------------------------------------
//
//! \brief   adjust splitter sizes
//
//! \author  Jo2003
//! \date    09.07.2012
//
//! \return  --
//---------------------------------------------------------------------------
void QHelpDialog::adjustSplitter()
{
   int         i, j;
   QList<int> sz = ui->splitter->sizes();

   // adjust splitter to have good dimension ...
   // 1:3
   i = sz.value(0) + sz.value(1);
   j = (int)(i / 4);
   sz.clear();
   sz.insert(0, j);
   sz.insert(1, i - j);
   ui->splitter->setSizes(sz);
}

//---------------------------------------------------------------------------
//
//! \brief   set help file / display help
//
//! \author  Jo2003
//! \date    06.07.2012
//
//! \param   helpFile string of help file name
//! \param   parent pointer to parent widget
//
//! \return  --
//---------------------------------------------------------------------------
void QHelpDialog::setHelpFile(const QString &helpFile)
{
   // check if new file exists ...
   if (QFile::exists(helpFile))
   {
      if (pHe && (sFile != helpFile))
      {
         ui->helpBrowser->setHelpEngine(NULL);
         ui->splitter->widget(0)->deleteLater();

         delete pHe;
         pHe = NULL;
      }

      if (!pHe)
      {
         pHe = new QHelpEngine (helpFile, this);
         pHe->setupData();

         ui->helpBrowser->setHelpEngine(pHe);
         ui->splitter->insertWidget(0, pHe->contentWidget());

         connect(pHe->contentWidget(), SIGNAL(clicked(const QModelIndex &)),
                 this, SLOT(slotContentClick(const QModelIndex &)));

         // display whole document (first link) ...
         QStringList sl = pHe->registeredDocumentations();

         if (sl.count() > 0)
         {
            QFileInfo fi(helpFile);
            QString   sUrl = QString("qthelp://%1/doc/%2.html").arg(sl.at(0)).arg(fi.baseName());
            ui->helpBrowser->setSource(QUrl(sUrl));
         }
      }

      sFile = helpFile;
   }
}

//---------------------------------------------------------------------------
//
//! \brief   content in content widget was clicked (single click) [slot]
//
//! \author  Jo2003
//! \date    10.07.2012
//
//! \param   idx clicked index
//
//! \return  --
//---------------------------------------------------------------------------
void QHelpDialog::slotContentClick(const QModelIndex &idx)
{
   if (pHe)
   {
      ui->helpBrowser->setSource(pHe->contentModel()->contentItemAt(idx)->url());
   }
}

///////////////////////////////////////////////////////////////////////////////
// QHelpBrowser
///////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//
//! \brief   constructs QHelpBrowser object
//
//! \author  Jo2003
//! \date    09.07.2012
//
//! \param   parent pointer to parent widget
//! \param   helpEngine pointer to QHelpEngine instance
//
//---------------------------------------------------------------------------
QHelpBrowser::QHelpBrowser(QWidget *parent, QHelpEngine *helpEngine)
   : QTextBrowser(parent), pHe(helpEngine)
{
   // decide which link where to open ...
   connect(this, SIGNAL(anchorClicked(const QUrl &)), this, SLOT(slotHelpAnchor(const QUrl &)));
}

//---------------------------------------------------------------------------
//
//! \brief   destroys QHelpBrowser object
//
//! \author  Jo2003
//! \date    09.07.2012
//
//---------------------------------------------------------------------------
QHelpBrowser::~QHelpBrowser()
{
   // nothing to do so far ...
}

//---------------------------------------------------------------------------
//
//! \brief   load resource either from QtHelp file or from resource system
//
//! \author  Jo2003
//! \date    09.07.2012
//
//! \param   type type of resource
//! \param   url file or url in QtHelp
//
//! \return  resource as QVariant (usually QByteArray)
//---------------------------------------------------------------------------
QVariant QHelpBrowser::loadResource(int type, const QUrl &url)
{
   if (pHe)
   {
      if (url.scheme() == "qthelp")
      {
         return QVariant(pHe->fileData(url));
      }
      else
      {
         return QTextBrowser::loadResource(type, url);
      }
   }

   return QVariant();
}

//---------------------------------------------------------------------------
//
//! \brief   set helpengine pointer
//
//! \author  Jo2003
//! \date    09.07.2012
//
//! \param   helpEngine Pointer to QHelpEngine
//
//! \return  --
//---------------------------------------------------------------------------
void QHelpBrowser::setHelpEngine(QHelpEngine *helpEngine)
{
   pHe = helpEngine;
}

//---------------------------------------------------------------------------
//
//! \brief   anchor in help text was clicked
//
//! \author  Jo2003
//! \date    10.07.2012
//
//! \param   link clicked url
//
//! \return  --
//---------------------------------------------------------------------------
void QHelpBrowser::slotHelpAnchor(const QUrl &link)
{
   // backup old position ...
   QUrl src = source();
   int  pos = verticalScrollBar()->value();

   // open external Url in browser ...
   if ((link.scheme() == "http") || (link.scheme() == "https"))
   {
      QDesktopServices::openUrl(link);

      // restore old position ...
      setSource(src);
      verticalScrollBar()->setValue(pos);
   }
}
