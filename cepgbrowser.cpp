/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 18.01.2010 / 16:07:29
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#include <QUrl>
#include <QUrlQuery>
#include "cepgbrowser.h"
#include "small_helpers.h"
#include "externals_inc.h"

/* -----------------------------------------------------------------\
|  Method: CEpgBrowser / constructor
|  Begin: 18.01.2010 / 16:07:59
|  Author: Jo2003
|  Description: create object, init values
|
|  Parameters: pointer to parent widget
|
|  Returns: --
\----------------------------------------------------------------- */
CEpgBrowser::CEpgBrowser(QWidget *parent) :
    QTextBrowser(parent)
{
   iCid = 0;
   _iTs = 0;
   mProgram.clear();
}

/* -----------------------------------------------------------------\
|  Method: DisplayEpg
|  Begin: 18.01.2010 / 16:08:35
|  Author: Jo2003
|  Description: display EPG entries
|
|  Parameters: list of entries, channel name, channel id,
|              timestamp, archiv flag, timeshift, external epg flag
|
|  Returns: --
\----------------------------------------------------------------- */
void CEpgBrowser::DisplayEpg(QVector<cparser::SEpg> epglist,
                             const QString &sName, int iChanID, uint uiGmt,
                             bool bHasArchiv, int iTs, bool bExt)
{
   epg::SShow actShow;

   // store values ...
   sChanName  = sName;
   iCid       = iChanID;
   uiTime     = uiGmt;
   bArchive   = bHasArchiv;
   _iTs       = iTs;
   bExtEPG    = bExt;

   // clear program map ...
   mProgram.clear();

   for (int i = 0; i < epglist.size(); i ++)
   {
      actShow.sShowName  = epglist[i].sName;
      actShow.sShowDescr = epglist[i].sDescr;
      actShow.uiStart    = epglist[i].uiGmt;

      if (epglist[i].uiEnd != 0)
      {
         actShow.uiEnd   = epglist[i].uiEnd;
      }
      else
      {
         actShow.uiEnd   = ((i + 1) < epglist.size()) ? epglist[i + 1].uiGmt : 0;
      }

      // store start time and show info ...
      mProgram.insert(epglist[i].uiGmt, actShow);
   }

   clear();
   setHtml(createHtmlCode());
   scrollTo();
}

/* -----------------------------------------------------------------\
|  Method: recreateEpg
|  Begin: 24.01.2011 / 14:55
|  Author: Jo2003
|  Description: re-create epg with values stored in class
|               (needed when changing timeshift)
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CEpgBrowser::recreateEpg()
{
   clear();
   setHtml(createHtmlCode());
   scrollTo();
}

/* -----------------------------------------------------------------\
|  Method: createHtmlCode
|  Begin: 24.01.2011 / 14:55
|  Author: Jo2003
|  Description: create epg html code
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
QString CEpgBrowser::createHtmlCode()
{
   QString     tab, row, page, sHeadLine, timeCell, img, progCell, buttons;
   const char* rowStyle;
   QDateTime   dtStartThis, dtStartNext;
   QUrl        url;
   QUrlQuery   urlq;
   bool        bMark;
   bool        bGray;
   epg::SShow  actShow;
   int         i, iAa, iFs = 0;
   QMap<uint, epg::SShow>::const_iterator cit;

   // reset current marker ...
   hasCurrent = false;

   // create compare times ...
   QDateTime dayStart(QDateTime::fromTime_t(uiTime).date());

   if (!bExtEPG)
   {
      // create headline (table head) ...
      sHeadLine = QString("%1 - %2")
                  .arg(sChanName)
                  .arg(QDateTime::fromTime_t(uiTime).toString("dd. MMM. yyyy"));

      // create table head ...
      row = pHtml->tableHead(sHeadLine, TMPL_TH_STYLE, 2);

      // wrap in row ...
      tab = pHtml->tableRow(row);
   }

   if (mProgram.isEmpty())
   {
      row  = pHtml->tableCell(tr("No information available!"), TMPL_A_STYLE, 2, "center");
      tab += pHtml->tableRow(row);
   }
   else
   {
      for (cit = mProgram.constBegin(), i = 0; cit != mProgram.constEnd(); cit++, i++)
      {
         timeCell    = "";
         buttons     = "";
         actShow     = *cit;
         bMark       = false;
         bGray       = false;
         dtStartThis = QDateTime::fromTime_t(actShow.uiStart + _iTs);

         // special handling for extended EPG ...
         if (bExtEPG)
         {
#ifdef __MY_EXT_EPG
            // don't show entries older then 22:00 last day ...
            if (dtStartThis < dayStart.addSecs(-EXT_EPG_TIME))
            {
               continue;
            }

            // all what is not today should be marked gray ...
            if ((dtStartThis < dayStart) || (dtStartThis > dayStart.addDays(1)))
            {
               bGray = true;
#else
            if ((dtStartThis < dayStart) || (dtStartThis > dayStart.addDays(1)))
            {
               continue;
#endif // __MY_EXT_EPG
            }
            else if (iFs == 0)
            {
               // mark first show for scrolling ...
               iFs = 1;
            }
         }

         // find out if we should mark the time ...
         if (actShow.uiEnd)
         {
            dtStartNext = QDateTime::fromTime_t(actShow.uiEnd + _iTs);
            bMark       = NowRunning(dtStartThis, dtStartNext);
         }
         else
         {
            bMark       = NowRunning(dtStartThis);
         }

         if (bMark)
         {
            // when creating current program cell it shouldn't be gray!
            bGray      = false;

            // add anchor ...
            timeCell   = pHtml->simpleTag("a", "name='nowPlaying'");

            // set current marker ...
            hasCurrent = true;
         }

         // add anchor for first show of day ...
         if (iFs == 1)
         {
            iFs        = -1;
            timeCell  += pHtml->simpleTag("a", "name='firstShowOfDay'");
         }

         // add time ...
         timeCell += pHtml->span(dtStartThis.toString("hh:mm"), bGray ? "color: #888;" : "color: black;") + "<hr>";

         // timer record stuff ...
         if (dtStartThis > tmSync.currentDateTimeSync())
         {
            // has not started so far ... add timer record link ...
            url.clear();
            urlq.clear();
            urlq.addQueryItem("action", "timerrec");
            urlq.addQueryItem("cid"  , QString::number(iCid));
            urlq.addQueryItem("start", QString::number(actShow.uiStart));
            urlq.addQueryItem("end"  , QString::number(actShow.uiEnd));
            url.setPath("vlc-record");
            url.setQuery(urlq);

            // rec button ...
            img = pHtml->image(":/png/timer", 16, 16, "", tr("add timer record ..."));

            // wrap in link ...
            buttons = pHtml->link(url.toEncoded(), img) + "&nbsp;";
         }

         // archive supported and still available ...
         if (bArchive && ((iAa = CSmallHelpers::archiveAvailable(actShow.uiStart, tmSync)) > -2))
         {
            // only show archiv links if this show is already available ...
            if (iAa == 1)
            {
               url.clear();
               urlq.clear();
               urlq.addQueryItem("action", "archivplay");
               urlq.addQueryItem("cid", QString::number(iCid));
               urlq.addQueryItem("gmt", QString::number(actShow.uiStart));
               url.setPath("vlc-record");
               url.setQuery(urlq);

               // play button ...
               img = pHtml->image(":/png/play", 16, 16, "", tr("play from archive ..."));

               // wrap in link ...
               buttons += pHtml->link(url.toEncoded(), img) + "&nbsp;";

               url.clear();
               urlq.clear();
               urlq.addQueryItem("action", "archivrec");
               urlq.addQueryItem("cid", QString::number(iCid));
               urlq.addQueryItem("gmt", QString::number(actShow.uiStart));
               url.setPath("vlc-record");
               url.setQuery(urlq);

               // rec button ...
               img = pHtml->image(":/png/record", 16, 16, "", tr("record from archive ..."));

               // wrap in link ...
               buttons += pHtml->link(url.toEncoded(), img) + "&nbsp;";
            }

            // mark for later view ...
            url.clear();
            urlq.clear();
            urlq.addQueryItem("action", "remember");
            urlq.addQueryItem("cid"  , QString::number(iCid));
            urlq.addQueryItem("gmt"  , QString::number(actShow.uiStart));
            url.setPath("vlc-record");
            url.setQuery(urlq);

            // remember button ...
            img = pHtml->image(":/png/remember", 16, 16, "", tr("add to watch list ..."));

            // wrap in link ...
            buttons += pHtml->link(url.toEncoded(), img);
         }

         timeCell += pHtml->htmlTag("div", buttons, "white-space: pre;");

         progCell = pHtml->span(actShow.sShowName, bGray ? "color: #888;" : "color: black;");

         if (actShow.sShowDescr != "")
         {
            progCell += "<br />" + pHtml->span(actShow.sShowDescr, bGray ? "color: #888;" : "color: #666;");
         }

         // check which row style to use ...
         if (bMark)
         {
            rowStyle = TMPL_CUR_STYLE;
         }
         else if (bGray)
         {
            rowStyle = (i % 2) ? TMPL_B_STYLE_GRAY : TMPL_A_STYLE_GRAY;
         }
         else
         {
            rowStyle = (i % 2) ? TMPL_B_STYLE : TMPL_A_STYLE;
         }

         row  = pHtml->tableCell(timeCell, rowStyle);
         row += pHtml->tableCell(progCell, rowStyle);

         // wrap in table row ...
         tab += pHtml->tableRow(row);
      }
   }

   // wrap in table ...
   tab = pHtml->table(tab, TMPL_TAB_STYLE);

   // wrap in page ...
   page = pHtml->htmlPage(tab, "EPG Table");

   return page;
}

//---------------------------------------------------------------------------
//
//! \brief   scroll either to current show or start of day
//
//! \author  Jo2003
//! \date    28.08.2014
//
//---------------------------------------------------------------------------
void CEpgBrowser::scrollTo()
{
   if (hasCurrent)
   {
      scrollToAnchor("nowPlaying");
   }
   else
   {
      scrollToAnchor("firstShowOfDay");
   }
}

/* -----------------------------------------------------------------\
|  Method: NowRunning
|  Begin: 18.01.2010 / 16:09:56
|  Author: Jo2003
|  Description: check if given show is now running
|
|  Parameters: this shows start time, next shows start time
|
|  Returns: true ==> running
|          false ==> not running
\----------------------------------------------------------------- */
bool CEpgBrowser::NowRunning (const QDateTime &startThis, const QDateTime &startNext)
{
   bool bNowRunning = false;

   // now given, later not given ...
   if (startThis.isValid() && !startNext.isValid())
   {
      int diff = tmSync.syncronizedTime_t() - startThis.toTime_t();

      // mark this show as running, if start wasn't more
      // than 3 hours ago ...
      if ((diff >= 0) && (diff < (3600 * 3))) // 3 hours
      {
         bNowRunning = true;
      }
   }
   // now and later given ...
   else if (startThis.isValid() && startNext.isValid())
   {
      if ((tmSync.currentDateTimeSync() >= startThis)
         && (tmSync.currentDateTimeSync() <= startNext))
      {
         bNowRunning = true;
      }
   }

   return bNowRunning;
}

/* -----------------------------------------------------------------\
|  Method: epgShow
|  Begin: 16.01.2010 / 10:52:12
|  Author: Jo2003
|  Description: get the program info
|
|  Parameters: timestamp for this show
|
|  Returns: show info
\----------------------------------------------------------------- */
const epg::SShow CEpgBrowser::epgShow(uint uiTimeT)
{
   return mProgram.value(uiTimeT);
}

/* -----------------------------------------------------------------\
|  Method: exportProgMap
|  Begin: 03.11.2011
|  Author: Jo2003
|  Description: export a copy of the program map
|
|  Parameters: --
|
|  Returns: program map
\----------------------------------------------------------------- */
QMap<uint, epg::SShow> CEpgBrowser::exportProgMap()
{
   return mProgram;
}

/* -----------------------------------------------------------------\
|  Method: EnlargeFont
|  Begin: 02.02.2010 / 16:52:12
|  Author: Jo2003
|  Description: enlarge font size by one
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CEpgBrowser::EnlargeFont()
{
   QFont epgFont = font();
   epgFont.setPointSize(epgFont.pointSize() + 1);
   setFont(epgFont);
   scrollTo();
}

/* -----------------------------------------------------------------\
|  Method: ReduceFont
|  Begin: 02.02.2010 / 16:52:12
|  Author: Jo2003
|  Description: reduce font size by one
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CEpgBrowser::ReduceFont()
{
   QFont epgFont = font();
   epgFont.setPointSize(epgFont.pointSize() - 1);
   setFont(epgFont);
   scrollTo();
}

/* -----------------------------------------------------------------\
|  Method: ChangeFontSize
|  Begin: 18.02.2010 / 12:52:12
|  Author: Jo2003
|  Description: change font size with value given as option
|
|  Parameters: change value
|
|  Returns: --
\----------------------------------------------------------------- */
void CEpgBrowser::ChangeFontSize(int iSz)
{
   QFont epgFont = font();
   epgFont.setPointSize(epgFont.pointSize() + iSz);
   setFont(epgFont);
   scrollTo();
}

/************************* History ***************************\
| $Log$
\*************************************************************/

