/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/qhlscontrol.cpp $
 *
 *  @file     qhlscontrol.cpp
 *
 *  @author   Jo2003
 *
 *  @date     16.12.2013
 *
 *  $Id: qhlscontrol.cpp 1277 2013-12-20 10:46:06Z Olenka.Joerg $
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#include "qhlscontrol.h"
#include "api_inc.h"
#include "clogfile.h"

// global client api classes ...
extern ApiClient *pApiClient;

// for logging ...
extern CLogFile VlcLog;

// global folders ...
extern CDirStuff *pFolders;

//---------------------------------------------------------------------------
//
//! \brief   constructs QHlsControl object
//
//! \author  Jo2003
//! \date    16.12.2013
//
//! \param   parent pointer to parent object
//
//! \return  --
//---------------------------------------------------------------------------
QHlsControl::QHlsControl(QObject *parent) :
   QObject(parent)
{
   // create playlist parser ...
   _pM3uParser    = new QExtM3uParser(this);

   _iBytesWritten =      -1;
   _iBandWidth    = 2048000; // 2Mbit
   _iBuffSecs     =      -1;

   // we aren't playing HLS so far ...
   _bGo           = false;
   _bPlay         = false;

   _tReloadPl.setSingleShot(true);
   connect (&_tReloadPl, SIGNAL(timeout()), this, SLOT(slotNewMediaPl()));
}

//---------------------------------------------------------------------------
//
//! \brief   destroys QHlsControl object
//
//! \author  Jo2003
//! \date    16.12.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
QHlsControl::~QHlsControl()
{
   if (_fVlcFifo.isOpen())
   {
      _fVlcFifo.close();
   }

   if (QFile::exists(DEF_STREAM_FIFO))
   {
      QFile::remove(DEF_STREAM_FIFO);
   }

   if (_tReloadPl.isActive())
   {
      _tReloadPl.stop();
   }
}

//---------------------------------------------------------------------------
//
//! \brief   handle playlist response from API server
//
//! \author  Jo2003
//! \date    16.12.2013
//
//! \param   id (int) response id playlist
//! \param   resp (const QString&) playlist content
//
//! \return  --
//---------------------------------------------------------------------------
void QHlsControl::slotM3uResp(int id, const QString& resp)
{
   Q_UNUSED(id)
#ifdef __TRACE
   mInfo(tr("Playlist content: ->\n ==8<==8<==8<==\n%1\n ==8<==8<==8<==").arg(resp));
#endif // __TRACE

   int iRet = 0;

   if (_bGo)
   {
      m3u::t_type plType = _pM3uParser->plType(resp);

      if (plType == m3u::M3U_MASTER_PL)
      {
         m3u::StreamVector sVec;

         if (!_pM3uParser->getStreams(resp, sVec))
         {
            if (!sVec.empty())
            {
               // also if we have more than one entry here, we'll grab the 1st one ...
               _sMasterPlUri = sVec.at(0).sUri;

               mInfo(tr("Using stream playlist %5 (id: %1, bandwidth: %2, size: %3x%4)")
                     .arg(sVec.at(0).iId)
                     .arg(sVec.at(0).iBandWidth)
                     .arg(sVec.at(0).szRes.width())
                     .arg(sVec.at(0).szRes.height())
                     .arg(_sMasterPlUri));

               _iBandWidth = sVec.at(0).iBandWidth;

               pApiClient->q_get((int)m3u::M3U_MEDIA_PL, _sMasterPlUri, Iptv::m3u);
            }
         }
      }
      else if(plType == m3u::M3U_MEDIA_PL)
      {
         iRet = _pM3uParser->getStreamToks(resp, _stVec);

         if (!iRet)
         {
            if (!_stVec.empty())
            {
               // trigger reload of media playlist ...
               if (!_pM3uParser->endList())
               {
                  _tReloadPl.start(1000 * _pM3uParser->targetDuration());
               }

               mInfo(tr("Downloading HLS stream token %1").arg(_stVec.at(0).sUri));

               pApiClient->q_get((int)m3u::M3U_HLS_TOK, _stVec.at(0).sUri, Iptv::hls);
               _stVec.remove(0);
            }
         }
         else if (iRet == 1)
         {
            // no new elements ...
            _tReloadPl.start(1000 * _pM3uParser->targetDuration() / 2);
         }
      }
   }
}

//---------------------------------------------------------------------------
//
//! \brief   handle stzream tok response from API server
//
//! \author  Jo2003
//! \date    16.12.2013
//
//! \param   id (int) response id playlist
//! \param   tok (const QByteArray&) stream data
//
//! \return  --
//---------------------------------------------------------------------------
void QHlsControl::slotStreamTokResp (int id, const QByteArray& tok)
{
   Q_UNUSED(id)

   qint64 i64BytesWritten = 0;

   if (_bGo)
   {
      if (_fVlcFifo.isOpen())
      {
         mInfo(tr("Writing %1 bytes HLS stream data to '%2'").arg(tok.size()).arg(_fVlcFifo.fileName()));

         i64BytesWritten = _fVlcFifo.write(tok);

         if (i64BytesWritten == (qint64)-1)
         {
            mInfo(tr("HLS queue-in error ..."));
         }
         else
         {
            mInfo(tr("Written %1 of %2 bytes!").arg(i64BytesWritten).arg(tok.size()));
            _iBytesWritten += i64BytesWritten;
         }

         // wait to have enough buffer before start to play ...
         if (!_bPlay && (_iBytesWritten >= ((_iBandWidth / 8) * _iBuffSecs)))
         {
            _bPlay = true;
            emit sigPlay(_fVlcFifo.fileName());
         }
      }

      // request next token ...
      if (!_stVec.empty())
      {
         mInfo(tr("Downloading HLS stream token %1").arg(_stVec.at(0).sUri));
         pApiClient->q_get((int)m3u::M3U_HLS_TOK, _stVec.at(0).sUri, Iptv::hls);
         _stVec.remove(0);
      }
   }
}

//---------------------------------------------------------------------------
//
//! \brief   get new media playlist
//
//! \author  Jo2003
//! \date    16.12.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QHlsControl::slotNewMediaPl()
{
   mInfo(tr("Downloading next media playlist %1").arg(_sMasterPlUri));
   pApiClient->q_get((int)m3u::M3U_MEDIA_PL, _sMasterPlUri, Iptv::m3u);
}

//---------------------------------------------------------------------------
//
//! \brief   start HLS play with getting the master playlist
//
//! \author  Jo2003
//! \date    16.12.2013
//
//! \param   sUrl (const QString&) url of master playlist
//
//! \return  --
//---------------------------------------------------------------------------
void QHlsControl::startHls(const QString &sUrl, int iBuffSec, const QString &sPath)
{
   // start HLS play ...
   _bGo       = true;
   _bPlay     = false;
   _iBuffSecs = iBuffSec;

   if (_fVlcFifo.isOpen())
   {
      _fVlcFifo.close();
   }

   if (QFile::exists(DEF_STREAM_FIFO))
   {
      QFile::remove(DEF_STREAM_FIFO);
   }

   if (_tReloadPl.isActive())
   {
      _tReloadPl.stop();
   }

   _fVlcFifo.setFileName(sPath.isEmpty() ? DEF_STREAM_FIFO : sPath);
   _fVlcFifo.open(QIODevice::WriteOnly);
   _iBytesWritten = 0;

   mInfo(tr("Starting HLS play from master playlist %1").arg(sUrl));

   // in case we haven't a master playlist, but only media playlist
   // we store this url to be used on playlist reaload ...
   _sMasterPlUri = sUrl;

   _pM3uParser->reset();
   _pM3uParser->setMasterUrl(sUrl);
   pApiClient->q_get((int)m3u::M3U_MASTER_PL, sUrl, Iptv::m3u);
}

//---------------------------------------------------------------------------
//
//! \brief   stop HLS play - breaking event loop
//
//! \author  Jo2003
//! \date    16.12.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QHlsControl::stop()
{
   mInfo(tr("Stopping event loop ..."));
   _bGo = false;

   if (_fVlcFifo.isOpen())
   {
      _fVlcFifo.close();
   }
///////////////////////////////////////////////////////////////
   if (_fVlcFifo.fileName() == DEF_STREAM_FIFO)
   {
      QFile::remove(DEF_STREAM_FIFO);
   }
///////////////////////////////////////////////////////////////
   if (_tReloadPl.isActive())
   {
      _tReloadPl.stop();
   }
}

//---------------------------------------------------------------------------
//
//! \brief   get duration of whole media playlist
//
//! \author  Jo2003
//! \date    16.12.2013
//
//! \param   --
//
//! \return  duration in seconds
//---------------------------------------------------------------------------
int QHlsControl::mediaPlDuration()
{
   int iRet = 0;

   for (int i = 0; i < _stVec.count(); i++)
   {
      iRet += _stVec.at(i).iDuration;
   }

   return iRet;
}

//---------------------------------------------------------------------------
//
//! \brief   check if hls download is active
//
//! \author  Jo2003
//! \date    19.12.2013
//
//! \param   --
//
//! \return  true -> active; false -> inactive
//---------------------------------------------------------------------------
bool QHlsControl::isActive()
{
   return _bGo;
}
