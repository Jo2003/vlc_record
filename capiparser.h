/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     capiparser.h
 *
 *  @author   Jo2003
 *
 *  @date     17.04.2013
 *
 *  $Id$
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#ifndef __20130417_CAPIPARSER_H
   #define __20130417_CAPIPARSER_H

#include <QXmlStreamReader>
#include <QDateTime>
#include <QStringList>

#include "clogfile.h"
#include "defdef.h"
#include "cparser.h"

//---------------------------------------------------------------------------
//! \class   CApiParser
//! \date    17.04.2013
//! \author  Jo2003
//! \brief   parent class for all IPTV api parser
//---------------------------------------------------------------------------
class CApiParser : public QObject
{
   Q_OBJECT

public:
   CApiParser(QObject * parent = 0);
   virtual ~CApiParser();
   virtual int fixTime (uint &uiTime);
   virtual int GetFixTime ();

   // new functions for use with API ...
   virtual int parseCookie (const QString &sResp, QString &sCookie, cparser::SAccountInfo &sInf) = 0;
   virtual int parseChannelList (const QString &sResp, QVector<cparser::SChan> &chanList, bool bFixTime) = 0;
   virtual int parseEpg (const QString &sResp, QVector<cparser::SEpg> &epgList) = 0;
   virtual int parseSetting(const QString& sResp, const QString &sName, QVector<int>& vValues, int& iActVal) = 0;
   virtual int parseSServersLogin (const QString& sResp, QVector<cparser::SSrv>& vSrv, QString& sActIp) = 0;
   virtual int parseVodList (const QString& sResp, QVector<cparser::SVodVideo>& vVodList, cparser::SGenreInfo &gInfo) = 0;
   virtual int parseUrl (const QString& sResp, QString& sUrl) = 0;
   virtual int parseVodUrls (const QString& sResp, QStringList& sUrls) = 0;
   virtual int parseVideoInfo (const QString& sResp, cparser::SVodVideo &vidInfo) = 0;
   virtual int parseGenres (const QString& sResp, QVector<cparser::SGenre>& vGenres) = 0;
   virtual int parseVodManager (const QString& sResp, QVector<cparser::SVodRate>& vRates) = 0;
   virtual int parseEpgCurrent (const QString& sResp, QCurrentMap &currentEpg) = 0;
   virtual int parseError (const QString& sResp, QString& sMsg, int& eCode) = 0;
   virtual int parseUpdInfo(const QString& sResp, cparser::SUpdInfo &updInfo);
   virtual int handleTsStuff(QVector<cparser::SChan> &chanList);
   virtual int parseVodLang(const QString &sResp, QVodLangMap &lMap);

protected:
   virtual void checkTimeOffSet (const uint &uiSrvTime);
   virtual void initChanEntry (cparser::SChan &entry, bool bIsChan = true);
   virtual void checkColor (QString& ccode, int idx);
   virtual bool ignoreGroup(cparser::SChan& grpEntry);

private:
   int         iOffset;
   QStringList slAltColors;

signals:
   void sigError(int iType, const QString& cap, const QString& descr);
};

#endif // __20130417_CAPIPARSER_H
