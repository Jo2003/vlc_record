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
         bIsHidden(false){}
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
      QVector<cparser::STimeShift> vTs;
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
      QString sPgRating;
      QString sImdbRating;
      QString sKinopoiskRating;
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
      bool      bHasIVI;
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

   struct ServiceSettings {
      ServiceSettings():timeShift(-1), bitrate(-1), buffering(-1), stats(0), handled(false){}
      QString login;
      QString pass;
      QString apiServer;
      QString strServer;
      int     timeShift;
      int     bitrate;
      int     buffering;
      int     stats;
      bool    handled;
   };

   /// \brief speed test data structure
   struct SSpeedTest {
       QString ip;
       QString descr;
       QString url;
   };

   /// \brief hold description data for stream standards
   struct StrStdDescr {
       QString sName;
       QString sDescr;
       bool    bDefault;
   };

   ///
   /// \brief The SIviInfo struct
   ///
   struct SIviInfo {
       QString url;     ///< ivi start video
       bool status;     ///< ivi stratus
       QString ivi_id;  ///< ivi id (session)
   };

   /// \brief define type for stream standards
   typedef QMap<QString, StrStdDescr> QStrStdMap;

   /// \brief define type for bit rates
   typedef QMap<int, QString> QBitratesMap;
}

// make life easier ...
typedef QMap<int, cparser::SChan>                 QChanMap;
typedef QMap<int, QVector<cparser::SEpgCurrent> > QCurrentMap;
typedef QVector<cparser::SGrp>                    QGrpVector;
typedef QVector<cparser::SChan>                   QChanList;
typedef QMap<QString, QString>                    QVodLangMap;
typedef QVector<cparser::SSpeedTest>              QSpeedDataVector;

#endif // __20130325_CPARSER_H
