/*=============================================================================\
| $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/cparser.h $
|
| Author: Jo2003
|
| last changed by: $Author: Olenka.Joerg $
|
| Begin: 25.03.2013
|
| $Id: cparser.h 1495 2015-02-20 13:48:22Z Olenka.Joerg $
|
\=============================================================================*/
#ifndef __20130325_CPARSER_H
   #define __20130325_CPARSER_H

#include <QVector>
#include <QString>
#include <QMap>
#include <QDateTime>

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
      SChan():uiStart(0),uiEnd(0),uiDefAud(0),iTs(0),iId(0),bIsVideo(false),
         bIsProtected(false),bHasArchive(false),bIsGroup(false),
         bIsHidden(false),bHasTsInfo(false){}
      QString sName;
      QString sProgramm;
      QString sIcon;
      uint    uiStart;
      uint    uiEnd;
      uint    uiDefAud;
      int     iTs;
      int     iId;
      bool    bIsVideo;
      bool    bIsProtected;
      bool    bHasArchive;
      bool    bIsGroup;
      bool    bIsHidden;
      bool    bHasTsInfo;
      QVector<cparser::STimeShift> vTs;
#ifdef _TASTE_STALKER
      QString url;
#endif // __TASTE_STALKER
   };

   struct SGrp
   {
      SGrp():iId(0){}
      int          iId;
      QString      sName;
      QString      sColor;
      QVector<int> vChannels;
   };

   struct SEpg
   {
      // constructor inits values ...
      SEpg():uiGmt(0), uiEnd(0){}
      QString sName;
      QString sDescr;
      uint    uiGmt;
      uint    uiEnd;
      int     id;
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
      bool    bProtected;
      bool    bFavourit;
      QVector<cparser::SVodFileInfo> vVodFiles;
   };

   struct SAccountInfo
   {
      QString   sExpires;
      QDateTime dtExpires;
      bool      bHasArchive;
      bool      bHasVOD;
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
      int                    iBuild;
      QString                sUrl;
   };

   struct SVodRate
   {
      int     iRateID;
      QString sGenre;
      QString sAccess;
   };

   struct SEpgCurrent
   {
      uint    uiStart;
      uint    uiEnd;
      QString sShow;
   };

   struct SAuth
   {
       QString token;
       QString toktype;
       int     expires;
       QString refreshToken;
       int     userId;
   };
}

// make life easier ...
typedef QMap<int, cparser::SChan>                 QChanMap;
typedef QMap<int, QVector<cparser::SEpgCurrent> > QCurrentMap;
typedef QVector<cparser::SGrp>                    QGrpVector;
typedef QVector<cparser::SChan>                   QChanList;
typedef QMap<QString, QString>                    QVodLangMap;
typedef QMap<QString, QString>                    QStalkerSettings;

#endif // __20130325_CPARSER_H
