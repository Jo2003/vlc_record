/*=============================================================================\
| $HeadURL: https://vlc-record.googlecode.com/svn/trunk/vlc-record/ckartinaclnt.h $
|
| Author: Jo2003
|
| last changed by: $Author: Olenka.Joerg $
|
| Begin: Monday, January 04, 2010 16:11:14
|
| $Id: ckartinaclnt.h 357 2010-12-20 16:17:33Z Olenka.Joerg $
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
                                 const QString &sGenre, int iIdx)
{
   vVideos = vList;
   int i, j, iCount = vList.count();

   QString sTab, sRows, sCol;
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
         sCol.replace(TMPL_LINK,  QString("videothek?action=vod_info&vodid=%1&gidx=%2")
                                         .arg(vList[j].uiVidId)
                                         .arg(iIdx));

         // add image ...
         info.setFile(vList[j].sImg);
         sCol.replace(TMPL_IMG,   QString("%1/%2")
                                         .arg(pFolders->getVodPixDir())
                                         .arg(info.fileName()));

         // add title ...
         sCol.replace(TMPL_TITLE, QString("%1 (%2 %3)")
                                         .arg(vList[j].sName).arg(vList[j].sCountry)
                                         .arg(vList[j].sYear));

         // insert into row template ...
         sRows.replace((j == i) ? TMPL_VOD_L : TMPL_VOD_R, sCol);
      }

      if (j == (iCount - 1))
      {
         sRows.replace(TMPL_VOD_R, "&nbsp;");
      }
   }

   sTab = EPG_TMPL;
   sTab.replace(TMPL_ROWS, sRows);
   sContent.replace(TMPL_CONT, sTab);

   setHtml(sContent);
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

}

