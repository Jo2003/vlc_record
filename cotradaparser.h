/*=============================================================================\
| $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/COtradaParser.h $
|
| Author: Jo2003
|
| last changed by: $Author: Olenka.Joerg $
|
| Begin: 19.03.2013
|
| $Id: COtradaParser.h 1252 2013-12-02 19:07:04Z Olenka.Joerg $
|
\=============================================================================*/
#ifndef __20130319_OTRADAPARSER_H
   #define __20130319_OTRADAPARSER_H

#include <QXmlStreamReader>
#include <QRegExp>
#include <QDateTime>
#include <QStringList>

#include "clogfile.h"
#include "defdef.h"
#include "cparser.h"
#include "capixmlparser.h"

/********************************************************************\
|  Class: COtradaParser
|  Date:  19.03.2013
|  Author: Jo2003
|  Description: parser for iptv sent xml content
|
\********************************************************************/
class COtradaParser : public CApiXmlParser
{
   Q_OBJECT

public:
   COtradaParser(QObject * parent = 0);

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
   virtual int parseAStreams (const QString& sResp, QStringList &sl);

protected:
   virtual int parseGroups (QXmlStreamReader &xml, QVector<cparser::SChan> &chanList, const QString& endTag, bool bFixTime);
   virtual int parseChannels(QXmlStreamReader &xml, QVector<cparser::SChan> &chanList, bool isRadio, bool bFixTime);
   virtual int parseStreamParams (QXmlStreamReader &xml, QVector<cparser::STimeShift>& vTs);

private:
   QString     sImgTmplTv;
   QString     sImgTmplRadio;

};

#endif // __20130319_OTRADAPARSER_H
