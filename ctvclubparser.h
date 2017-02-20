/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     ctvclubparser.h
 *
 *  @author   Jo2003
 *
 *  @date     15.01.2017
 *
 *  $Id$
 *
 *///------------------------- (c) 2017 by Jo2003  --------------------------
#ifndef __20170115_CTVCLUBPARSER_H
   #define __20170115_CTVCLUBPARSER_H

#include "cstdjsonparser.h"

class CSettingsDlg;

//---------------------------------------------------------------------------
//! \class   CTVClubParser
//! \date    15.01.2017
//! \author  Jo2003
//! \brief   parser for iptv api with json responses (tvclub)
//---------------------------------------------------------------------------
class  CTVClubParser : public CStdJsonParser
{
   Q_OBJECT

public:
   CTVClubParser(QObject * parent = 0);

   // new functions for use with API ...
   virtual int parseCookie (const QString &sResp, QString &sCookie, cparser::SAccountInfo &sInf);
   virtual int parseChannelList (const QString &sResp, QVector<cparser::SChan> &chanList, bool bFixTime);
   virtual int parseChannelGroups(const QString &sResp, QGrpMap &chanGroups);
   virtual int parseUrl(const QString &sResp, QString &sUrl);
   virtual int parseEpg (const QString &sResp, QVector<cparser::SEpg> &epgList);
   virtual int parseSetting(const QString& sResp, const QString &sName, QVector<int>& vValues, int& iActVal);
   virtual int parseSServersLogin (const QString& sResp, QVector<cparser::SSrv>& vSrv, QString& sActIp);
   virtual int parseVodList (const QString& sResp, QVector<cparser::SVodVideo>& vVodList, cparser::SGenreInfo &gInfo);
   virtual int parseVideoInfo (const QString& sResp, cparser::SVodVideo &vidInfo);
   virtual int parseEpgCurrent (const QString& sResp, QCurrentMap &currentEpg);
   virtual int parseSettings(const QString& sResp, CSettingsDlg& settings);
   virtual int addChanInfo(QEpgChanInfMap& epgChanInf);

protected:
   QEpgChanInfMap mEpgChanInf;
};

#endif // __20170115_CTVCLUBPARSER_H
