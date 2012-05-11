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
#include <QMap>
#include <QMutex>

#include "clogfile.h"
#include "defdef.h"

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

   struct SVodFileInfo
   {
      int     iId;
      QString sTitle;
      QString sFormat;
      QString sUrl;
      int     iSize;
      int     iLength;
      QString sCodec;
      int     iWidth;
      int     iHeight;
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
      QString sGenres;
      uint    uiLength;
      QVector<cparser::SVodFileInfo> vVodFiles;
   };

   struct SAccountInfo
   {
      QString sExpires;
      bool    bHasArchive;
      bool    bHasVOD;
   };

   struct SGenreInfo
   {
      QString sType;
      int     iCount;
      int     iPage;
      int     iTotal;
   };

   struct SUpdInfo
   {
      QString                sVersion;
      int                    iMajor;
      int                    iMinor;
      QString                sUrl;
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
   int GetFixTime () { return iOffset; }

   // new functions for use with API ...
   int checkResponse (const QString &sResp, const QString& sFunction, int iLine);
   int parseCookie (const QString &sResp, QString &sCookie, cparser::SAccountInfo &sInf);
   int parseTimeShift (const QString &sResp, QVector<int> &vValues, int &iShift);
   int parseChannelList (const QString &sResp, QVector<cparser::SChan> &chanList, bool bFixTime, bool bAllowEros);
   int parseEpg (const QString &sResp, QVector<cparser::SEpg> &epgList);
   int parseSettings(const QString& sResp, QVector<int>& vValues, int& iActVal, QString &sName);
   int parseSetting(const QString& sResp, const QString &sName, QVector<int>& vValues, int& iActVal);
   int parseSServers (const QString& sResp, QVector<cparser::SSrv>& vSrv, QString& sActIp);
   int parseSServersLogin (const QString& sResp, QVector<cparser::SSrv>& vSrv, QString& sActIp);
   int parseVodList (const QString& sResp, QVector<cparser::SVodVideo>& vVodList, cparser::SGenreInfo &gInfo);
   int parseUrl (const QString& sResp, QString& sUrl);
   int parseVideoInfo (const QString& sResp, cparser::SVodVideo &vidInfo);
   int parseGenres (const QString& sResp, QVector<cparser::SGenre>& vGenres);
   int fillErrorMap();
   int parseUpdInfo(const QString& sResp, cparser::SUpdInfo &updInfo);

protected:
   void checkTimeOffSet (const uint &uiSrvTime);
   QString xmlElementToValue (const QString &sElement, const QString &sName);
   void initChanEntry (cparser::SChan &entry, bool bIsChan = true);
   int parseGroups (QXmlStreamReader &xml, QVector<cparser::SChan> &chanList, bool bFixTime, bool bAllowEros);
   int parseChannels(QXmlStreamReader &xml, QVector<cparser::SChan> &chanList, bool bFixTime, bool bAllowEros);
   int parseStreamParams (QXmlStreamReader &xml, QVector<cparser::STimeShift>& vTs);
   int oneLevelParser (const QString &sEndElement, const QStringList& slNeeded, QMap<QString, QString>& mResults);


private:
   int iOffset;
   QString sErr, sCleanResp;
   QXmlStreamReader   xmlSr;
   QMap<int, QString> mapError;
   QMutex             mutex;
};

#endif /* __201005075459_CKARTINAXMLPARSER_H */
/*=============================================================================\
|                                    History:
| ---------------------------------------------------------------------------
| 05.Jan.2010 -
\=============================================================================*/

