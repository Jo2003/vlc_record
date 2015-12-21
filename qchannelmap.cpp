/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/qchannelmap.cpp $
 *
 *  @file     qchannelmap.cpp
 *
 *  @author   Jo2003
 *
 *  @date     10.10.2013
 *
 *  $Id: qchannelmap.cpp 1214 2013-10-22 12:00:27Z Olenka.Joerg $
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

    cparser::SGrp grpEntry;

    clear();

    m_grpMap.clear();

    // since we may have one channel in multiple
    // groups we have to run this tvice!

    // create channel map ...
    foreach (const cparser::SChan& chan, chanList)
    {
        if (!chan.bIsGroup)
        {
            insert(chan.iId, chan);
            grpEntry.vChannels.append(chan.iId);
        }
        else
        {
            if (!grpEntry.sName.isEmpty())
            {
                m_grpMap.insert(grpEntry.iId, grpEntry);
            }

            grpEntry.iId    = chan.iId;
            grpEntry.sColor = chan.sProgramm;
            grpEntry.sName  = chan.sName;
            grpEntry.vChannels.clear();
        }
    }

    // make sure we add last group ...
    if (!grpEntry.sName.isEmpty())
    {
        m_grpMap.insert(grpEntry.iId, grpEntry);
    }

    // look again for additional grouping ...
    foreach (const cparser::SChan& chan, chanList)
    {
        if (!chan.bIsGroup && (chan.vInGroups.count() > 1))
        {
            foreach (int gid, chan.vInGroups)
            {
                if (m_grpMap.contains(gid))
                {
                    cparser::SGrp& grp = m_grpMap[gid];
                    if (!grp.vChannels.contains(chan.iId))
                    {
                        grp.vChannels.append(chan.iId);
                    }
                }
            }
        }
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
    m_mtx.lock();
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
    m_mtx.unlock();
}

//---------------------------------------------------------------------------
//
//! \brief   get access to group map
//
//! \author  Jo2003
//! \date    10.10.2013
//
//! \param   --
//
//! \return  const ref. to internal group map
//---------------------------------------------------------------------------
const QGrpMap& QChannelMap::groupMap()
{
   return m_grpMap;
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
