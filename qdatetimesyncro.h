/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qdatetimesyncro.h
 *
 *  @author   Jo2003
 *
 *  @date     27.05.2014
 *
 *  $Id$
 *
 *///------------------------- (c) 2014 by Jo2003  --------------------------
#ifndef __20140527_QDATETIMESYNCRO_H
   #define __20140527_QDATETIMESYNCRO_H

#include <QDateTime>

// internal value for internal timeshift value usage ...
// most commonly unused ...
#define __INTERN_TS  -2899

//---------------------------------------------------------------------------
//! \class   QDateTimeSyncro
//! \date    27.05.2014
//! \author  Jo2003
//! \brief   date time syncronized with API server
//---------------------------------------------------------------------------
class QDateTimeSyncro : public QDateTime
{
public:
   QDateTimeSyncro ();
   QDateTimeSyncro (const QDate & date);
   QDateTimeSyncro (const QDate & date, const QTime & time, Qt::TimeSpec spec = Qt::LocalTime);
   QDateTimeSyncro (const QDateTime & other);
   virtual ~QDateTimeSyncro ();

   // property functions for offset ...
   void setOffset(int iDelta);
   int  offset();

   // property functions for timeshift ...
   void setTimeShift(int iTs);
   int  timeShift();

   uint syncronizedTime_t() const;
   QDateTime currentDateTimeSync();
   uint tsToGmt(uint uit, int iTs = __INTERN_TS);
   uint gmtToTs(uint uit, int iTs = __INTERN_TS);
   QString gmtToTsFormatted(uint uit, const QString &format, int iTs = __INTERN_TS);
   QString tsToGmtFormatted(uint uit, const QString &format, int iTs = __INTERN_TS);

private:
   int _iOffset;
   int _iTimeShift;
};

#endif // __20140527_QDATETIMESYNCRO_H
