/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     cstdjsonparser.h
 *
 *  @author   Jo2003
 *
 *  @date     15.04.2013
 *
 *  $Id$
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#ifndef __20130415_CSTDJSONPARSER_H
   #define __20130415_CSTDJSONPARSER_H

#include <QXmlStreamReader>
#include <QDateTime>
#include <QStringList>
#include <QJson/Parser>
#include <QVariantMap>

#include "clogfile.h"
#include "defdef.h"
#include "cparser.h"

//---------------------------------------------------------------------------
//! \class   CStdJsonParser
//! \date    15.04.2013
//! \author  Jo2003
//! \brief   parser for iptv api with json responses
//---------------------------------------------------------------------------
class CStdJsonParser : public QObject
{
   Q_OBJECT

public:
   CStdJsonParser(QObject * parent = 0);
   int fixTime (uint &uiTime);
   int GetFixTime () { return iOffset; }

   // new functions for use with API ...
   int parseCookie (const QString &sResp, QString &sCookie, cparser::SAccountInfo &sInf);
   int parseTimeShift (const QString &sResp, QVector<int> &vValues, int &iShift);
   int parseChannelList (const QString &sResp, QVector<cparser::SChan> &chanList, bool bFixTime);
   int parseEpg (const QString &sResp, QVector<cparser::SEpg> &epgList);
   int parseSetting(const QString& sResp, const QString &sName, QVector<int>& vValues, int& iActVal);
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
   void initChanEntry (cparser::SChan &entry, bool bIsChan = true);

private:
   int iOffset;

signals:
   void sigError(int iType, const QString& cap, const QString& descr);
};

#endif // __20130415_CSTDJSONPARSER_H
