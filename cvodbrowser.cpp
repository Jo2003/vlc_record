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

extern CLogFile VlcLog;

// for folders ...
extern CDirStuff *pFolders;

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
                                 const QString &sGenre,
                                 bool bSaveList)
{
   int i, j, iCount = vList.count(), iPixToLoad = 0;

   if (bSaveList)
   {
      vVideos = vList;
   }

   QString sTab, sRows, sCol, sVidTitle, sLock, sImage;
   QString sContent = HTML_SITE;
   QFileInfo info;
   sContent.replace(TMPL_TITLE, tr("VOD"));

   // we create a table with 2 columns here ...
   sRows = TR_HEAD;
   sRows.replace(TMPL_HEAD, sGenre);

   for (i = 0; i < iCount; i += 2)
   {
      sRows += TR_VOD_LIST;

      for (j = i; (j <= (i + 1)) && (j < iCount); j ++)
      {
         // load link image template ...
         sCol = TMPL_IMG_LINK;

         // add link ...
         sCol.replace(TMPL_LINK,  QString("videothek?action=vod_info&vodid=%1&pass_protect=%2")
                      .arg(vList[j].uiVidId)
                      .arg(vList[j].bProtected ? 1 : 0));

         // handle image ...
         info.setFile(vList[j].sImg);
         sImage = QString("%1/%2").arg(pFolders->getVodPixDir()).arg(info.fileName());

         // enqueue pic if not already there in cache ...
         if (!QFile::exists(sImage))
         {
            iPixToLoad ++;
            pPixCache->enqueuePic(vList[j].sImg, pFolders->getVodPixDir());
         }

         // add image ...
         sCol.replace(TMPL_IMG,   QUrl::toPercentEncoding(sImage));

         // add title ...
         sCol.replace(TMPL_TITLE, QString("%1 (%2 %3)")
                                         .arg(vList[j].sName).arg(vList[j].sCountry)
                                         .arg(vList[j].sYear));

         sLock = "";
         // show if video is protected ...
         if (vList[j].bProtected)
         {
            sLock = TMPL_HTML_ICON;
            sLock.replace(TMPL_IMG, ":/access/locked");
            sLock.replace(TMPL_TITLE, tr("password protected"));
         }

         // add title below image ...
         sVidTitle = TMPL_VIDEO_TITLE;
         sVidTitle.replace(TMPL_TITLE, QString("%1%2").arg(sLock).arg(vList[j].sName));

         // insert into row template ...
         sRows.replace((j == i) ? TMPL_VOD_L   : TMPL_VOD_R,   sCol);
         sRows.replace((j == i) ? TMPL_TITLE_L : TMPL_TITLE_R, sVidTitle);
      }

      if (j == (iCount - 1))
      {
         sRows.replace(TMPL_VOD_R,   "&nbsp;");
         sRows.replace(TMPL_TITLE_R, "&nbsp;");
      }
   }

   sTab = EPG_TMPL;
   sTab.replace(TMPL_ROWS, sRows);
   sContent.replace(TMPL_CONT, sTab);

   if (iPixToLoad && pPixCache->busy())
   {
      // postbone display (when all pictures are ready) ...
      sContentBuffer = sContent;
   }
   else
   {
      setHtml(sContent);
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
   if (sContentBuffer != "")
   {
      setHtml(sContentBuffer);
      sContentBuffer = "";
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
   QString   sDoc = HTML_SITE;
   QString   sCss = TMPL_CSS_WRAPPER;
   QString   sLinks;
   QString   sLinkTab;
   QString   sTitle;
   QString   sFormat;
   QString   sFav;
   QFileInfo info(sInfo.sImg);

   // add css stuff ...
   sCss.replace(TMPL_CSS, TMPL_CSS_IMG_FLOAT);
   sDoc.replace(TMPL_CSS, sCss);

   // document title ...
   sDoc.replace(TMPL_TITLE, tr("Video Details"));

   // insert video details template ...
   sDoc.replace(TMPL_CONT, TMPL_VIDEO_DETAILS);

   // insert poster ...
   sDoc.replace(TMPL_IMG, QUrl::toPercentEncoding(QString("%1/%2")
                .arg(pFolders->getVodPixDir())
                .arg(info.fileName())));

   // insert name ...
   sDoc.replace(TMPL_TITLE, sInfo.sName);
   sName = sInfo.sName;

   // insert country, year and time ...
   sDoc.replace(TMPL_TIME, QString("%1 %2, %3 %4")
                .arg(sInfo.sCountry)
                .arg(sInfo.sYear)
                .arg(sInfo.uiLength)
                .arg(tr("min.")));

   // insert director ...
   sDoc.replace(TMPL_DIREC, tr("Director: %1")
                .arg(sInfo.sDirector));

   // insert actors ...
   sDoc.replace(TMPL_ACTORS, tr("With: %1")
                .arg(sInfo.sActors));

   // insert genres ...
   sDoc.replace(TMPL_GENRE, tr("Genre: %1")
                .arg(sInfo.sGenres));

   // insert description ...
   sDoc.replace(TMPL_PROG, sInfo.sDescr);

   // for the short info we can end here ...
   sShortContent = sDoc;
   sShortContent.replace(TMPL_LINK, "");
   sShortContent.replace(TMPL_FAVO, "");

   // add favourite stuff ...
   sFav = TEMPL_VOD_FAV;
   if (sInfo.bFavourit)
   {
      // is favourite ...
      sFav.replace(TMPL_IMG, ":/vod/is_fav");
      sFav.replace(TMPL_TITLE, tr("Remove from favourites."));
      sFav.replace(TMPL_LINK, QString("videothek?action=del_fav&vodid=%1&pass_protect=%2")
                   .arg(sInfo.uiVidId)
                   .arg(sInfo.bProtected ? 1 : 0));
   }
   else
   {
      // not a favourite ...
      sFav.replace(TMPL_IMG, ":/vod/not_fav");
      sFav.replace(TMPL_TITLE, tr("Add to favourites."));
      sFav.replace(TMPL_LINK, QString("videothek?action=add_fav&vodid=%1&pass_protect=%2")
                   .arg(sInfo.uiVidId)
                   .arg(sInfo.bProtected ? 1 : 0));
   }
   sDoc.replace(TMPL_FAVO, sFav);

   sLinks = TMPL_VIDEO_LINKS;

   // back link ...
   sLinks.replace(TMPL_END, tr("Back"));

   sLinkTab = "<table>\n";

   // links ...
   for (int i = 0; i < sInfo.vVodFiles.count(); i ++)
   {
      sLinkTab += "<tr>\n";
      sTitle  = (sInfo.vVodFiles[i].sTitle == "") ? tr("Part %1").arg(i + 1) : sInfo.vVodFiles[i].sTitle;
      sFormat = TMPL_CODEC;
      sFormat.replace(TMPL_TITLE, QString("(%1; %2)").arg(sInfo.vVodFiles[i].sFormat).arg(sInfo.vVodFiles[i].sCodec));

      sLinkTab += QString("<td style='padding: 3px;'><b>%1</b></td>\n").arg(sTitle);
      sLinkTab += QString("<td style='padding: 3px;'>%1</td>\n").arg(sFormat);

      // play link ...
      sLinkTab += "<td style='padding: 3px;'>\n";
      sLinkTab += TMPL_IMG_LINK;
      sLinkTab.replace(TMPL_IMG, ":png/play");
      sLinkTab.replace(TMPL_LINK, QString("videothek?action=play&vid=%1&pass_protect=%2")
                       .arg(sInfo.vVodFiles[i].iId)
                       .arg(sInfo.bProtected ? 1 : 0));

      sLinkTab.replace(TMPL_TITLE, tr("Play Movie ..."));

      sLinkTab += "&nbsp;&nbsp;";

      // record link ...
      sLinkTab += TMPL_IMG_LINK;
      sLinkTab.replace(TMPL_IMG, ":png/record");
      sLinkTab.replace(TMPL_LINK, QString("videothek?action=record&vid=%1&pass_protect=%2")
                       .arg(sInfo.vVodFiles[i].iId)
                       .arg(sInfo.bProtected ? 1 : 0));
      sLinkTab.replace(TMPL_TITLE, tr("Record Movie ..."));

      sLinkTab += "\n</td>\n</tr>\n";
   }

   sLinkTab += "</table>\n";

   sLinks.replace(TMPL_LINK, sLinkTab);

   sDoc.replace(TMPL_LINK, sLinks);

   setHtml(sDoc);
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
   return sName;
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
   return sShortContent;
}

/************************* History ***************************\
| 23.12.2010 - show record link only when registration is ok
\*************************************************************/
