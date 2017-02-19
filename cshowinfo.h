/*********************** Information *************************\
| $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/cshowinfo.h $
|
| Author: Jo2003
|
| Begin: 15.06.2010 / 16:41:34
|
| Last edited by: $Author: Olenka.Joerg $
|
| $Id: cshowinfo.h 1267 2013-12-17 13:55:47Z Olenka.Joerg $
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
   void setLangCode(const QString& lcode);
   void setEndOfLife(uint gmt);
   void setChanGrp(int i);

   // get values ...
   const QString &showName() const;
   const QString &chanName() const;
   const QString &htmlDescr() const;
   const QString &adUrl() const;
   const QString &pCode() const;
   const int &channelId() const;
   const int &chanGrp() const;
   const uint &starts() const;
   const uint &ends() const;
   const uint &lastJump() const;
   const uint &endOfLife() const;
   const IncPlay::ePlayStates &playState() const;
   const ShowInfo::eProgType &showType() const;
   bool  canCtrlStream() const;
   bool  streamLoader() const;
   bool  isHls() const;
   const int& vodId() const;
   ulong epgUpdTime() const;
   int   autoUpdate(uint uiTime);
   const int& defAStream() const;
   const QString& langCode() const;

private:
   QString              sShowName;
   int                  iChannelId;
   int                  iChanGrp;
   QString              sChanName;
   uint                 uiStart;
   uint                 uiEnd;
   uint                 uiJumpTime;
   uint                 uiEOL;
   IncPlay::ePlayStates ePlayState;
   ShowInfo::eProgType  eShowType;
   QString              sDescr;
   QString              sAdUrl;
   QString              sPCode;
   int                  iVodId;
   t_EpgMap             epgMap;
   bool                 bStreamLoader;
   bool                 bHls;
   ulong                ulLastEpgUpd;
   int                  iDefAStream;
   QString              sLangCode;
};

#endif // __150610_CSHOWINFO_H
/************************* History ***************************\
| $Log$
\*************************************************************/
