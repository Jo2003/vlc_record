/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qwatchstats.h
 *
 *  @author   Jo2003
 *
 *  @date     15.10.2014
 *
 *  $Id$
 *
 *///------------------------- (c) 2014 by Jo2003  --------------------------
#ifndef __20141015_QWATCHSTATS_H
   #define __20141015_QWATCHSTATS_H

#include <QObject>
#include <QString>
#include <QList>
#include <QtJson>
#include "cshowinfo.h"
#include "csettingsdlg.h"

/// \brief create a namespace for statistics
namespace WStats {

   /// \brief object for statistic entry
   typedef QtJson::JsonObject StatsEntry;

   /// \brief define a type for a list of statistic entries
   typedef QtJson::JsonArray  EntryList;

   class QWatchStats;
}

//---------------------------------------------------------------------------
//! \class   QWatchStats
//! \date    15.10.2014
//! \author  Jo2003
//! \brief   class to collect watch statistics
//---------------------------------------------------------------------------
class QWatchStats : public QObject
{
   Q_OBJECT

public:
   QWatchStats(QObject *pParent = NULL);
   virtual ~QWatchStats();

   void setSettings(CSettingsDlg *pSet);
   void playStarts(const QString& sUrl);
   void playEnds(int iErrCount);
   QString serialize(const QString& devId);
   QString osVersion();


private:
   WStats::EntryList  mEntryList;
   WStats::StatsEntry mEntry;
   CSettingsDlg*      mpSettings;
   bool               mbStartSet;
};

#endif // __20141015_QWATCHSTATS_H
