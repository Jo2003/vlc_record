/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 15.06.2010 / 16:41:34
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#ifndef __150610_CSHOWINFO_H
   #define __150610_CSHOWINFO_H

#include <QString>
#include <QMap>
#include <QObject>
#include "playstates.h"
#include "cepgbrowser.h"

typedef QMap<uint, epg::SShow> t_EpgMap;

namespace ShowInfo
{
   //----------------------------------------------
   /// \brief which kind / type of program is it
   //----------------------------------------------
   enum eProgType
   {
      Live,      ///< live stream
      Archive,   ///< archive
      VOD,       ///< video on demand
      Unknown    ///< unknown type
   };
}

/********************************************************************\
|  Class: CShowInfo
|  Date:  15.06.2010 / 14:02:24
|  Author: Jo2003
|  Description: a simple storage class for show information
|
\********************************************************************/
class CShowInfo : public QObject
{
   Q_OBJECT

public:

   // constructor ...
   CShowInfo(QObject *parent = 0);

   // clean showInfo ...
   void cleanShowInfo();

   // save values ...
   void setShowName (const QString &name);
   void setChanName (const QString &name);
   void setShowType (ShowInfo::eProgType type);
   void setChanId (int id);
   void setStartTime (uint start);
   void setEndTime (uint end);
   void setPlayState (IncPlay::ePlayStates state);
   void setLastJumpTime (uint start);
   void setHtmlDescr (const QString &descr);
   void setAdUrl(const QString &adUrl);
   void setPCode (const QString &code);
   void setVodId (int id);
   void setEpgMap(const t_EpgMap &map);
   void useStreamLoader (bool bUse);
   void useHls (bool bUse);
   void setEpgUpdTime (ulong ts);
   void updWithChanEntry (ulong ulTime, const cparser::SChan &entry);
   void setDefAStream(int idx);
   void setNoAd (bool a);

   // get values ...
   const QString &showName();
   const QString &chanName();
   const QString &htmlDescr();
   const QString &adUrl();
   const QString &pCode();
   const int &channelId();
   const uint &starts();
   const uint &ends();
   const uint &lastJump();
   const IncPlay::ePlayStates &playState();
   const ShowInfo::eProgType &showType();
   bool  canCtrlStream();
   bool  streamLoader();
   bool  isHls();
   const int& vodId();
   ulong epgUpdTime();
   int   autoUpdate(uint uiTime);
   const int& defAStream();
   bool noAd();

signals:
   void sigHtmlDescr (const QString& s);

private:
   QString              sShowName;
   int                  iChannelId;
   QString              sChanName;
   uint                 uiStart;
   uint                 uiEnd;
   uint                 uiJumpTime;
   IncPlay::ePlayStates ePlayState;
   ShowInfo::eProgType  eShowType;
   QString              sDescr;
   QString              sAdUrl;
   QString              sPCode;
   int                  iVodId;
   t_EpgMap             epgMap;
   bool                 bStreamLoader;
   bool                 bHls;
   bool                 bNoAd;
   ulong                ulLastEpgUpd;
   int                  iDefAStream;
};

#endif // __150610_CSHOWINFO_H
/************************* History ***************************\
| $Log$
\*************************************************************/
