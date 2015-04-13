/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/cnovoeparser.h $
 *
 *  @file     cnovoeparser.h
 *
 *  @author   Jo2003
 *
 *  @date     15.04.2013
 *
 *  $Id: cnovoeparser.h 1158 2013-08-01 10:10:02Z Olenka.Joerg $
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#ifndef __20130612_CNOVOEPARSER_H
   #define __20130612_CNOVOEPARSER_H

#include "cstdjsonparser.h"

//---------------------------------------------------------------------------
//! \class   CNovoeParser
//! \date    12.06.2013
//! \author  Jo2003
//! \brief   parser for iptv api with json responses (novoe tv)
//---------------------------------------------------------------------------
class  CNovoeParser : public CStdJsonParser
{
   Q_OBJECT

public:
   CNovoeParser(QObject * parent = 0);

   // new functions for use with API ...
   virtual int parseChannelList (const QString &sResp, QVector<cparser::SChan> &chanList, bool bFixTime);
   virtual int parseEpg (const QString &sResp, QVector<cparser::SEpg> &epgList);
   virtual int parseSetting(const QString& sResp, const QString &sName, QVector<int>& vValues, int& iActVal);
   virtual int parseSServersLogin (const QString& sResp, QVector<cparser::SSrv>& vSrv, QString& sActIp);
   virtual int parseVodList (const QString& sResp, QVector<cparser::SVodVideo>& vVodList, cparser::SGenreInfo &gInfo);
   virtual int parseVideoInfo (const QString& sResp, cparser::SVodVideo &vidInfo);
   virtual int parseEpgCurrent (const QString& sResp, QCurrentMap &currentEpg);
};

#endif // __20130612_CNOVOEPARSER_H
