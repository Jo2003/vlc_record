/*=============================================================================\
| $HeadURL$
|
| Author: Jo2003
|
| last changed by: $Author$
|
| Begin: 19.03.2013
|
| $Id$
|
\=============================================================================*/
#ifndef __20130319_CRODNOEPARSER_H
   #define __20130319_CRODNOEPARSER_H

#include <QXmlStreamReader>
#include <QRegExp>
#include <QDateTime>
#include <QStringList>

#include "clogfile.h"
#include "defdef.h"
#include "cparser.h"
#include "capixmlparser.h"

/********************************************************************\
|  Class: CRodnoeParser
|  Date:  19.03.2013
|  Author: Jo2003
|  Description: parser for iptv sent xml content
|
\********************************************************************/
class CRodnoeParser : public CApiXmlParser
{
   Q_OBJECT

public:
   CRodnoeParser(QObject * parent = 0);

   // new functions for use with API ...
   virtual int parseCookie (const QString &sResp, QString &sCookie, cparser::SAccountInfo &sInf);
   virtual int parseChannelList (const QString &sResp, QVector<cparser::SChan> &chanList, bool bFixTime);
   virtual int parseEpg (const QString &sResp, QVector<cparser::SEpg> &epgList);
   virtual int parseSetting(const QString& sResp, const QString &sName, QVector<int>& vValues, int& iActVal);
   virtual int parseSServersLogin (const QString& sResp, QVector<cparser::SSrv>& vSrv, QString& sActIp);
   virtual int parseVodList (const QString& sResp, QVector<cparser::SVodVideo>& vVodList, cparser::SGenreInfo &gInfo);
   virtual int parseUrl (const QString& sResp, QString& sUrl);
   virtual int parseVodUrls (const QString& sResp, QStringList& sUrls);
   virtual int parseVideoInfo (const QString& sResp, cparser::SVodVideo &vidInfo);
   virtual int parseGenres (const QString& sResp, QVector<cparser::SGenre>& vGenres);
   virtual int parseVodManager (const QString& sResp, QVector<cparser::SVodRate>& vRates);
   virtual int parseEpgCurrent (const QString& sResp, QCurrentMap &currentEpg);

protected:
   int parseGroups (QXmlStreamReader &xml, QVector<cparser::SChan> &chanList, bool bFixTime);
   int parseChannels(QXmlStreamReader &xml, QVector<cparser::SChan> &chanList, bool bFixTime);
   int parseStreamParams (QXmlStreamReader &xml, QVector<cparser::STimeShift>& vTs);

private:
   QString     sImgTmplTv;
   QString     sImgTmplRadio;

};

#endif // __20130319_CRODNOEPARSER_H
