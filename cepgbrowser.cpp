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
#include "cepgbrowser.h"

// log file functions ...
extern CLogFile VlcLog;

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
   iTimeShift = 0;
   iCid       = 0;
   mProgram.clear();
}

/* -----------------------------------------------------------------\
|  Method: DisplayEpg
|  Begin: 18.01.2010 / 16:08:35
|  Author: Jo2003
|  Description: display EPG entries
|
|  Parameters: list of entries, channel name, channel id,
|              timestamp, archiv flag
|
|  Returns: --
\----------------------------------------------------------------- */
void CEpgBrowser::DisplayEpg(QVector<cparser::SEpg> epglist,
                             const QString &sName, int iChanID, uint uiGmt,
                             bool bHasArchiv)
{
   epg::SShow actShow;

   // store values ...
   sChanName = sName;
   iCid      = iChanID;
   uiTime    = uiGmt;
   bArchive  = bHasArchiv;

   // clear program map ...
   mProgram.clear();

   for (int i = 0; i < epglist.size(); i ++)
   {
      actShow.sShowName  = epglist[i].sName;
      actShow.sShowDescr = epglist[i].sDescr;
      actShow.uiStart    = epglist[i].uiGmt;
      actShow.uiEnd      = ((i + 1) < epglist.size()) ? epglist[i + 1].uiGmt : 0;

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
   QString    sRows   = "", sRow, sProgCell, sHtmlDoc, sStartTime, sHeadLine;
   QDateTime  dtStartThis, dtStartNext;
   bool       bMark;
   epg::SShow actShow;
   int        i;
   QMap<uint, epg::SShow>::const_iterator cit;


   // create headline (table head) ...
   sHeadLine = QString("%1 - %2")
               .arg(sChanName)
               .arg(QDateTime::fromTime_t(uiTime).toString("dd. MMM. yyyy"));

   sRow = TR_HEAD;
   sRow.replace(TMPL_HEAD, sHeadLine);
   sRows += sRow;

   for (cit = mProgram.constBegin(), i = 0; cit != mProgram.constEnd(); cit++, i++)
   {
      actShow = *cit;

      bMark       = false;
      sProgCell   = "";
      dtStartThis = QDateTime::fromTime_t(actShow.uiStart + iTimeShift * 3600);

      // find out if we should mark the time ...
      if (actShow.uiEnd)
      {
         dtStartNext = QDateTime::fromTime_t(actShow.uiEnd + iTimeShift * 3600);
         bMark       = NowRunning(dtStartThis, dtStartNext);
      }
      else
      {
         bMark       = NowRunning(dtStartThis);
      }

      if (bMark)
      {
         sRow      = TR_TMPL_ACTUAL;
         sProgCell = "<a name='nowPlaying' />";
      }
      else
      {
         sRow = (i % 2) ? TR_TMPL_B : TR_TMPL_A;
      }

      sProgCell += actShow.sShowName;

      if (actShow.sShowDescr != "")
      {
         sProgCell += QString("<br /><span style='color: #666'>%1</span>").arg(actShow.sShowDescr);
      }

      sStartTime = dtStartThis.toString("hh:mm");

      // archiv available ...
      if (bArchive)
      {
         // only show archiv links if this show already has ended ...
         if (ArchivAvailable(actShow.uiStart))
         {
            QString sArchivLinks = QString("<hr /><b>%1:</b> &nbsp;")
                                   .arg(tr("Ar."));

            // play ...
            sArchivLinks += QString("<a href='vlc-record?action=archivplay&cid=%1&gmt=%2'>"
                                    "<img src=':png/play' width='16' height='16' alt='play' "
                                    "title='%3' /></a>&nbsp;")
                                    .arg(iCid).arg(actShow.uiStart)
                                    .arg(tr("play from archive ..."));

            // record ...
            sArchivLinks += QString("<a href='vlc-record?action=archivrec&cid=%1&gmt=%2'>"
                                    "<img src=':png/record' width='16' height='16' alt='record' "
                                    "title='%3' /></a>")
                                    .arg(iCid).arg(actShow.uiStart)
                                    .arg(tr("record from archive ..."));

            sStartTime += sArchivLinks;
         }
      }

      // timer record stuff ...
      if (dtStartThis > QDateTime::currentDateTime())
      {
         // has not started so far ... add timer record link ...


         // add timer rec link ...
         sStartTime += QString("&nbsp;<a href='vlc-record?action=timerrec&cid=%1&start=%2&end=%3'>"
                               "<img src=':png/timer' width='16' height='16' alt='timer' "
                               "title='%4' /></a>&nbsp;")
                               .arg(iCid).arg(actShow.uiStart).arg(actShow.uiEnd)
                               .arg(tr("add timer record ..."));
      }


      sRow.replace(TMPL_PROG, sProgCell);
      sRow.replace(TMPL_TIME, sStartTime);

      sRows += sRow;
   }

   sHtmlDoc = EPG_TMPL;
   sHtmlDoc.replace(TMPL_ROWS, sRows);

   return sHtmlDoc;
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
|  Method: ArchivAvailable
|  Begin: 18.01.2010 / 16:11:12
|  Author: Jo2003
|  Description: check if archiv is avalable for given show
|
|  Parameters: timestamp for this show
|
|  Returns: true ==> archiv available
|          false ==> not available
\----------------------------------------------------------------- */
bool CEpgBrowser::ArchivAvailable(uint uiThisShow)
{
   bool bArchiv = false;
   uint now     = QDateTime::currentDateTime().toTime_t();

   // archiv should be available 10 minutes after show start
   // in a time frame of 2 weeks ...
   if (((now - ARCHIV_OFFSET) > uiThisShow)        // 10 mins. up
      && (uiThisShow > (now - MAX_ARCHIV_AGE)))    // within the 2 weeks
   {
      bArchiv = true;
   }

   return bArchiv;
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

