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
#include <QRegExp>
#include <QDateTime>
#include <QStringList>

#include "clogfile.h"
#include "defdef.h"
#include "cparser.h"

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
   int parseCookie (const QString &sResp, QString &sCookie, cparser::SAccountInfo &sInf);
   int parseTimeShift (const QString &sResp, QVector<int> &vValues, int &iShift);
   int parseChannelList (const QString &sResp, QVector<cparser::SChan> &chanList, bool bFixTime);
   int parseEpg (const QString &sResp, QVector<cparser::SEpg> &epgList);
   int parseSettings(const QString& sResp, QVector<int>& vValues, int& iActVal, QString &sName);
   int parseSetting(const QString& sResp, const QString &sName, QVector<int>& vValues, int& iActVal);
   int parseSServers (const QString& sResp, QVector<cparser::SSrv>& vSrv, QString& sActIp);
   int parseSServersLogin (const QString& sResp, QVector<cparser::SSrv>& vSrv, QString& sActIp);
   int parseVodList (const QString& sResp, QVector<cparser::SVodVideo>& vVodList, cparser::SGenreInfo &gInfo);
   int parseUrl (const QString& sResp, QString& sUrl);
   int parseVodUrls (const QString& sResp, QStringList& sUrls);
   int parseVideoInfo (const QString& sResp, cparser::SVodVideo &vidInfo);
   int parseGenres (const QString& sResp, QVector<cparser::SGenre>& vGenres);
   int parseVodManager (const QString& sResp, QVector<cparser::SVodRate>& vRates);
   int parseUpdInfo(const QString& sResp, cparser::SUpdInfo &updInfo);
   int parseEpgCurrent (const QString& sResp, QCurrentMap &currentEpg);

protected:
   void checkTimeOffSet (const uint &uiSrvTime);
   QString xmlElementToValue (const QString &sElement, const QString &sName);
   void initChanEntry (cparser::SChan &entry, bool bIsChan = true);
   int parseGroups (QXmlStreamReader &xml, QVector<cparser::SChan> &chanList, bool bFixTime);
   int parseChannels(QXmlStreamReader &xml, QVector<cparser::SChan> &chanList, bool bFixTime);
   int parseStreamParams (QXmlStreamReader &xml, QVector<cparser::STimeShift>& vTs);
   int oneLevelParser (QXmlStreamReader &xml, const QString &sEndElement, const QStringList& slNeeded, QMap<QString, QString>& mResults);
   int ignoreUntil(QXmlStreamReader &xml, const QString &sEndElement);


private:
   int iOffset;

signals:
   void sigWrongPass();
   void sigError(int iType, const QString& cap, const QString& descr);
};

#endif /* __201005075459_CKARTINAXMLPARSER_H */
/*=============================================================================\
|                                    History:
| ---------------------------------------------------------------------------
| 05.Jan.2010 -
\=============================================================================*/

