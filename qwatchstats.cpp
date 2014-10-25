/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qwatchstats.cpp
 *
 *  @author   Jo2003
 *
 *  @date     15.10.2014
 *
 *  $Id$
 *
 *///------------------------- (c) 2014 by Jo2003  --------------------------
#include "qwatchstats.h"
#include "version_info.h"
#include <QSysInfo>

// external stuuf can't be included from our include header since we
// are global as well ...
#include "qcustparser.h"
#include "qdatetimesyncro.h"

// global syncronized time ...
extern QDateTimeSyncro tmSync;

// global customization class ...
extern QCustParser *pCustomization;

// global showinfo class ...
extern CShowInfo showInfo;

//---------------------------------------------------------------------------
//
//! \brief   constructs QWatchStats object
//
//! \author  Jo2003
//! \date    15.10.2014
//
//! \param   pParent [in] (QObject *) pointer to parent object
//
//---------------------------------------------------------------------------
QWatchStats::QWatchStats(QObject *pParent) : QObject(pParent), mpSettings(NULL), mbStartSet(false)
{
}

//---------------------------------------------------------------------------
//
//! \brief   destroys QWatchStats object
//
//! \author  Jo2003
//! \date    15.10.2014
//
//---------------------------------------------------------------------------
QWatchStats::~QWatchStats()
{
}

//---------------------------------------------------------------------------
//
//! \brief   import settings class
//
//! \author  Jo2003
//! \date    15.10.2014
//
//! \param   pSet [in] (CSettingsDlg *) pointer to settings class
//
//---------------------------------------------------------------------------
void QWatchStats::setSettings(CSettingsDlg *pSet)
{
   mpSettings = pSet;
}

//---------------------------------------------------------------------------
//
//! \brief   a stream should be started, grab needed informations
//
//! \author  Jo2003
//! \date    15.10.2014
//
//! \param   sUrl [in] (const QString &) stream url
//
//---------------------------------------------------------------------------
void QWatchStats::playStarts(const QString &sUrl)
{
   mbStartSet = true;

   // clean entry ...
   mEntry.clear();

   mEntry["stream_url"]    = sUrl;
   mEntry["play_start"]    = tmSync.currentDateTimeSync().toTime_t();
   mEntry["prog_type"]     = CShowInfo::progType2String(showInfo.showType());
   mEntry["stream_server"] = mpSettings->getStreamServer();
   mEntry["buffering"]     = mpSettings->GetBufferTime();

   switch (showInfo.showType())
   {
   case ShowInfo::Archive:
      // archive needs gmt, cid, timeshift, bitrate ...
      mEntry["bitrate"]    = mpSettings->GetBitRate();
      mEntry["timeshift"]  = mpSettings->getTimeShift();
      mEntry["cid"]        = showInfo.channelId();
      mEntry["gmt"]        = showInfo.starts() + showInfo.lastJump();
      break;

   case ShowInfo::Live:
      // live needs gmt, cid, timeshift, bitrate ...
      mEntry["bitrate"]    = mpSettings->GetBitRate();
      mEntry["timeshift"]  = mpSettings->getTimeShift();
      mEntry["cid"]        = showInfo.channelId();
      mEntry["gmt"]        = -1;
      break;

   case ShowInfo::VOD:
      // vod needs movie id and file id
      mEntry["movie_id"]   = showInfo.videoId();
      mEntry["file_id"]    = showInfo.vodFileId();
      break;

   default:
      mbStartSet = false;
      break;
   }
}

//---------------------------------------------------------------------------
//
//! \brief   player ends, store stats record
//
//! \author  Jo2003
//! \date    15.10.2014
//
//! \param   iErrCount [in] (int) error count
//
//---------------------------------------------------------------------------
void QWatchStats::playEnds(int iErrCount)
{
   if (mbStartSet)
   {
      if ((tmSync.currentDateTimeSync().toTime_t() - mEntry["play_start"].toUInt()) > 10)
      {
         // more than 10 seconds play time ...
         mEntry["play_end"]    = tmSync.currentDateTimeSync().toTime_t();
         mEntry["error_count"] = iErrCount;
         mEntryList.append(mEntry);
      }
      mbStartSet = false;
   }
}

//---------------------------------------------------------------------------
//
//! \brief   JSON serialize statistics
//
//! \author  Jo2003
//! \date    15.10.2014
//
//! \param   devId [in] (const QString&) unique device id
//
//---------------------------------------------------------------------------
QString QWatchStats::serialize(const QString& devId)
{
   WStats::StatsEntry entry;
   QString fwVersion = VERSION_MAJOR "." VERSION_MINOR BETA_EXT;

   entry["server_address"]   = mpSettings->GetAPIServer();
   entry["account_number"]   = mpSettings->GetUser();
   entry["device_id"]        = devId;
   entry["device_type"]      = QString("%1%2-%3").arg(pCustomization->strVal("APPLICATION_SHORTCUT")).arg(OP_SYS).arg(SOFTID_DEVELOPER);
   entry["firmware_version"] = fwVersion;
   entry["os_version"]       = osVersion();

   if (!mEntryList.isEmpty())
   {
      entry["play_statistics"]  = mEntryList;
   }

   QString ret = QtJson::serialize(entry);
   mEntryList.clear();
   mbStartSet = false;

   return ret;
}

//---------------------------------------------------------------------------
//
//! \brief   create string with OS version
//
//! \author  Jo2003
//! \date    17.10.2014
//
//! \return  OS string
//---------------------------------------------------------------------------
QString QWatchStats::osVersion()
{
   QString os;
#ifdef Q_OS_LINUX
   os = QString("Linux (%1)").arg((QSysInfo::WordSize == 32) ? "i386" : (QSysInfo::WordSize == 64) ? "amd64" : "unknown");
#elif defined Q_OS_WIN
   switch (QSysInfo::windowsVersion())
   {
   case QSysInfo::WV_XP:
      os = QString("Windows XP (%1 bit)").arg(QSysInfo::WordSize);
      break;
   case QSysInfo::WV_VISTA:
      os = QString("Windows Vista (%1 bit)").arg(QSysInfo::WordSize);
      break;
   case QSysInfo::WV_WINDOWS7:
      os = QString("Windows 7 (%1 bit)").arg(QSysInfo::WordSize);
      break;
   case QSysInfo::WV_WINDOWS8:
      os = QString("Windows 8 (%1 bit)").arg(QSysInfo::WordSize);
      break;
   case 0x00b0: // QSysInfo::WV_WINDOWS8_1:
      os = QString("Windows 8.1 (%1 bit)").arg(QSysInfo::WordSize);
      break;
   default:
      os = QString("Windows (%1 bit)").arg(QSysInfo::WordSize);
      break;
   }
#elif defined Q_OS_MAC
   switch (QSysInfo::MacintoshVersion)
   {
   case QSysInfo::MV_10_6:
      os = QString("MacOS X 10.6 (%1 bit)").arg(QSysInfo::WordSize);
      break;
   case QSysInfo::MV_10_7:
      os = QString("MacOS X 10.7 (%1 bit)").arg(QSysInfo::WordSize);
      break;
   case QSysInfo::MV_10_8:
      os = QString("MacOS X 10.8 (%1 bit)").arg(QSysInfo::WordSize);
      break;
   case 0x000b: // QSysInfo::MV_10_9:
      os = QString("MacOS X 10.9 (%1 bit)").arg(QSysInfo::WordSize);
      break;
   case 0x000c: // yosmite
      os = QString("MacOS X 10.10 (%1 bit)").arg(QSysInfo::WordSize);
      break;
   default:
      os = QString("MacOS X (%1 bit)").arg(QSysInfo::WordSize);
      break;
   }
#else
   os = QString("Unknown (%1 bit)").arg(QSysInfo::WordSize);
#endif

   return os;
}
