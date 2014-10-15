/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qchannelmap.cpp
 *
 *  @author   Jo2003
 *
 *  @date     10.10.2013
 *
 *  $Id$
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#include "qchannelmap.h"

//---------------------------------------------------------------------------
//
//! \brief   constructs QChannelMap object
//
//! \author  Jo2003
//! \date    10.10.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
QChannelMap::QChannelMap() :
   QChanMap()
{
}

//---------------------------------------------------------------------------
//
//! \brief   destroys QChannelMap
//
//! \author  Jo2003
//! \date    10.10.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
QChannelMap::~QChannelMap()
{
   // nothing to do so far ...
}

//---------------------------------------------------------------------------
//
//! \brief   fill map with values from iptv provider
//
//! \author  Jo2003
//! \date    10.10.2013
//
//! \param   chanList (const QChanList &) channel list as vector
//
//! \return  0
//---------------------------------------------------------------------------
int QChannelMap::fillFromChannelList(const QChanList &chanList)
{
   lock();

   QChanList::const_iterator cit;
   cparser::SGrp grpEntry;

   clear();
   _grpVector.clear();

   // create channel map ...
   for (cit = chanList.constBegin(); cit != chanList.constEnd(); cit++)
   {
      if (!(*cit).bIsGroup)
      {
         insert((*cit).iId, *cit);
         grpEntry.vChannels.append((*cit).iId);
      }
      else
      {
         if (!grpEntry.sName.isEmpty())
         {
            _grpVector.append(grpEntry);
         }

         grpEntry.iId    = (*cit).iId;
         grpEntry.sColor = (*cit).sProgramm;
         grpEntry.sName  = (*cit).sName;
         grpEntry.vChannels.clear();
      }
   }

   // make sure we add last group ...
   if (!grpEntry.sName.isEmpty())
   {
      _grpVector.append(grpEntry);
   }

   unlock();

   return 0;
}

//---------------------------------------------------------------------------
//
//! \brief   does list contains channel entry?
//
//! \author  Jo2003
//! \date    10.10.2013
//
//! \param   key (int) channel id
//! \param   bLock (bool) do mutex locking (optional)
//
//! \return  true -> contains; false -> doesn't contain
//---------------------------------------------------------------------------
bool QChannelMap::contains(int key, bool bLock)
{
   bool bRV;

   if (bLock) lock();

   bRV = QChanMap::contains(key);

   if (bLock) unlock();

   return bRV;
}

//---------------------------------------------------------------------------
//
//! \brief   get whole channel entry
//
//! \author  Jo2003
//! \date    10.10.2013
//
//! \param   key (int) channel id
//! \param   bLock (bool) do mutex locking (optional)
//
//! \return  channel entry
//---------------------------------------------------------------------------
const cparser::SChan QChannelMap::value(int key, bool bLock)
{
   cparser::SChan rv;

   if (bLock) lock();

   rv = QChanMap::value(key);

   if (bLock) unlock();

   return rv;
}

//---------------------------------------------------------------------------
//
//! \brief   get whole channel entry with error check
//
//! \author  Jo2003
//! \date    10.10.2013
//
//! \param   key (int) channel id
//! \param   chan (cparser::SChan &) ref. to store channel entry
//! \param   bLock (bool) do mutex locking (optional)
//
//! \return  0 -> ok; -1 -> not found
//---------------------------------------------------------------------------
int QChannelMap::entry(int key, cparser::SChan &chan, bool bLock)
{
   int iRV = 0;

   if (bLock) lock();

   if (contains(key))
   {
      chan = value(key);
   }
   else
   {
      iRV = -1;
   }

   if (bLock) unlock();

   return iRV;
}

//---------------------------------------------------------------------------
//
//! \brief   update a channel entry
//
//! \author  Jo2003
//! \date    10.10.2013
//
//! \param   key (int) channel id
//! \param   chan (const cparser::SChan &) channel data to update with
//! \param   bLock (bool) do mutex locking (optional)
//
//! \return  --
//---------------------------------------------------------------------------
void QChannelMap::update(int key, const cparser::SChan& chan, bool bLock)
{
   if (bLock) lock();
   operator [](key) = chan;
   if (bLock) unlock();
}

//---------------------------------------------------------------------------
//
//! \brief   lock channel map
//
//! \author  Jo2003
//! \date    10.10.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QChannelMap::lock()
{
   _mtx.lock();
}

//---------------------------------------------------------------------------
//
//! \brief   unlock channel map
//
//! \author  Jo2003
//! \date    10.10.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QChannelMap::unlock()
{
   _mtx.unlock();
}

//---------------------------------------------------------------------------
//
//! \brief   get access to group vector
//
//! \author  Jo2003
//! \date    10.10.2013
//
//! \param   --
//
//! \return  const ref. to internal group vector
//---------------------------------------------------------------------------
const QGrpVector& QChannelMap::groupVector()
{
   return _grpVector;
}

//---------------------------------------------------------------------------
//
//! \brief   get timeshift value for channel
//
//! \author  Jo2003
//! \date    18.10.2013
//
//! \param   key (int) channel id
//! \param   bLock (bool) do mutex locking (optional)
//
//! \return  timeshift
//---------------------------------------------------------------------------
int QChannelMap::timeShift(int key, bool bLock)
{
   int iRV = 0;

   if (bLock) lock();

   if (contains(key))
   {
      iRV = value(key).iTs;
   }

   if (bLock) unlock();

   return iRV;
}
