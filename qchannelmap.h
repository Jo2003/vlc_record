/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qchannelmap.h
 *
 *  @author   Jo2003
 *
 *  @date     10.10.2013
 *
 *  $Id$
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#ifndef __20131010_QCHANNELMAP_H
   #define __20131010_QCHANNELMAP_H

#include <QMap>
#include <QMutex>

#include "cparser.h"

//---------------------------------------------------------------------------
//! \class   QChannelMap
//! \date    10.10.2013
//! \author  Jo2003
//! \brief   used as global channel map with mutex locking support
//---------------------------------------------------------------------------
class QChannelMap : public QChanMap
{
public:
   explicit QChannelMap();
   ~QChannelMap();
   int fillFromChannelList(const QChanList& chanList);
   bool contains(int key, bool bLock = false);
   int timeShift(int key, bool bLock = true);
   const cparser::SChan value(int key, bool bLock = false);
   int entry(int key, cparser::SChan &chan, bool bLock = true);
   void lock();
   void unlock();
   const QGrpVector& groupVector();
   void update(int key, const cparser::SChan& chan, bool bLock = true);

private:
   QMutex     _mtx;
   QGrpVector _grpVector;
};

#endif // __20131010_QCHANNELMAP_H
