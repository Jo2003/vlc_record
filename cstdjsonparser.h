/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/cstdjsonparser.h $
 *
 *  @file     cstdjsonparser.h
 *
 *  @author   Jo2003
 *
 *  @date     15.04.2013
 *
 *  $Id: cstdjsonparser.h 1291 2014-01-28 15:03:17Z Olenka.Joerg $
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#ifndef __20130415_CSTDJSONPARSER_H
   #define __20130415_CSTDJSONPARSER_H

#include <QDateTime>
#include <QtJson>
#include <QVariantMap>

#include "clogfile.h"
#include "defdef.h"
#include "cparser.h"
#include "capiparser.h"

//---------------------------------------------------------------------------
//! \class   CStdJsonParser
//! \date    15.04.2013
//! \author  Jo2003
//! \brief   parser for iptv api with json responses
//---------------------------------------------------------------------------
class CStdJsonParser : public CApiParser
{
   Q_OBJECT

public:
   CStdJsonParser(QObject * parent = 0);

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
   virtual int parseError (const QString& sResp, QString& sMsg, int& eCode);
   virtual int parseVodLang(const QString &sResp, QVodLangMap &lMap);
};

#endif // __20130415_CSTDJSONPARSER_H
