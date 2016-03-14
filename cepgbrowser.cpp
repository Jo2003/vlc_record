/*********************** Information *************************\
| $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/cepgbrowser.cpp $
|
| Author: Jo2003
|
| Begin: 18.01.2010 / 16:07:29
|
| Last edited by: $Author: Olenka.Joerg $
|
| $Id: cepgbrowser.cpp 1213 2013-10-11 12:29:36Z Olenka.Joerg $
\*************************************************************/
#include <QUrl>
#include "cepgbrowser.h"
#include "small_helpers.h"
#include "chtmlwriter.h"

// log file functions ...
extern CLogFile VlcLog;

// global html writer ...
extern CHtmlWriter *pHtml;

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
|              timestamp, archiv flag, timeshift
|
|  Returns: --
\----------------------------------------------------------------- */
void CEpgBrowser::DisplayEpg(QVector<cparser::SEpg> epglist,
                             const QString &sName, int iChanID, uint uiGmt,
                             bool bHasArchiv, int iTs, int iLen)
{
   epg::SShow actShow;

   // store values ...
   sChanName = sName;
   iCid      = iChanID;
   uiTime    = uiGmt;
   bArchive  = bHasArchiv;
   _iTs      = iTs;
   iArLen    = iLen;

   // clear program map ...
   mProgram.clear();

   for (int i = 0; i < epglist.size(); i ++)
   {
      actShow.sShowName  = epglist[i].sName;
      actShow.sShowDescr = epglist[i].sDescr;
      actShow.uiStart    = epglist[i].uiGmt;
      actShow.id         = epglist[i].id;

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
   scrollToAnchor("nowPlaying");
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
   scrollToAnchor("nowPlaying");
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
   QString    tab, row, page, sHeadLine, timeCell, img, progCell;
   QDateTime  dtStartThis, dtStartNext;
   QUrl       url;
   bool       bMark;
   epg::SShow actShow;
   int        i, iAa;
   QMap<uint, epg::SShow>::const_iterator cit;


   // create headline (table head) ...
   sHeadLine = QString("%1 - %2")
               .arg(sChanName)
               .arg(QDateTime::fromTime_t(uiTime).toString("dd. MMM. yyyy"));

   // mInfo(tr("Creating EPG html for \"%1\" with timeshift %2s!").arg(sChanName).arg(_iTs));

   // create table head ...
   row = pHtml->tableHead(sHeadLine, TMPL_TH_STYLE, 2);

   // wrap in row ...
   tab = pHtml->tableRow(row);

   for (cit = mProgram.constBegin(), i = 0; cit != mProgram.constEnd(); cit++, i++)
   {
      actShow     = *cit;
      bMark       = false;
      dtStartThis = QDateTime::fromTime_t(actShow.uiStart + _iTs);

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

      timeCell  = bMark ? pHtml->simpleTag("a", "name='nowPlaying'") : "";
      timeCell += dtStartThis.toString("hh:mm") + "&nbsp;";

      // timer record stuff ...
      if (dtStartThis > QDateTime::currentDateTime())
      {
         // has not started so far ... add timer record link ...
         url.clear();
         url.setPath("vlc-record");
         url.addQueryItem("action", "timerrec");
         url.addQueryItem("cid"  , QString::number(iCid));
         url.addQueryItem("start", QString::number(actShow.uiStart));
         url.addQueryItem("end"  , QString::number(actShow.uiEnd));

         // rec button ...
         img = pHtml->image(":/png/timer", 16, 16, "", tr("add timer record ..."));

         // wrap in link ...
         timeCell += pHtml->link(url.toEncoded(), img);
      }

      // archive supported and still available ...
#ifdef _TASTE_STALKER
      if (bArchive && ((iAa = CSmallHelpers::archiveAvailable(actShow.uiStart, iArLen * 3600)) > -2))
#else
      if (bArchive && ((iAa = CSmallHelpers::archiveAvailable(actShow.uiStart)) > -2))
#endif // _TASTE_STALKER
      {
         timeCell += "<hr />" + pHtml->htmlTag("b", tr("Ar.")) + "&nbsp;";

         // only show archiv links if this show is already available ...
         if (iAa == 1)
         {
            url.clear();
            url.setPath("vlc-record");
            url.addQueryItem("action", "archivplay");
            url.addQueryItem("cid", QString::number(iCid));
            url.addQueryItem("gmt", QString::number(actShow.uiStart));

            if (actShow.id > 0)
            {
                url.addQueryItem("epg_id", QString::number(actShow.id));
            }

            // play button ...
            img = pHtml->image(":/png/play", 16, 16, "", tr("play from archive ..."));

            // wrap in link ...
            timeCell += pHtml->link(url.toEncoded(), img) + "&nbsp;";

            url.clear();
            url.setPath("vlc-record");
            url.addQueryItem("action", "archivrec");
            url.addQueryItem("cid", QString::number(iCid));
            url.addQueryItem("gmt", QString::number(actShow.uiStart));

            if (actShow.id > 0)
            {
                url.addQueryItem("epg_id", QString::number(actShow.id));
            }

            // rec button ...
            img = pHtml->image(":/png/record", 16, 16, "", tr("record from archive ..."));

            // wrap in link ...
            timeCell += pHtml->link(url.toEncoded(), img) + "&nbsp;";
         }

         // mark for later view ...
         url.clear();
         url.setPath("vlc-record");
         url.addQueryItem("action", "remember");
         url.addQueryItem("cid"  , QString::number(iCid));
         url.addQueryItem("gmt"  , QString::number(actShow.uiStart));

         if (actShow.id > 0)
         {
             url.addQueryItem("epg_id", QString::number(actShow.id));
         }

         // remember button ...
         img = pHtml->image(":/png/remember", 16, 16, "", tr("add to watch list ..."));

         // wrap in link ...
         timeCell += pHtml->link(url.toEncoded(), img);
      }

      progCell = actShow.sShowName;

      if (actShow.sShowDescr != "")
      {
         progCell += "<br />" + pHtml->span(actShow.sShowDescr, "color: #666");
      }

      row  = pHtml->tableCell(timeCell, bMark ? TMPL_CUR_STYLE : ((i % 2) ? TMPL_B_STYLE : TMPL_A_STYLE));
      row += pHtml->tableCell(progCell, bMark ? TMPL_CUR_STYLE : ((i % 2) ? TMPL_B_STYLE : TMPL_A_STYLE));

      // wrap in table row ...
      tab += pHtml->tableRow(row);
   }

   // wrap in table ...
   tab = pHtml->table(tab, TMPL_TAB_STYLE);

   // wrap in page ...
   page = pHtml->htmlPage(tab, "EPG Table");

   return page;
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
      int diff = QDateTime::currentDateTime().toTime_t() - startThis.toTime_t();

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
      if ((QDateTime::currentDateTime() >= startThis)
         && (QDateTime::currentDateTime() <= startNext))
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
   scrollToAnchor("nowPlaying");
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
   scrollToAnchor("nowPlaying");
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
   scrollToAnchor("nowPlaying");
}

/************************* History ***************************\
| $Log$
\*************************************************************/

