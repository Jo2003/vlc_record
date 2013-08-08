/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qwatchlistdlg.cpp
 *
 *  @author   Jo2003
 *
 *  @date     06.08.2013
 *
 *  $Id$
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#include <QDateTime>
#include <QStringList>
#include "qwatchlistdlg.h"
#include "ui_qwatchlistdlg.h"
#include "templates.h"
#include "small_helpers.h"
#include "clogfile.h"
#include "ctimeshift.h"

// storage db ...
extern CVlcRecDB *pDb;

// for logging ...
extern CLogFile VlcLog;

// global timeshift class ...
extern CTimeShift *pTs;

//---------------------------------------------------------------------------
//
//! \brief   constructs QWatchListDlg dialog
//
//! \author  Jo2003
//! \date    06.08.2013
//
//! \param   parent (QWidget *) pointer to parent widget
//
//! \return  --
//---------------------------------------------------------------------------
QWatchListDlg::QWatchListDlg(QWidget *parent) :
   QDialog(parent),
   ui(new Ui::QWatchListDlg)
{
   ui->setupUi(this);
   connect(ui->txtWatchTab, SIGNAL(anchorClicked(QUrl)), this, SLOT(slotListAnchor(QUrl)));
}

//---------------------------------------------------------------------------
//
//! \brief   destroys QWatchListDlg dialog object
//
//! \author  Jo2003
//! \date    06.08.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
QWatchListDlg::~QWatchListDlg()
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
void QWatchListDlg::changeEvent(QEvent *e)
{
   QDialog::changeEvent(e);

   switch (e->type())
   {
   case QEvent::LanguageChange:
      {
         ui->retranslateUi(this);
      }
      break;

   default:
       break;
   }
}

//---------------------------------------------------------------------------
//
//! \brief   create watch list table
//
//! \author  Jo2003
//! \date    07.08.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QWatchListDlg::buildWatchTab()
{
   int iRet;
   QVector<cparser::SChan> vE;
   QStringList sl;
   int i;
   QString content, tab, line, act;
   QUrl url;
   QString html = HTML_SITE;
   html.replace(TMPL_CSS  , TMPL_CSS_WRAPPER);
   html.replace(TMPL_CSS  , "body {background-color: rgb(255, 254, 212);}\n");
   html.replace(TMPL_TITLE, tr("Watch List"));

   if (!pDb->getWatchEntries(vE))
   {
      content += EPG_TMPL;

      tab  = "  <tr>\n";
      tab += TD_HEAD;
      tab.replace(TMPL_HEAD, tr("Start"));
      tab += TD_HEAD;
      tab.replace(TMPL_HEAD, tr("Length"));
      tab += TD_HEAD;
      tab.replace(TMPL_HEAD, tr("Channel"));
      tab += TD_HEAD;
      tab.replace(TMPL_HEAD, tr("Show"));
      tab += TD_HEAD;
      tab.replace(TMPL_HEAD, tr("Actions"));
      tab += "  </tr>\n";

      for (i = 0; i < vE.count(); i++)
      {
         act  = "";
         line = "";

         // handle old / very new entries ...
         if ((iRet = CSmallHelpers::archiveAvailable(vE.at(i).uiStart)) == -2)
         {
            // show to old and therefore no more part of archive -> remove db entry ...
            pDb->delWatchEntry(vE.at(i).iId, vE.at(i).uiStart);

            // and continue ...
            continue;
         }
         else if (iRet == 1)
         {
            // archive for this show available ...

            // build action links ...
            url.clear();
            url.setPath("vlc-record");
            url.addQueryItem("action", "wl_play");
            url.addQueryItem("cid"   , QString::number(vE.at(i).iId));
            url.addQueryItem("start" , QString::number(vE.at(i).uiStart));
            url.addQueryItem("end"   , QString::number(vE.at(i).uiEnd));
            url.addQueryItem("chan"  , vE.at(i).sName);
            url.addQueryItem("show"  , vE.at(i).sProgramm);

            // play ...
            act += QString("<a href='%2'><img src=':/png/play' width='16' height='16' alt='play' title='%1' /></a>&nbsp;")
                           .arg(tr("play from archive ..."))
                           .arg(QString(url.toEncoded()));

            url.clear();
            url.setPath("vlc-record");
            url.addQueryItem("action", "wl_rec");
            url.addQueryItem("cid"   , QString::number(vE.at(i).iId));
            url.addQueryItem("start" , QString::number(vE.at(i).uiStart));
            url.addQueryItem("end"   , QString::number(vE.at(i).uiEnd));
            url.addQueryItem("chan"  , vE.at(i).sName);
            url.addQueryItem("show"  , vE.at(i).sProgramm);

            // record ...
            act += QString("<a href='%2'><img src=':/png/record' width='16' height='16' alt='record' title='%1' /></a>&nbsp;")
                           .arg(tr("record from archive ..."))
                           .arg(QString(url.toEncoded()));
         }

         // add delete link ...
         url.clear();
         url.setPath("vlc-record");
         url.addQueryItem("action", "wl_del");
         url.addQueryItem("cid"   , QString::number(vE.at(i).iId));
         url.addQueryItem("gmt"   , QString::number(vE.at(i).uiStart));

         // delete ...
         act += QString("<a href='%2'><img src=':/png/remove' width='16' height='16' alt='record' title='%1' /></a>")
                        .arg(tr("delete from list ..."))
                        .arg(QString(url.toEncoded()));

         // do we have a description ?
         sl   = vE.at(i).sProgramm.split("\n");
         line = sl.at(0);

         // add description ...
         if (sl.count() > 1)
         {
            line += QString("<br /><span style='color: #666'>%1</span>").arg(sl.at(1));
         }

         tab += "  <tr>\n";
         tab += (i % 2) ? TD_TMPL_A : TD_TMPL_B;
         tab.replace(TMPL_CONT, pTs->fromGmtFormatted(vE.at(i).uiStart, "dd.MM.yyyy<br />hh:mm"));
         tab += (i % 2) ? TD_TMPL_A : TD_TMPL_B;
         tab.replace(TMPL_CONT, tr("%1 min.").arg((vE.at(i).uiEnd - vE.at(i).uiStart) / 60));
         tab += (i % 2) ? TD_TMPL_A : TD_TMPL_B;
         tab.replace(TMPL_CONT, QString("<b>%1</b>").arg(vE.at(i).sName));
         tab += (i % 2) ? TD_TMPL_A : TD_TMPL_B;
         tab.replace(TMPL_CONT, line);
         tab += (i % 2) ? TD_TMPL_A : TD_TMPL_B;
         tab.replace(TMPL_CONT, act);
         tab += "  </tr>\n";
      }

      content.replace(TMPL_ROWS, tab);
   }

   html.replace(TMPL_CONT, content);

   ui->txtWatchTab->setHtml(html);
}

//---------------------------------------------------------------------------
//
//! \brief   handle clicked link
//
//! \author  Jo2003
//! \date    07.08.2013
//
//! \param   url (QUrl) url of clicked link
//
//! \return  --
//---------------------------------------------------------------------------
void QWatchListDlg::slotListAnchor(QUrl url)
{
   // delete watch list entry ...
   if (url.queryItemValue("action") == "wl_del")
   {
      int cid = url.queryItemValue("cid").toInt();
      int gmt = url.queryItemValue("gmt").toUInt();

      pDb->delWatchEntry(cid, gmt);

      buildWatchTab();
   }
   else
   {
      // send url ...
      emit sigClick(url);

      // close dialog ...
      accept();
   }
}

