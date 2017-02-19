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
      SChan():uiStart(0),uiEnd(0),uiEol(0),uiDefAud(0),iTs(0),iId(0),iPrimGrp(-1),iArchHours(0),
         bIsVideo(false), bIsProtected(false),bHasArchive(false),bIsGroup(false),
         bIsHidden(false),bHasTsInfo(false){}
      QString sName;
      QString sProgramm;
      QString sIcon;
      QString sLangCode;
      uint    uiStart;
      uint    uiEnd;
      uint    uiEol;
      uint    uiDefAud;
      int     iTs;
      int     iId;
      int     iPrimGrp;
      int     iArchHours;
      bool    bIsVideo;
      bool    bIsProtected;
      bool    bHasArchive;
      bool    bIsGroup;
      bool    bIsHidden;
      bool    bHasTsInfo;
      QVector<cparser::STimeShift> vTs;
      QVector<int> vInGroups;
   };

   struct SGrp
   {
      SGrp():iId(0), iCount(0){}
      int          iId;
      int          iCount;
      QString      sName;
      QString      sNameEn;
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
      SVodFileInfo()
          :iId(-1), iSize(0), iLength(0), iWidth(0), iHeight(0){}
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
      SVodVideo()
          :uiVidId(0), uiLength(0), uiLengthSec(0), bProtected(false), bFavourit(false){}
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
      uint    uiLengthSec;
      bool    bProtected;
      bool    bFavourit;
      QVector<cparser::SVodFileInfo> vVodFiles;
   };

   struct SAccountInfo
   {
      QString   sExpires;
      QDateTime dtExpires;
      QString   sName;
      QString   sMail;
      bool      bHasArchive;
      bool      bHasVOD;
      QMap<QString, QString> services;
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
}

// make life easier ...
typedef QMap<int, cparser::SChan>                 QChanMap;
typedef QMap<int, QVector<cparser::SEpgCurrent> > QCurrentMap;
typedef QMap<int, cparser::SGrp>                  QGrpMap;
typedef QVector<cparser::SChan>                   QChanList;
typedef QMap<QString, QString>                    QVodLangMap;

#endif // __20130325_CPARSER_H
