/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/qwatchlistdlg.cpp $
 *
 *  @file     qwatchlistdlg.cpp
 *
 *  @author   Jo2003
 *
 *  @date     06.08.2013
 *
 *  $Id: qwatchlistdlg.cpp 1238 2013-11-27 15:15:56Z Olenka.Joerg $
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
#include "chtmlwriter.h"

// storage db ...
extern CVlcRecDB *pDb;

// for logging ...
extern CLogFile VlcLog;

// global timeshift class ...
extern CTimeShift *pTs;

// global html writer ...
extern CHtmlWriter *pHtml;

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
   _bRecAll = false;
   _tGap.setSingleShot(true);
   connect(ui->txtWatchTab, SIGNAL(anchorClicked(QUrl)), this, SLOT(slotListAnchor(QUrl)));
   connect(&_tGap, SIGNAL(timeout()), this, SLOT(slotNextRec()));
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
   uint eol;
   QString page, row, tab, line, act, img, len;
   QUrl url;

   if (!_bRecAll)
   {
      _vUrls.clear();
   }

   // remove old entries ...
   cleanWatchList();

   if (!pDb->getWatchEntries(vE))
   {
      // build table head ...
      row  = pHtml->tableHead(tr("Start")  , TMPL_TH_STYLE);
      row += pHtml->tableHead(tr("Program"), TMPL_TH_STYLE);

      // wrap as row ...
      tab = pHtml->tableRow(row);

      for (i = 0; i < vE.count(); i++)
      {
         act  = "";
         row  = "";
         line = "";

         eol  = vE.at(i).uiEol ? (vE.at(i).uiEol - vE.at(i).uiStart) : MAX_ARCHIV_AGE;

         // handle old / very new entries ...
         if ((iRet = CSmallHelpers::archiveAvailable(vE.at(i).uiStart, eol)) == -2)
         {
            // old entries should be deleted already (cleanWatchList())
            // so this shouldn't happen ...
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
            url.addQueryItem("eol"   , QString::number(vE.at(i).uiEol));
            url.addQueryItem("chan"  , vE.at(i).sName);
            url.addQueryItem("show"  , vE.at(i).sProgramm);

            // play button ...
            img = pHtml->image(":/png/play", 16, 16, "", tr("play from archive ..."));

            // wrap in link ...
            act = pHtml->link(url.toEncoded(), img) + "&nbsp;";

            url.clear();
            url.setPath("vlc-record");
            url.addQueryItem("action", "wl_rec");
            url.addQueryItem("cid"   , QString::number(vE.at(i).iId));
            url.addQueryItem("start" , QString::number(vE.at(i).uiStart));
            url.addQueryItem("end"   , QString::number(vE.at(i).uiEnd));
            url.addQueryItem("eol"   , QString::number(vE.at(i).uiEol));
            url.addQueryItem("chan"  , vE.at(i).sName);
            url.addQueryItem("show"  , vE.at(i).sProgramm);

            // record button ...
            img  = pHtml->image(":/png/record", 16, 16, "", tr("record from archive ..."));

            // wrap in link ...
            act += pHtml->link(url.toEncoded(), img) + "&nbsp;";

            if (!_bRecAll)
            {
               // tell we should stop at end ...
               url.addQueryItem("stopatend", "1");

               // save url ...
               _vUrls.append(url);
            }
         }

         // add delete link ...
         url.clear();
         url.setPath("vlc-record");
         url.addQueryItem("action", "wl_del");
         url.addQueryItem("cid"   , QString::number(vE.at(i).iId));
         url.addQueryItem("gmt"   , QString::number(vE.at(i).uiStart));

         // delete button ...
         img  = pHtml->image(":/png/remove", 16, 16, "", tr("delete from list ..."));

         // wrap in link ...
         act += pHtml->link(url.toEncoded(), img);

         // channel in red bold ...
         line  = pHtml->htmlTag("b", vE.at(i).sName, "font-size: medium; color: red;") + "<br />";

         // do we have a description ?
         sl    = vE.at(i).sProgramm.split("\n");

         line += pHtml->htmlTag("b", sl.at(0));

         // add description ...
         if (sl.count() > 1)
         {
            line += "<br />" + pHtml->span(sl.at(1), "color: #666");
         }

         // there might be no end time ...
         len  = vE.at(i).uiEnd ? QString(" (%1)").arg(tr("%1 min.").arg((vE.at(i).uiEnd - vE.at(i).uiStart) / 60)) : "";

         row  = pTs->fromGmtFormatted(vE.at(i).uiStart, "dd. MMM. yyyy, hh:mm") + len + "<br /> <br />" + act;
         row  = pHtml->tableCell(row , (i % 2) ? TMPL_A_STYLE : TMPL_B_STYLE);
         row += pHtml->tableCell(line, (i % 2) ? TMPL_A_STYLE : TMPL_B_STYLE);

         // wrap as row ...
         tab += pHtml->tableRow(row);
      }

      // wrap tab ...
      tab = pHtml->table(tab, TMPL_TAB_STYLE);
   }

   // wrap in page ...
   page = pHtml->htmlPage(tab, "Watch List");

   ui->txtWatchTab->setHtml(page);

   if (!vE.isEmpty() && !_bRecAll)
   {
      ui->pushRecordAll->setEnabled(true);
   }
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

      emit sigUpdCount();
   }
   else
   {
      // send url ...
      emit sigClick(url);

      // close dialog ...
      accept();
   }
}

//---------------------------------------------------------------------------
//
//! \brief   get entry count
//
//! \author  Jo2003
//! \date    21.08.2013
//
//! \param   --
//
//! \return  number of entries in watch list
//---------------------------------------------------------------------------
int QWatchListDlg::count()
{
   QSqlQuery q;
   int       iRet = 0;

   // make sure we don't count old entries ...
   cleanWatchList();

   if(!pDb->ask("SELECT COUNT(*) AS numb FROM watchlist", q))
   {
      if (q.first())
      {
         iRet = q.value(0).toInt();
      }
   }

   return iRet;
}

//---------------------------------------------------------------------------
//
//! \brief   delete old entries from watch list
//
//! \author  Jo2003
//! \date    26.08.2013
//
//! \param   --
//
//! \return  0 -> ok; else -> error
//---------------------------------------------------------------------------
int QWatchListDlg::cleanWatchList()
{
   uint      now       = QDateTime::currentDateTime().toTime_t();
   uint      uiArchLow = now - MAX_ARCHIV_AGE;   // no older than 2 weeks
   QSqlQuery q;

   q.prepare("DELETE FROM watchlist WHERE t_start < ?");
   q.addBindValue(uiArchLow);

   return pDb->ask(q);
}

//---------------------------------------------------------------------------
//
//! \brief   record all entries
//
//! \author  Jo2003
//! \date    27.11.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QWatchListDlg::on_pushRecordAll_clicked()
{
   _bRecAll = true;
   ui->pushRecordAll->setDisabled(true);

   slotNextRec();

   // close dialog ...
   accept();
}

//---------------------------------------------------------------------------
//
//! \brief   one watch list record ended
//
//! \author  Jo2003
//! \date    27.11.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QWatchListDlg::slotWLRecEnded()
{
   _tGap.start(5000);
}

//---------------------------------------------------------------------------
//
//! \brief   start next watch list record
//
//! \author  Jo2003
//! \date    27.11.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QWatchListDlg::slotNextRec()
{
   if (_bRecAll)
   {
      if (!_vUrls.isEmpty())
      {
         emit sigClick(_vUrls.at(0));
         _vUrls.remove(0);
      }
      else
      {
         _bRecAll = false;
         ui->pushRecordAll->setEnabled(true);
      }
   }
}

//---------------------------------------------------------------------------
//
//! \brief   stop record all
//
//! \author  Jo2003
//! \date    27.11.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QWatchListDlg::slotStopRecAll()
{
   _bRecAll = false;
   _vUrls.clear();
   ui->pushRecordAll->setEnabled(true);
}
