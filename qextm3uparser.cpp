/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/qextm3uparser.cpp $
 *
 *  @file     qextm3uparser.cpp
 *
 *  @author   Jo2003
 *
 *  @date     12.12.2013
 *
 *  $Id: qextm3uparser.cpp 1267 2013-12-17 13:55:47Z Olenka.Joerg $
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#include "qextm3uparser.h"
#include <QRegExp>
#include <QStringList>

//---------------------------------------------------------------------------
//
//! \brief   constructs QExtM3uParser object
//
//! \author  Jo2003
//! \date    12.12.2013
//
//! \param   parent (QObject *) pointer to parent object
//
//! \return  --
//---------------------------------------------------------------------------
QExtM3uParser::QExtM3uParser(QObject *parent) :
   QObject(parent),_iMediaIndex(-1),_iMediaIdxUsed(-1),_iTrgDuration(-1)
{
}

//---------------------------------------------------------------------------
//
//! \brief   destroys QExtM3uParser object
//
//! \author  Jo2003
//! \date    12.12.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
QExtM3uParser::~QExtM3uParser()
{
}

//---------------------------------------------------------------------------
//
//! \brief   check for playlist type
//
//! \author  Jo2003
//! \date    12.12.2013
//
//! \param   pl (QString&) ref. to playlist content
//
//! \return  m3u::M3U_MASTER_PL -> master playlist;
//!          m3u::M3U_MEDIA_PL  -> media playlist
//!          m3u::M3U_UNKWN_PL  -> unknown playlist type
//---------------------------------------------------------------------------
m3u::t_type QExtM3uParser::plType(const QString &pl)
{
   m3u::t_type trv = m3u::M3U_UNKWN_PL;
   QStringList sl  = pl.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
   int iM3u = 0, iTok = 0;

   // to find out which type of playlist this is we should look
   // for all URIs there.
   // If this file only contains of playlists it is a master playlist.
   // If this file only contains media URIs it is a media playlist.
   // Else it is an unknown list type.
   for (int i = 0; i < sl.count(); i++)
   {
      // no tag, but URI ...
      if (!isTag(sl.at(i)) && isURI(sl.at(i)))
      {
         // playlist extension in URI ... ?
         if (sl.at(i).indexOf("m3u", 0, Qt::CaseInsensitive) > -1)
         {
            // count ...
            iM3u ++;
         }
         else
         {
            // count ..
            iTok ++;
         }
      }
   }

   // make the final check ...
   if (iM3u && !iTok)
   {
      trv = m3u::M3U_MASTER_PL;
   }
   else if (!iM3u && iTok)
   {
      trv = m3u::M3U_MEDIA_PL;
   }

   return trv;
}

//---------------------------------------------------------------------------
//
//! \brief   simple check for tag
//
//! \author  Jo2003
//! \date    12.12.2013
//
//! \param   line (QString&) ref. to one line from playlist
//
//! \return  true -> is tag; false -> no tag
//---------------------------------------------------------------------------
bool QExtM3uParser::isTag(const QString& line)
{
   // simply check if line starts with "#EXT" ...
   if (line.indexOf("#EXT") == 0)
   {
      return true;
   }
   else
   {
      return false;
   }
}

//---------------------------------------------------------------------------
//
//! \brief   very simple check for URI
//
//! \author  Jo2003
//! \date    12.12.2013
//
//! \param   line (QString&) ref. to one line from playlist
//
//! \return  true -> is URI; false -> no URI
//---------------------------------------------------------------------------
bool QExtM3uParser::isURI(const QString& line)
{
   QString s = line.simplified();

   // Maybe a bit to simple?
   // We think if line is not empty not a comment and no tag, it's an URI ...
   if ((s.length() > 0) && (s.at(0) != '#'))
   {
      return true;
   }
   else
   {
      return false;
   }
}

//---------------------------------------------------------------------------
//
//! \brief   get available streams from master playlist
//
//! \author  Jo2003
//! \date    12.12.2013
//
//! \param   pl (QString&) ref. to one line from playlist
//! \param   sVec (m3u::StreamVector&) ref. to stream vector
//
//! \return  0 -> ok; -1 -> any error
//---------------------------------------------------------------------------
int QExtM3uParser::getStreams(const QString& pl, m3u::StreamVector& sVec)
{
   int           iRet   = -1;
   QRegExp       rx;
   m3u::t_Stream stream;
   QStringList   sl     = pl.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);

   sVec.clear();

   // can be handled on master playlist only ...
   if (plType(pl) == m3u::M3U_MASTER_PL)
   {
      for (int i = 0; i < sl.count(); i++)
      {
         if (isTag(sl.at(i)))
         {
            // get stream info ...
            if (sl.at(i).indexOf("#EXT-X-STREAM-INF") > -1)
            {
               rx.setPattern("PROGRAM-ID=([0-9]*)");

               if (rx.indexIn(sl.at(i)) > -1)
               {
                  stream.iId = rx.cap(1).toInt();
               }

               rx.setPattern("RESOLUTION=([0-9]*)x([0-9]*)");

               if (rx.indexIn(sl.at(i)) > -1)
               {
                  stream.szRes.setWidth(rx.cap(1).toInt());
                  stream.szRes.setHeight(rx.cap(2).toInt());
               }

               rx.setPattern("BANDWIDTH=([0-9]*)");

               if (rx.indexIn(sl.at(i)) > -1)
               {
                  stream.iBandWidth = rx.cap(1).toInt();
               }
            }

            // other tags not supported so far ...
         }
         else if(isURI(sl.at(i)))
         {
            // get url of media playlist ...
            stream.sUri = completeUri(sl.at(i).simplified());

            // add element to vector ...
            sVec.append(stream);

            // reset stream structure ...
            stream.iBandWidth = -1;
            stream.iId        = -1;
            stream.szRes      = QSize();
            stream.sUri       = "";
         }
      }
   }

   if (!sVec.isEmpty())
   {
      iRet = 0;
   }

   return iRet;
}

//---------------------------------------------------------------------------
//
//! \brief   get stream parts / tokens from media playlist
//
//! \author  Jo2003
//! \date    12.12.2013
//
//! \param   pl (QString&) ref. to one line from playlist
//! \param   sTVec (m3u::StreamTokVector&) ref. to stream token vector
//
//! \return  0 -> ok; 1 -> playlist already handled; -1 -> any error
//---------------------------------------------------------------------------
int QExtM3uParser::getStreamToks(const QString& pl, m3u::StreamTokVector& sTVec)
{
   int              iRet      = -1;
   int              iTokCount =  0;
   QRegExp          rx;
   m3u::t_StreamTok stok;
   QStringList      sl        = pl.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);

   // reset endlist tag ...
   _bEndList = false;

   sTVec.clear();

   // can be handled on media playlist only ...
   if (plType(pl) == m3u::M3U_MEDIA_PL)
   {
      for (int i = 0; i < sl.count(); i++)
      {
         if (isTag(sl.at(i)))
         {
            if (sl.at(i).contains("#EXT-X-MEDIA-SEQUENCE:"))
            {
               rx.setPattern("#EXT-X-MEDIA-SEQUENCE:([0-9]*)");

               if (rx.indexIn(sl.at(i)) > -1)
               {
                  if (rx.cap(1).toInt() > _iMediaIndex)
                  {
                     _iMediaIndex = rx.cap(1).toInt();

                     if (_iMediaIdxUsed == -1)
                     {
                        _iMediaIdxUsed = _iMediaIndex;
                     }
                  }
                  else
                  {
                     // we already handled this playlist ...
                     iRet = 1;
                     break;
                  }
               }
            }
            else if(sl.at(i).contains("#EXT-X-TARGETDURATION:"))
            {
               rx.setPattern("#EXT-X-TARGETDURATION:([0-9]*)");

               if (rx.indexIn(sl.at(i)) > -1)
               {
                  _iTrgDuration = rx.cap(1).toInt();
               }
            }
            else if(sl.at(i).contains("#EXTINF:"))
            {
               rx.setPattern("#EXTINF:([^,]*),");

               if (rx.indexIn(sl.at(i)) > -1)
               {
                  stok.iDuration = qRound(rx.cap(1).toFloat());
               }

               rx.setPattern("#EXTINF:.*,(.*)$");

               if (rx.indexIn(sl.at(i)) > -1)
               {
                  stok.sTitle = rx.cap(1);
               }
            }
            else if(sl.at(i).contains("#EXT-X-BYTERANGE:"))
            {
               if (sl.at(i).indexOf('@') > -1)
               {
                  // offset given ...
                  rx.setPattern("#EXT-X-BYTERANGE:([0-9]*)@([0-9]*)");

                  if (rx.indexIn(sl.at(i)) > -1)
                  {
                     stok.iByteCount = rx.cap(1).toInt();
                     stok.iBOffset   = rx.cap(2).toInt();
                  }
               }
               else
               {
                  // without offset ...
                  rx.setPattern("#EXT-X-BYTERANGE:([0-9]*)");

                  if (rx.indexIn(sl.at(i)) > -1)
                  {
                     stok.iByteCount = rx.cap(1).toInt();
                     stok.iBOffset   = 0;
                  }
               }
            }
            else if(sl.at(i).contains("#EXT-X-ENDLIST"))
            {
               _bEndList = true;
            }

            // other tags not supported so far ...
         }
         else if(isURI(sl.at(i)))
         {
            iTokCount ++;
            if ((_iMediaIndex + iTokCount) > _iMediaIdxUsed)
            {
               stok.sUri = completeUri(sl.at(i).simplified());
               _iMediaIdxUsed ++;
               sTVec.append(stok);
            }

            // reset stream structure ...
            stok.iBOffset   = -1;
            stok.iByteCount = -1;
            stok.iDuration  = -1;
            stok.sTitle     = "";
            stok.sUri       = "";
         }
      }
   }

   if (!sTVec.isEmpty() && (iRet != 1))
   {
      iRet = 0;
   }

   return iRet;
}

//---------------------------------------------------------------------------
//
//! \brief   reset duration and media index
//
//! \author  Jo2003
//! \date    12.12.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QExtM3uParser::reset()
{
   _iMediaIndex   = -1;
   _iMediaIdxUsed = -1;
   _iTrgDuration  = -1;
   _sMasterUrl    = "";
}

//---------------------------------------------------------------------------
//
//! \brief   get media index
//
//! \author  Jo2003
//! \date    12.12.2013
//
//! \param   --
//
//! \return  media index
//---------------------------------------------------------------------------
const int& QExtM3uParser::mediaIndex()
{
   return _iMediaIndex;
}

//---------------------------------------------------------------------------
//
//! \brief   get max. duration value
//
//! \author  Jo2003
//! \date    12.12.2013
//
//! \param   --
//
//! \return  max. duration value
//---------------------------------------------------------------------------
const int& QExtM3uParser::targetDuration()
{
   return _iTrgDuration;
}

//---------------------------------------------------------------------------
//
//! \brief   set master url used in case of relative playlist pathes
//
//! \author  Jo2003
//! \date    13.12.2013
//
//! \param   s (const QString&) master url
//
//! \return  --
//---------------------------------------------------------------------------
void QExtM3uParser::setMasterUrl(const QString &s)
{
   int i = s.lastIndexOf("/");

   // remove playlist part of URI ...
   if (i > -1)
   {
      _sMasterUrl = s.left(i + 1); // include the slash ...
   }
   else
   {
      _sMasterUrl = s;
   }
}

//---------------------------------------------------------------------------
//
//! \brief   prepend absolute path to uri if needed
//
//! \author  Jo2003
//! \date    16.12.2013
//
//! \param   sIn (const QString&) master url
//
//! \return  URI
//---------------------------------------------------------------------------
QString QExtM3uParser::completeUri(const QString& sIn)
{
   // absolute or relative path ...
   if (sIn.indexOf("://") > -1)
   {
      // absolute ...
      return sIn;
   }
   else
   {
      // relative -> add first part ..
      return _sMasterUrl + sIn;
   }
}

//---------------------------------------------------------------------------
//
//! \brief   end of list reached
//
//! \author  Jo2003
//! \date    17.12.2013
//
//! \param   --
//
//! \return  true -> end reached; false -> not reached
//---------------------------------------------------------------------------
bool QExtM3uParser::endList()
{
   return _bEndList;
}
