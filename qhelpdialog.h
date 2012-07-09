/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qhelpdialog.h
 *
 *  @author   Jo2003
 *
 *  @date     06.07.2012
 *
 *  $Id$
 *
 *///------------------------- (c) 2012 by Jo2003  --------------------------
#ifndef __20120706_QHELPDIALOG_H
   #define __20120706_QHELPDIALOG_H

#include <QDialog>
#include <QString>
#include <QSplitter>
#include <QtHelp>
#include <QTextBrowser>
#include <QTimer>
#include <QShowEvent>

// -----------------------------------------------
// namespace ...
// -----------------------------------------------
namespace Ui {
   class QHelpDialog;
}

// forward declaration ...
class QHelpBrowser;

//---------------------------------------------------------------------------
//! \class   QHelpDialog
//! \date    06.07.2012
//! \author  Jo2003
//! \brief   simple dialog to show a Qt help file
//---------------------------------------------------------------------------
class QHelpDialog : public QDialog
{
   Q_OBJECT

public:
   QHelpDialog(QWidget *parent = 0, const QString &helpFile = QString());
   ~QHelpDialog();
   void setHelpFile (const QString &helpFile);

protected:
   virtual void showEvent(QShowEvent * event);

private slots:
   void adjustSplitter();

private:
   Ui::QHelpDialog *ui;
   QHelpEngine     *pHe;
   QString          sFile;
};

//---------------------------------------------------------------------------
//! \class   QHelpBrowser
//! \date    09.07.2012
//! \author  Jo2003
//! \brief   a textbrowser subclassed to display QtHelp
//---------------------------------------------------------------------------
class QHelpBrowser : public QTextBrowser
{
   Q_OBJECT

public:
   QHelpBrowser(QWidget * parent = NULL, QHelpEngine *helpEngine = NULL);
   virtual ~QHelpBrowser();
   void     setHelpEngine (QHelpEngine *helpEngine);
   virtual QVariant loadResource(int type, const QUrl &url);

private:
   QHelpEngine *pHe;
};

#endif // __20120706_QHELPDIALOG_H
