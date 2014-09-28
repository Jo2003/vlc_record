/*=============================================================================\
| $HeadURL$
|
| Author: Jo2003
|
| last changed by: $Author$
|
| Begin: Monday, January 04, 2010 16:11:14
|
| $Id$
|
\=============================================================================*/
#include "cvodbrowser.h"
#include <QUrl>
#include <QUrlQuery>
#include "externals_inc.h"

/* -----------------------------------------------------------------\
|  Method: CVodBrowser / constructor
|  Begin: 21.12.2010 / 10:11
|  Author: Jo2003
|  Description: construct vod browser object
|
|  Parameters: parent widget
|
|  Returns:  --
\----------------------------------------------------------------- */
CVodBrowser::CVodBrowser(QWidget *parent) : QTextBrowser(parent)
{
   pSettings   = NULL;
   pPixCache   = NULL;
   _uiLength   = (uint)-1;
}

/* -----------------------------------------------------------------\
|  Method: ~CVodBrowser / destructor
|  Begin: 21.12.2010 / 10:11
|  Author: Jo2003
|  Description: clean on destruction
|
|  Parameters: --
|
|  Returns:  --
\----------------------------------------------------------------- */
CVodBrowser::~CVodBrowser()
{
}

/* -----------------------------------------------------------------\
|  Method: setSettings
|  Begin: 23.12.2010 / 13:11
|  Author: Jo2003
|  Description: set settings dialog to use in this class
|
|  Parameters: pointer to settings dialog
|
|  Returns:  --
\----------------------------------------------------------------- */
void CVodBrowser::setSettings(CSettingsDlg *pDlg)
{
   pSettings = pDlg;
}

/* -----------------------------------------------------------------\
|  Method: setPixCache
|  Begin: 31.05.2012
|  Author: Jo2003
|  Description: set pix loader
|
|  Parameters: pointer to pix loader
|
|  Returns:  --
\----------------------------------------------------------------- */
void CVodBrowser::setPixCache(CPixLoader *pCache)
{
   pPixCache = pCache;

   // we need to know when we can display the VOD site ...
   connect(pPixCache, SIGNAL(allDone()), this, SLOT(slotSetBufferedHtml()));
}

/* -----------------------------------------------------------------\
|  Method: displayVodList
|  Begin: 21.12.2010 / 10:11
|  Author: Jo2003
|  Description: display video list given by kartina.zv
|
|  Parameters: vector with video items, genre string
|
|  Returns:  --
\----------------------------------------------------------------- */
void CVodBrowser::displayVodList(const QVector<cparser::SVodVideo> &vList,
                                 const QString &sGenre)
{
   int     i, iCount = vList.count();
   bool    bDelay = false;
   QString tab, row, page;

   row = pHtml->tableHead(sGenre, TMPL_TH_STYLE, 2);
   tab = pHtml->tableRow(row);

   for (i = 0; i < iCount;)
   {
      // create table cell by cell and row by row  ...
      row = createVodListTableCell(vList[i], bDelay);
      i++;

      // data available ... ?
      if (i < iCount)
      {
         // yes -> add next cell with data ...
         row += createVodListTableCell(vList[i], bDelay);
      }
      else
      {
         // no -> add empty cell ...
         row += pHtml->tableCell("&nbsp;<br />&nbsp;", TMPL_VOD_STYLE, 1, "center", "middle");
      }

      // wrap into table row and add to table ...
      tab += pHtml->tableRow(row);

      i++;
   }

   // wrap rows into table ...
   tab = pHtml->table(tab, TMPL_TAB_STYLE);

   // wrap tab into page ...
   page = pHtml->htmlPage(tab, "VOD");

   if (bDelay && pPixCache->busy())
   {
      // postbone display (when all pictures are ready) ...
      _contentBuffer = page;
   }
   else
   {
      setHtml(page);
   }
}

//---------------------------------------------------------------------------
//
//! \brief   create table cell for one video in list view
//
//! \author  Jo2003
//! \date    28.08.2013
//
//! \param   entry (const cparser::SVodVideo&) vod list entry
//! \param   delay (bool &) flag for delayed display
//
//! \return  table cell html code (<td>...</td>)
//---------------------------------------------------------------------------
QString CVodBrowser::createVodListTableCell(const cparser::SVodVideo& entry, bool& delay)
{
   QString   img, title, cell;
   QFileInfo fi;
   QUrl      url;
   QUrlQuery q;

   // image ...
   fi.setFile(entry.sImg);
   img = QString("%1/%2").arg(pFolders->getVodPixDir()).arg(fi.fileName());

   if (!QFile::exists(img))
   {
      delay = true;
      pPixCache->enqueuePic(entry.sImg, pFolders->getVodPixDir());
   }

   // image tag ...
   img = pHtml->image(QUrl::toPercentEncoding(img), VOD_POSTER_WIDTH, VOD_POSTER_HEIGHT, "",
                      QString("%1 (%2 %3)").arg(entry.sName).arg(entry.sCountry).arg(entry.sYear));

   // create link url ...
   url.setPath("videothek");
   q.addQueryItem("action", "vod_info");
   q.addQueryItem("vodid" , QString::number(entry.uiVidId));
   q.addQueryItem("pass_protect", entry.bProtected ? "1" : "0");
   url.setQuery(q);

   // wrap image into link ...
   cell = pHtml->link(url.toEncoded(), img) + "<br />";

   if (entry.bProtected)
   {
      // add locked image ...
      title = pHtml->image(":/access/locked", 20, 20, "", tr("password protected")) + "&nbsp;";
   }

   // add title ...
   title += entry.sName;

   // wrap title in div ...
   cell += pHtml->htmlTag("div", title, "padding: 5px;");

   // wrap into cell ...
   return pHtml->tableCell(cell, TMPL_VOD_STYLE, 1, "center", "middle");
}

/* -----------------------------------------------------------------\
|  Method: slotSetBufferedHtml
|  Begin: 31.05.2012
|  Author: Jo2003
|  Description: display buffered content
|
|  Parameters: --
|
|  Returns:  --
\----------------------------------------------------------------- */
void CVodBrowser::slotSetBufferedHtml()
{
   if (!_contentBuffer.isEmpty())
   {
      setHtml(_contentBuffer);
      _contentBuffer.clear();
   }
}

/* -----------------------------------------------------------------\
|  Method: displayVideoDetails
|  Begin: 21.12.2010 / 10:11
|  Author: Jo2003
|  Description: display video details
|
|  Parameters: struct with video info
|
|  Returns:  --
\----------------------------------------------------------------- */
void CVodBrowser::displayVideoDetails(const cparser::SVodVideo &sInfo)
{
   int       i;
   QString   img, content, tab, title, link, page, tok, back;
   QUrl      url;
   QUrlQuery q;
   QFileInfo info(sInfo.sImg);

   // save name ...
   _name = sInfo.sName;

   // save length in seconds ...
   _uiLength = sInfo.uiLength * 60;

   // back link ...
   url.clear();
   url.setPath("videothek");
   q.clear();
   q.addQueryItem("action", "backtolist");
   url.setQuery(q);
   
   link  = pHtml->link(url.toEncoded(), tr("Back"));
   link  = "[ " + link + " ]";
   back  = pHtml->div(link, "", "center");

   // create source url for image ...
   img = QString("%1/%2").arg(pFolders->getVodPixDir()).arg(info.fileName());

   // add image ...
   content = pHtml->image(QUrl::toPercentEncoding(img), VOD_POSTER_WIDTH, VOD_POSTER_HEIGHT,
                           TMPL_IMG_RFLOAT, sInfo.sName);

   // add headline ...
   content += pHtml->htmlTag("h3", QString("%1&nbsp;&nbsp;&nbsp;").arg(sInfo.sName));

   // create short from content ...
   _shortContent = content;

   // prepend back link ...
   content = back + content;

   // add favorite link ...
   if (sInfo.bFavourit)
   {
      // is favourite ...
      url.clear();
      url.setPath("videothek");
      q.clear();
      q.addQueryItem("action", "del_fav");
      q.addQueryItem("vodid", QString::number(sInfo.uiVidId));
      q.addQueryItem("pass_protect", sInfo.bProtected ? "1" : "0");
      url.setQuery(q);

      img = pHtml->image(":/vod/is_fav", 20, 20, "", tr("Remove from favourites."));
   }
   else
   {
      // not a favourite ...
      url.clear();
      url.setPath("videothek");
      q.clear();
      q.addQueryItem("action", "add_fav");
      q.addQueryItem("vodid", QString::number(sInfo.uiVidId));
      q.addQueryItem("pass_protect", sInfo.bProtected ? "1" : "0");
      url.setQuery(q);

      img = pHtml->image(":/vod/not_fav", 20, 20, "", tr("Add to favourites."));
   }

   // add favorite stuff ...
   tok            = pHtml->link(url.toEncoded(), img);
   content       += tok;

   // add genre ...
   tok            = pHtml->htmlTag("p", tr("Genre: %1").arg(sInfo.sGenres), "color: #080;");
   content       += tok;
   _shortContent += tok;

   // add country, year  ...
   tok = QString("%1 %2").arg(sInfo.sCountry).arg(sInfo.sYear);

   // add length ...
   if (sInfo.uiLength != 0)
   {
      tok += QString(", %1 %2").arg(sInfo.uiLength).arg(tr("min."));
   }

   // add rating...
   if (!sInfo.sPgRating.isEmpty())
   {
      tok += QString(", %1: %2").arg(tr("Rating")).arg(sInfo.sPgRating);
   }

   tok  = pHtml->htmlTag("p", tok, "color: #888;");
   content       += tok;
   _shortContent += tok;

   tok = "";

   if (!sInfo.sImdbRating.isEmpty())
   {
      tok = tr("IMDB: %1").arg(sInfo.sImdbRating);
   }

   if (!sInfo.sKinopoiskRating.isEmpty())
   {
      if (!tok.isEmpty())
      {
         tok += "; ";
      }

      tok += tr("Kinopoisk: %1").arg(sInfo.sKinopoiskRating);
   }

   if (!tok.isEmpty())
   {
      tok  = pHtml->htmlTag("p", tok, "color: #880;");
      content       += tok;
      _shortContent += tok;
   }

   // add director ...
   tok            = pHtml->htmlTag("p", tr("Director: %1").arg(sInfo.sDirector), "color: #800;");
   content       += tok;
   _shortContent += tok;

   // add actors ...
   tok            = pHtml->htmlTag("p", tr("With: %1").arg(sInfo.sActors), "color: #008;");
   content       += tok;
   _shortContent += tok;

   // add description ...
   tok            = pHtml->htmlTag("p", sInfo.sDescr);
   content       += tok;
   _shortContent += tok;

   // add play / record links ...
   for (i = 0; i < sInfo.vVodFiles.count(); i ++)
   {
      // add stream title ...
      title = sInfo.vVodFiles[i].sTitle.isEmpty() ? tr("Part %1").arg(i + 1) : sInfo.vVodFiles[i].sTitle;
      title = pHtml->htmlTag("b", title);

      tok   = pHtml->tableCell(title, "padding: 3px;");

      // add codec ...
      title = QString("(%1; %2)").arg(sInfo.vVodFiles[i].sFormat).arg(sInfo.vVodFiles[i].sCodec);
      title = pHtml->span(title, "color: #888");

      tok  += pHtml->tableCell(title, "padding: 3px;");

      // add play button ...
      url.clear();
      url.setPath("videothek");
      q.clear();
      q.addQueryItem("action", "play");
      q.addQueryItem("vid", QString::number(sInfo.vVodFiles[i].iId));
      q.addQueryItem("pass_protect", sInfo.bProtected ? "1" : "0");
      url.setQuery(q);

      img  = pHtml->image(":/png/play", 16, 16, "", tr("Play Movie ..."));
      link = pHtml->link(url.toEncoded(), img) + "&nbsp;";

      // add record button ...
      url.clear();
      url.setPath("videothek");
      q.clear();
      q.addQueryItem("action", "record");
      q.addQueryItem("vid", QString::number(sInfo.vVodFiles[i].iId));
      q.addQueryItem("pass_protect", sInfo.bProtected ? "1" : "0");
      url.setQuery(q);

      img   = pHtml->image(":/png/record", 16, 16, "", tr("Record Movie ..."));
      link += pHtml->link(url.toEncoded(), img);

      tok += pHtml->tableCell(link, "padding: 3px;");

      tab += pHtml->tableRow(tok);
   }

   tab      = pHtml->htmlTag("table", tab);

   content += pHtml->htmlTag("p", tab);

   // back link ...
   content += back;

   page          = pHtml->htmlPage(content, tr("Video Details"), TMPL_VOD_BODY);
   _shortContent = pHtml->oneCellPage(_shortContent, TMPL_ONE_CELL);

   setHtml(page);
}

/* -----------------------------------------------------------------\
|  Method: getName
|  Begin: 22.12.2010 / 10:11
|  Author: Jo2003
|  Description: get last used movie name
|
|  Parameters: --
|
|  Returns:  movie name
\----------------------------------------------------------------- */
const QString& CVodBrowser::getName()
{
   return _name;
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
void CVodBrowser::EnlargeFont()
{
   QFont vodFont = font();
   vodFont.setPointSize(vodFont.pointSize() + 1);
   setFont(vodFont);
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
void CVodBrowser::ReduceFont()
{
   QFont vodFont = font();
   vodFont.setPointSize(vodFont.pointSize() - 1);
   setFont(vodFont);
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
void CVodBrowser::ChangeFontSize(int iSz)
{
   QFont vodFont = font();
   vodFont.setPointSize(vodFont.pointSize() + iSz);
   setFont(vodFont);
}

/* -----------------------------------------------------------------\
|  Method: getShortContent
|  Begin: 22.09.2011
|  Author: Jo2003
|  Description: get short content html code
|
|  Parameters: --
|
|  Returns: html code as string
\----------------------------------------------------------------- */
const QString& CVodBrowser::getShortContent()
{
   return _shortContent;
}

//---------------------------------------------------------------------------
//
//! \brief   get video length
//
//! \author  Jo2003
//! \date    23.05.2014
//
//! \param   --
//
//! \return  length in seconds
//---------------------------------------------------------------------------
uint CVodBrowser::getLength()
{
   return _uiLength;
}

/************************* History ***************************\
| 23.12.2010 - show record link only when registration is ok
\*************************************************************/
