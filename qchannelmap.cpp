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
//! \brief     set groups map (in case channel list hasn't groups
//
//! \param[in] grps (const QGrpMap &) groups map from out side
//---------------------------------------------------------------------------
void QChannelMap::setGroupMap(const QGrpMap &grps)
{
    m_grpMap  = grps;
}

//---------------------------------------------------------------------------
//! \brief   fill map with values from iptv provider
//
//! \author  Jo2003
//! \date    10.10.2013
//
//! \param   chanList (const QChanList &) channel list as vector
//---------------------------------------------------------------------------
void QChannelMap::fillFromChannelList(const QChanList &chanList)
{
    lock();

    clear();

    // create channel map ...
    foreach (const cparser::SChan& chan, chanList)
    {
        // check group entry ...
        if (m_grpMap.contains(chan.iPrimGrp))
        {
            // check if channel id is stored there ...
            if (!m_grpMap[chan.iPrimGrp].vChannels.contains(chan.iId))
            {
                m_grpMap[chan.iPrimGrp].vChannels.append(chan.iId);
            }
        }

        // do not depend on any group ...
        insert(chan.iId, chan);
    }

    unlock();
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
//! \brief   search for channels with filter
//
//! \param[in] filter (const QString&) filter string
//
//! \return  channel list
//---------------------------------------------------------------------------
QChanList QChannelMap::filterChannels(const QString &filter)
{
    QChanList cl;

    ConstIterator cit;

    for(cit = begin(); cit != end(); cit ++)
    {
        if (filter.isEmpty() || cit->sName.contains(filter, Qt::CaseInsensitive))
        {
            cl.append(*cit);
        }
    }

    return cl;
}

//---------------------------------------------------------------------------
//
//! \brief   update channel data
//
//! \param[in] epgChanInf (const QEpgChanInfMap&) additional channel data
//
//---------------------------------------------------------------------------
void QChannelMap::updateChannelData(const QEpgChanInfMap &epgChanInf)
{
    foreach(int cid, epgChanInf.keys())
    {
        if (contains(cid))
        {
            cparser::SChan chan = value(cid);
            chan.bHasArchive    = (epgChanInf.value(cid).miArchHours > 0);
            chan.bIsProtected   = epgChanInf.value(cid).mbProtected;
            chan.iArchHours     = epgChanInf.value(cid).miArchHours;

            operator [](cid) = chan;
        }
    }
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
