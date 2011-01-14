/*=============================================================================\
| $HeadURL$
|
| Author: Jo2003
|
| last changed by: $Author$
|
| Begin: Tuesday, January 05, 2010 07:54:59
|
| $Id$
|
\=============================================================================*/
#ifndef __201005075459_CKARTINAXMLPARSER_H
   #define __201005075459_CKARTINAXMLPARSER_H

#include <QXmlStreamReader>
#include <QVector>
#include <QMessageBox>
#include <QRegExp>
#include <QDateTime>
#include <QString>
#include <QXmlQuery>
#include <QDomNodeList>
#include <QDomDocument>

#include "clogfile.h"
#include "defdef.h"
#include "customization.h"

//===================================================================
// XPATH defines ...
//===================================================================
#define XP_GROUP_IDS      "/response/groups/item/id/string()"
#define XP_GROUP_NAME     "/response/groups/item[id=%1]/name/string()"
#define XP_GROUP_COLOR    "/response/groups/item[id=%1]/color/string()"
#define XP_CHAN_IDS       "/response/groups/item[id=%1]/channels/item/id/string()"
#define XP_CHAN_NAME      "/response/groups/item[id=%1]/channels/item[id=%2]/name/string()"
#define XP_CHAN_VIDFLAG   "/response/groups/item[id=%1]/channels/item[id=%2]/is_video/string()"
#define XP_CHAN_PROTFLAG  "/response/groups/item[id=%1]/channels/item[id=%2]/protected/string()"
#define XP_CHAN_ARCHFLAG  "/response/groups/item[id=%1]/channels/item[id=%2]/have_archive/string()"
#define XP_CHAN_ICON      "/response/groups/item[id=%1]/channels/item[id=%2]/icon/string()"
#define XP_CHAN_SHOW      "/response/groups/item[id=%1]/channels/item[id=%2]/epg_progname/string()"
#define XP_CHAN_START     "/response/groups/item[id=%1]/channels/item[id=%2]/epg_start/string()"
#define XP_CHAN_END       "/response/groups/item[id=%1]/channels/item[id=%2]/epg_end/string()"
#define XP_STREAM_TS      "/response/groups/item[id=%1]/channels/item[id=%2]/stream_params/item/ts/string()"
#define XP_STREAM_RATE    "/response/groups/item[id=%1]/channels/item[id=%2]/stream_params/item/rate/string()"

//===================================================================
// namespace
//===================================================================
namespace cparser
{
   struct STimeShift
   {
      int iTimeShift;
      int iBitRate;
   };

   struct SChan
   {
      QString sName;
      QString sProgramm;
      QString sIcon;
      uint    uiStart;
      uint    uiEnd;
      int     iId;
      bool    bIsVideo;
      bool    bIsProtected;
      bool    bHasArchive;
      bool    bIsGroup;
      QVector<cparser::STimeShift> vTs;
   };

   struct SEpg
   {
      QString sName;
      QString sDescr;
      uint    uiGmt;
   };

   struct SSrv
   {
      QString sName;
      QString sIp;
   };

   struct SGenre
   {
      uint    uiGid;
      QString sGName;
   };

   struct SVodVideo
   {
      uint uiVidId;
      QString sName;
      QString sYear;
      QString sActors;
      QString sCountry;
      QString sDescr;
      QString sImg;
      QString sDirector;
      uint    uiLength;
      QVector<uint> vVodFiles;
   };
}

/********************************************************************\
|  Class: CKartinaXMLParser
|  Date:  19.01.2010 / 15:25:59
|  Author: Jo2003
|  Description: parser for kartina.tv sent xml content
|
\********************************************************************/
class CKartinaXMLParser : public QObject
{
   Q_OBJECT

public:
   CKartinaXMLParser(QObject * parent = 0);
   int fixTime (uint &uiTime);
   int GetTimeShift () { return iTimeShift; }
   int GetFixTime () { return iOffset; }

   // new functions for use with API ...
   int kartinaError (const QString &sResp, QString &sErr);
   int parseCookie (const QString &sResp, QString &sCookie);
   int parseTimeShift (const QString &sResp, QVector<int> &vValues, int &iShift);
   int parseChannelList (const QString &sResp, QVector<cparser::SChan> &chanList, bool bFixTime);
   int parseEpg (const QString &sResp, QVector<cparser::SEpg> &epgList);
   int parseSettings(const QString& sResp, QVector<int>& vValues, int& iActVal, QString &sName);
   int parseSServers (const QString& sResp, QVector<cparser::SSrv>& vSrv, QString& sActIp);
   int parseVodList (const QString& sResp, QVector<cparser::SVodVideo>& vVodList);
   int parseUrl (const QString& sResp, QString& sUrl);
   int parseVideoInfo (const QString& sResp, cparser::SVodVideo &vidInfo);

   // will be replaced if API is ready ...
   int parseGenres (const QString& sResp, QVector<cparser::SGenre>& vGenres);

protected:
   void checkTimeOffSet (const uint &uiSrvTime);
   QString xmlElementToValue (const QString &sElement, const QString &sName);
   void initChanEntry (cparser::SChan &entry, bool bIsChan = true);
   int parseGroups (QXmlStreamReader &xml, QVector<cparser::SChan> &chanList, bool bFixTime);
   int parseChannels(QXmlStreamReader &xml, QVector<cparser::SChan> &chanList, bool bFixTime);
   int parseStreamParams (QXmlStreamReader &xml, QVector<cparser::STimeShift>& vTs);


private:
   int iOffset;
   int iTimeShift;
};

#endif /* __201005075459_CKARTINAXMLPARSER_H */
/*=============================================================================\
|                                    History:
| ---------------------------------------------------------------------------
| 05.Jan.2010 -
\=============================================================================*/

