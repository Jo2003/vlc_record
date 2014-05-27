/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qdatetimesyncro.cpp
 *
 *  @author   Jo2003
 *
 *  @date     27.05.2014
 *
 *  $Id$
 *
 *///------------------------- (c) 2014 by Jo2003  --------------------------
#include "qdatetimesyncro.h"

//---------------------------------------------------------------------------
//
//! \brief   constructor
//
//! \author  Jo2003
//! \date    27.05.2014
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
QDateTimeSyncro::QDateTimeSyncro ()
   : QDateTime(), _iOffset(0), _iTimeShift(0)
{

}

//---------------------------------------------------------------------------
//
//! \brief   constructor
//
//! \author  Jo2003
//! \date    27.05.2014
//
//! \param   date (const QDate &) date to construct from
//
//! \return  --
//---------------------------------------------------------------------------
QDateTimeSyncro::QDateTimeSyncro (const QDate & date)
   : QDateTime(date), _iOffset(0), _iTimeShift(0)
{

}

//---------------------------------------------------------------------------
//
//! \brief   constructor
//
//! \author  Jo2003
//! \date    27.05.2014
//
//! \param   date (const QDate &) date to construct from
//! \param   time (const QTime &) time to construct from
//! \param   spec (Qt::TimeSpec) time specification
//
//! \return  --
//---------------------------------------------------------------------------
QDateTimeSyncro::QDateTimeSyncro (const QDate & date, const QTime & time, Qt::TimeSpec spec)
   : QDateTime(date, time, spec), _iOffset(0), _iTimeShift(0)
{

}

//---------------------------------------------------------------------------
//
//! \brief   constructor
//
//! \author  Jo2003
//! \date    27.05.2014
//
//! \param   other (const QDateTime &) datetime to construct from
//
//! \return  --
//---------------------------------------------------------------------------
QDateTimeSyncro::QDateTimeSyncro (const QDateTime & other)
   : QDateTime(other), _iOffset(0), _iTimeShift(0)
{

}

//---------------------------------------------------------------------------
//
//! \brief   destructor
//
//! \author  Jo2003
//! \date    27.05.2014
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
QDateTimeSyncro::~QDateTimeSyncro ()
{

}

//---------------------------------------------------------------------------
//
//! \brief   set offset between server and computer
//
//! \author  Jo2003
//! \date    27.05.2014
//
//! \param   iDelta (int) offset in seconds
//
//! \return  --
//---------------------------------------------------------------------------
void QDateTimeSyncro::setOffset(int iDelta)
{
   _iOffset = iDelta;
}

//---------------------------------------------------------------------------
//
//! \brief   get offset
//
//! \author  Jo2003
//! \date    27.05.2014
//
//! \param   --
//
//! \return  offset in seconds
//---------------------------------------------------------------------------
int QDateTimeSyncro::offset()
{
   return _iOffset;
}

//---------------------------------------------------------------------------
//
//! \brief   set timeshift in hours
//
//! \author  Jo2003
//! \date    27.05.2014
//
//! \param   iTs (int) timeshift in hours
//
//! \return  --
//---------------------------------------------------------------------------
void QDateTimeSyncro::setTimeShift(int iTs)
{
   _iTimeShift = iTs;
}

//---------------------------------------------------------------------------
//
//! \brief   get timeshift
//
//! \author  Jo2003
//! \date    27.05.2014
//
//! \param   --
//
//! \return  timeshift in hours
//---------------------------------------------------------------------------
int QDateTimeSyncro::timeShift()
{
   return _iTimeShift;
}

//---------------------------------------------------------------------------
//
//! \brief   get current unix timestamp including offset to server time
//
//! \author  Jo2003
//! \date    27.05.2014
//
//! \param   --
//
//! \return  unix timestamp
//---------------------------------------------------------------------------
uint QDateTimeSyncro::syncronizedTime_t() const
{
   return QDateTime::currentDateTime().toTime_t() + _iOffset;
}

//---------------------------------------------------------------------------
//
//! \brief   get current date time including offset to server time
//
//! \author  Jo2003
//! \date    27.05.2014
//
//! \param   --
//
//! \return  date time syncronized with server time
//---------------------------------------------------------------------------
QDateTime QDateTimeSyncro::currentDateTimeSync()
{
   return QDateTime::currentDateTime().addSecs(_iOffset);
}

//---------------------------------------------------------------------------
//
//! \brief   convert gmt to timeshift value
//
//! \author  Jo2003
//! \date    07.08.2013
//
//! \param   uit (uint) value to convert
//! \param   iTs (int) optional external timestamp
//
//! \return  value incl. timeshift
//---------------------------------------------------------------------------
uint QDateTimeSyncro::gmtToTs(uint uit, int iTs)
{
   qint64 i64 = uit;

   if (iTs == __INTERN_TS)
   {
      iTs = _iTimeShift;
   }

   return (uint)(i64 + (iTs * 3600));
}

//---------------------------------------------------------------------------
//
//! \brief   convert timeshift to gmt value
//
//! \author  Jo2003
//! \date    07.08.2013
//
//! \param   uit (uint) value to convert
//! \param   iTs (int) optional external timestamp
//
//! \return  gmt value
//---------------------------------------------------------------------------
uint QDateTimeSyncro::tsToGmt(uint uit, int iTs)
{
   qint64 i64 = uit;

   if (iTs == __INTERN_TS)
   {
      iTs = _iTimeShift;
   }

   return (uint)(i64 - (iTs * 3600));
}

//---------------------------------------------------------------------------
//
//! \brief   convert from gmt to timeshift with format
//
//! \author  Jo2003
//! \date    07.08.2013
//
//! \param   uit (uint) value to convert
//! \param   format (QString) format string
//! \param   iTs (int) optional external timestamp
//
//! \return  string with formatted time stamp
//---------------------------------------------------------------------------
QString QDateTimeSyncro::gmtToTsFormatted(uint uit, const QString &format, int iTs)
{
   return QDateTime::fromTime_t(gmtToTs(uit, iTs)).toString(format);
}

//---------------------------------------------------------------------------
//
//! \brief   convert from timeshift to gmt with format
//
//! \author  Jo2003
//! \date    07.08.2013
//
//! \param   uit (uint) value to convert
//! \param   format (QString) format string
//! \param   iTs (int) optional external timestamp
//
//! \return  string with formatted time stamp
//---------------------------------------------------------------------------
QString QDateTimeSyncro::tsToGmtFormatted(uint uit, const QString &format, int iTs)
{
   return QDateTime::fromTime_t(tsToGmt(uit, iTs)).toString(format);
}
