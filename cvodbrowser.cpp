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
#include "chtmlwriter.h"
#include <QUrl>

extern CLogFile VlcLog;

// for folders ...
extern CDirStuff *pFolders;

// global html writer ...
extern CHtmlWriter *pHtml;

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
   int i, j, iCount = vList.count(), iPixToLoad = 0;
   QFileInfo info;

   QString tab, row, img, link, page, title;
   QUrl url;

   row = pHtml->tableHead(sGenre, TMPL_TH_STYLE, 2);
   tab = pHtml->tableRow(row);

   for (i = 0; i < iCount; i += 2)
   {
      row = "";

      for (j = i; (j <= (i + 1)) && (j < iCount); j ++)
      {
         title = "";

         // image ...
         info.setFile(vList[j].sImg);
         img = QString("%1/%2").arg(pFolders->getVodPixDir()).arg(info.fileName());

         if (!QFile::exists(img))
         {
            iPixToLoad ++;
            pPixCache->enqueuePic(vList[j].sImg, pFolders->getVodPixDir());
         }

         // image tag ...
         img = pHtml->image(QUrl::toPercentEncoding(img), 0, 0, "", QString("%1 (%2 %3)").arg(vList[j].sName).arg(vList[j].sCountry).arg(vList[j].sYear));

         // create link url ...
         url.clear();
         url.setPath("videothek");
         url.addQueryItem("action", "vod_info");
         url.addQueryItem("vodid" , QString::number(vList[j].uiVidId));
         url.addQueryItem("pass_protect", vList[j].bProtected ? "1" : "0");

         // wrap image into link ...
         link  = pHtml->link(url.toEncoded(), img) + "<br />";

         if (vList[j].bProtected)
         {
            // add locked image ...
            title += pHtml->image(":/access/locked", 20, 20, "", tr("password protected")) + "&nbsp;";
         }

         // add title ...
         title += vList[j].sName;

         // wrap title in span ...
         link += pHtml->htmlTag("div", title, "padding: 5px;");

         // wrap into cell and add to row ...
         row  += pHtml->tableCell(link, TMPL_VOD_STYLE, 1, "center", "middle");
      }

      if (j == (iCount - 1))
      {
         row += pHtml->tableCell("&nbsp; <br /> &nbsp;", TMPL_VOD_STYLE, 1, "center", "middle");
      }

      tab += pHtml->tableRow(row);
   }

   // wrap rows into table ...
   tab = pHtml->table(tab, TMPL_TAB_STYLE);

   // wrap tab into page ...
   page = pHtml->htmlPage(tab, "VOD");

   if (iPixToLoad && pPixCache->busy())
   {
      // postbone display (when all pictures are ready) ...
      _contentBuffer = page;
   }
   else
   {
      setHtml(page);
   }
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
   QString   img, content, tab, title, link, page, tok;
   QUrl      url;
   QFileInfo info(sInfo.sImg);

   // save name ...
   _name = sInfo.sName;

   // create source url for image ...
   img = QString("%1/%2").arg(pFolders->getVodPixDir()).arg(info.fileName());

   // add image ...
   content  = pHtml->image(QUrl::toPercentEncoding(img), 0, 0, TMPL_IMG_RFLOAT, sInfo.sName);

   // add headline ...
   content += pHtml->htmlTag("h3", sInfo.sName) + "&nbsp;&nbsp;&nbsp;";

   // create short from content ...
   _shortContent = content;

   // add favorite link ...
   if (sInfo.bFavourit)
   {
      // is favourite ...
      url.clear();
      url.setPath("videothek");
      url.addQueryItem("action", "del_fav");
      url.addQueryItem("vodid", QString::number(sInfo.uiVidId));
      url.addQueryItem("pass_protect", sInfo.bProtected ? "1" : "0");

      img = pHtml->image(":/vod/is_fav", 20, 20, "", tr("Remove from favourites."));
   }
   else
   {
      // not a favourite ...
      url.clear();
      url.setPath("videothek");
      url.addQueryItem("action", "add_fav");
      url.addQueryItem("vodid", QString::number(sInfo.uiVidId));
      url.addQueryItem("pass_protect", sInfo.bProtected ? "1" : "0");

      img = pHtml->image(":/vod/not_fav", 20, 20, "", tr("Add to favourites."));
   }

   // add favorite stuff ...
   tok            = pHtml->link(url.toEncoded(), img);
   content       += tok;

   // add genre ...
   tok            = pHtml->htmlTag("p", tr("Genre: %1").arg(sInfo.sGenres), "color: #080;");
   content       += tok;
   _shortContent += tok;

   // add country, year and length ...
   tok            = pHtml->htmlTag("p", QString("%1 %2, %3 %4").arg(sInfo.sCountry).arg(sInfo.sYear).arg(sInfo.uiLength).arg(tr("min.")), "color: #888;");
   content       += tok;
   _shortContent += tok;

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
      url.addQueryItem("action", "play");
      url.addQueryItem("vid", QString::number(sInfo.vVodFiles[i].iId));
      url.addQueryItem("pass_protect", sInfo.bProtected ? "1" : "0");

      img  = pHtml->image(":/png/play", 16, 16, "", tr("Play Movie ..."));
      link = pHtml->link(url.toEncoded(), img) + "&nbsp;";

      // add record button ...
      url.clear();
      url.setPath("videothek");
      url.addQueryItem("action", "record");
      url.addQueryItem("vid", QString::number(sInfo.vVodFiles[i].iId));
      url.addQueryItem("pass_protect", sInfo.bProtected ? "1" : "0");

      img   = pHtml->image(":/png/record", 16, 16, "", tr("Record Movie ..."));
      link += pHtml->link(url.toEncoded(), img);

      tok += pHtml->tableCell(link, "padding: 3px;");

      tab += pHtml->tableRow(tok);
   }

   tab      = pHtml->htmlTag("table", tab);

   content += pHtml->htmlTag("p", tab);

   // back link ...
   url.clear();
   url.setPath("videothek");
   url.addQueryItem("action", "backtolist");

   link     = pHtml->link(url.toEncoded(), tr("Back"));
   link     = "[ " + link + " ]";
   content += pHtml->div(link, "", "center");

   page          = pHtml->htmlPage(content, tr("Video Details"), "body {background-color: rgb(255, 254, 212);}");
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

/************************* History ***************************\
| 23.12.2010 - show record link only when registration is ok
\*************************************************************/
