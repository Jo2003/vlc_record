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
#include "qurlex.h"
#include "externals_inc.h"
#include <QScrollBar>

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
CVodBrowser::CVodBrowser(QWidget *parent) : QTextBrowserEx(parent)
{
   pSettings   = NULL;
   pPixCache   = NULL;
   _uiLength   = (uint)-1;
   mScrollPos  = -1;
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

//---------------------------------------------------------------------------
//
//! \brief   delete video entry from cached video list
//
//! \author  Jo2003
//! \date    09.11.2015
//
//! \param  [in] vIdx (uint) video id
//
//---------------------------------------------------------------------------
void CVodBrowser::deleteFromLastList(uint vIdx)
{
    if (!mLastList.isEmpty())
    {
        QVector<cparser::SVodVideo>::Iterator it;

        for (it = mLastList.begin(); it != mLastList.end(); it ++)
        {
            if (it->uiVidId == vIdx)
            {
                it = mLastList.erase(it);
                break;
            }
        }
    }
}

//---------------------------------------------------------------------------
//
//! \brief   save scrollbar value for re-creation
//
//! \author  Jo2003
//! \date    09.11.2015
//
//---------------------------------------------------------------------------
void CVodBrowser::saveScrollPos()
{
    mScrollPos = verticalScrollBar()->value();
}

//---------------------------------------------------------------------------
//
//! \brief   re-create vod list from cached entry
//
//! \author  Jo2003
//! \date    09.11.2015
//
//---------------------------------------------------------------------------
void CVodBrowser::recreateVodList()
{
    displayVodList(mLastList, mLastGenre, mBackLink);
    verticalScrollBar()->setValue(mScrollPos);
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
|  Parameters: vector with video items, genre string, link back
|
|  Returns:  --
\----------------------------------------------------------------- */
void CVodBrowser::displayVodList(const QVector<cparser::SVodVideo> &vList,
                                 const QString &sGenre, bool backLink)
{
   int     i, iCount = vList.count();
   bool    bDelay = false;
   QString tab, row, page;

   // buffer data ...
   mLastList  = vList;
   mLastGenre = sGenre;
   mBackLink  = backLink;

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

   if (backLink)
   {
       // back link ...
       QUrlEx url;
       QString link, back;
       url.addQueryItem("action", "backHome");
       url.setPath("videothek");

       link  = pHtml->link(url.toEncoded(), tr("Back"));
       link  = "[ " + link + " ]";
       back  = pHtml->div(link, "font-weight: bold; color: #800;", "center");

       tab = back + tab + back;
   }

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
   QUrlEx    url;

   // image ...
   fi.setFile(entry.sImg);
   img = QString("%1/%2").arg(pFolders->getVodPixDir()).arg(fi.fileName());

   if (!QFile::exists(img))
   {
      delay = true;
      pPixCache->enqueuePic(entry.sImg, pFolders->getVodPixDir());
   }

   // image tag ...
   img = pHtml->image(QUrl::toPercentEncoding(img), VOD_POSTER_WIDTH, 0, "",
                      QString("%1 (%2 %3)").arg(entry.sName).arg(entry.sCountry).arg(entry.sYear));

   // create link url ...
   url.clear();
   url.addQueryItem("action", "vod_info");
   url.addQueryItem("vodid" , QString::number(entry.uiVidId));
   if (entry.iKind != -1)
   {
       url.addQueryItem("kind" , QString::number(entry.iKind));
   }

   if (entry.iContentCount != -1)
   {
       url.addQueryItem("count" , QString::number(entry.iContentCount));
   }

   if (entry.iCompId != -1)
   {
       url.addQueryItem("compid" , QString::number(entry.iCompId));
   }

   url.addQueryItem("pass_protect", entry.bProtected ? "1" : "0");
   url.setPath("videothek");

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
   QUrlEx    url;
   QFileInfo info(sInfo.sImg);

   // save name ...
   _name = sInfo.sName;

   // save length in seconds ...
   _uiLength = sInfo.uiLength * 60;

   // back link ...
   url.clear();
   url.addQueryItem("action", "backtolist");
   url.setPath("videothek");

   link  = pHtml->link(url.toEncoded(), tr("Back"));
   link  = "[ " + link + " ]";
   back  = pHtml->div(link, "", "center");

   // create source url for image ...
   img = QString("%1/%2").arg(pFolders->getVodPixDir()).arg(info.fileName());

   // add image ...
   content = pHtml->image(QUrl::toPercentEncoding(img), VOD_POSTER_WIDTH, 0,
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
      url.addQueryItem("action", "del_fav");

      // video compilation (do not handle a single show independed) ...
      if (sInfo.iCompId != -1)
      {
         url.addQueryItem("vodid", QString::number(sInfo.iCompId));
         url.addQueryItem("kind", QString::number(2)); // ivi::KIND_COMPILATION
      }
      else
      {
         url.addQueryItem("vodid", QString::number(sInfo.uiVidId));
         if (sInfo.iKind > 0)
         {
             url.addQueryItem("kind", QString::number(sInfo.iKind));
         }
      }

      url.addQueryItem("pass_protect", sInfo.bProtected ? "1" : "0");
      url.setPath("videothek");

      img = pHtml->image(":/vod/is_fav", 20, 20, "", tr("Remove from favourites."));
   }
   else
   {
      // not a favourite ...
      url.clear();
      url.addQueryItem("action", "add_fav");

      // video compilation (do not handle a single show independed) ...
      if (sInfo.iCompId != -1)
      {
         url.addQueryItem("vodid", QString::number(sInfo.iCompId));
         url.addQueryItem("kind", QString::number(2)); // ivi::KIND_COMPILATION
      }
      else
      {
         url.addQueryItem("vodid", QString::number(sInfo.uiVidId));
         if (sInfo.iKind > 0)
         {
             url.addQueryItem("kind", QString::number(sInfo.iKind));
         }
      }

      url.addQueryItem("pass_protect", sInfo.bProtected ? "1" : "0");
      url.setPath("videothek");

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
#ifdef _TASTE_POLSKY_TV
      tok += tr("Filmweb: %1").arg(sInfo.sKinopoiskRating);
#else
      tok += tr("Kinopoisk: %1").arg(sInfo.sKinopoiskRating);
#endif // _TASTE_POLSKY_TV
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
      title = QString("%1 / %2").arg(sInfo.vVodFiles[i].sFormat).arg(sInfo.vVodFiles[i].sCodec);

      if (sInfo.vVodFiles[i].sFormat == "tv")
      {
         title = pHtml->image(":/vod/eco", 18, 18, "", title);
      }
      else if (sInfo.vVodFiles[i].sFormat == "dvd")
      {
         title = pHtml->image(":/vod/sd", 18, 18, "", title);
      }
      else if ((sInfo.vVodFiles[i].sFormat == "fullhd") || (sInfo.vVodFiles[i].sFormat == "hd"))
      {
         title = pHtml->image(":/vod/hd", 18, 18, "", title);
      }
      else if (sInfo.vVodFiles[i].sFormat == "3d")
      {
         title = pHtml->image(":/vod/3d", 18, 18, "", title);
      }
      else if (sInfo.vVodFiles[i].sFormat == "hq")
      {
         title = pHtml->image(":/vod/hq", 18, 18, "", title);
      }
      else
      {
         title = pHtml->span(QString("(%1)").arg(title), "color: #888");
      }

      // title = QString("(%1; %2)").arg(sInfo.vVodFiles[i].sFormat).arg(sInfo.vVodFiles[i].sCodec);
      // title = pHtml->span(title, "color: #888");

      tok  += pHtml->tableCell(title, "padding: 3px;");

      // add play button ...
      url.clear();
      url.addQueryItem("action", "play");
      url.addQueryItem("vid", QString::number(sInfo.vVodFiles[i].iId));
      url.addQueryItem("video_id", QString::number(sInfo.uiVidId));
      url.addQueryItem("pass_protect", sInfo.bProtected ? "1" : "0");
      if (!sInfo.vVodFiles[i].sUrl.isEmpty())
      {
          url.addQueryItem("video_url", QUrlEx::toPercentEncoding(sInfo.vVodFiles[i].sUrl.toUtf8()));
      }
      url.setPath("videothek");

      img  = pHtml->image(":/png/play", 16, 16, "", tr("Play Movie ..."));
      link = pHtml->link(url.toEncoded(), img) + "&nbsp;";

      // add record button ...
      url.clear();
      url.addQueryItem("action", "record");
      url.addQueryItem("vid", QString::number(sInfo.vVodFiles[i].iId));
      url.addQueryItem("video_id", QString::number(sInfo.uiVidId));
      url.addQueryItem("pass_protect", sInfo.bProtected ? "1" : "0");
      if (!sInfo.vVodFiles[i].sUrl.isEmpty())
      {
          url.addQueryItem("video_url", QUrlEx::toPercentEncoding(sInfo.vVodFiles[i].sUrl.toUtf8()));
      }
      url.setPath("videothek");

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
