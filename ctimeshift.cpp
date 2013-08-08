/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     ctimeshift.cpp
 *
 *  @author   Jo2003
 *
 *  @date     07.08.2013
 *
 *  $Id$
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#include "ctimeshift.h"
#include <QDateTime>

//---------------------------------------------------------------------------
//
//! \brief   constructs CTimeShift object
//
//! \author  Jo2003
//! \date    07.08.2013
//
//! \param   parent (QObject *) pointer to parent object
//
//! \return  --
//---------------------------------------------------------------------------
CTimeShift::CTimeShift(QObject *parent) :
   QObject(parent), _ts(0)
{
   // nothing to do so far ...
}

//---------------------------------------------------------------------------
//
//! \brief   destroys CTimeShift object
//
//! \author  Jo2003
//! \date    07.08.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
CTimeShift::~CTimeShift()
{
   // nothing to do so far ...
}

//---------------------------------------------------------------------------
//
//! \brief   get time shift value
//
//! \author  Jo2003
//! \date    07.08.2013
//
//! \param   --
//
//! \return  timeshift in hours
//---------------------------------------------------------------------------
int CTimeShift::timeShift()
{
   return _ts;
}

//---------------------------------------------------------------------------
//
//! \brief   set time shift value
//
//! \author  Jo2003
//! \date    07.08.2013
//
//! \param   i (int) new time shift value in hours
//
//! \return  --
//---------------------------------------------------------------------------
void CTimeShift::setTimeShift(int i)
{
   _ts = i;
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
uint CTimeShift::fromGmt(uint uit, int iTs)
{
   qint64 i64 = uit;

   if (iTs == INTERN_TS)
   {
      iTs = _ts;
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
uint CTimeShift::toGmt(uint uit, int iTs)
{
   qint64 i64 = uit;

   if (iTs == INTERN_TS)
   {
      iTs = _ts;
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
QString CTimeShift::fromGmtFormatted(uint uit, const QString &format, int iTs)
{
   return QDateTime::fromTime_t(fromGmt(uit, iTs)).toString(format);
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
QString CTimeShift::toGmtFormatted(uint uit, const QString &format, int iTs)
{
   return QDateTime::fromTime_t(toGmt(uit, iTs)).toString(format);
}
