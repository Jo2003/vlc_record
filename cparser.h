/*=============================================================================\
| $HeadURL$
|
| Author: Jo2003
|
| last changed by: $Author$
|
| Begin: 25.03.2013
|
| $Id$
|
\=============================================================================*/
#ifndef __20130325_CPARSER_H
   #define __20130325_CPARSER_H

#include <QVector>
#include <QString>
#include <QMap>

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
      QString sName;
      QString sProgramm;
      QString sIcon;
      uint    uiStart;
      uint    uiEnd;
      int     iId;
      bool    bIsVideo;
      bool    bIsProtected;
      bool    bHasArchive;
      bool    bIsGroup;
      bool    bIsHidden;
      QVector<cparser::STimeShift> vTs;
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
      QString sExpires;
      bool    bHasArchive;
      bool    bHasVOD;
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

#endif // __20130325_CPARSER_H
