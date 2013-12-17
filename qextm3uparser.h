/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qextm3uparser.h
 *
 *  @author   Jo2003
 *
 *  @date     12.12.2013
 *
 *  $Id$
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#ifndef __20131212_QEXTM3UPARSER_H
   #define __20131212_QEXTM3UPARSER_H

#include <QString>
#include <QVector>
#include <QMap>

#include <QObject>
#include <QSize>

// ----------------------------------------------------------
//    namespace ...
// ----------------------------------------------------------
namespace m3u {

   typedef struct Stream {
      Stream():iId(-1),iBandWidth(-1){}
      int     iId;
      int     iBandWidth;
      QSize   szRes;
      QString sUri;
   } t_Stream;

   typedef struct StreamTok {
      StreamTok():iDuration(-1),iByteCount(-1),iBOffset(-1){}
      int iDuration;
      int iByteCount;
      int iBOffset;
      QString sTitle;
      QString sUri;
   } t_StreamTok;

   typedef enum {
      M3U_MASTER_PL,
      M3U_MEDIA_PL,
      M3U_HLS_TOK,
      M3U_UNKWN_PL = 255
   } t_type;

   typedef QVector<t_Stream>       StreamVector;
   typedef QVector<t_StreamTok>    StreamTokVector;
}

//---------------------------------------------------------------------------
//! \class   QExtM3uParser
//! \date    12.12.2013
//! \author  Jo2003
//! \brief   an EXT-M3U playlist parser
//---------------------------------------------------------------------------
class QExtM3uParser : public QObject
{
   Q_OBJECT

public:
   QExtM3uParser(QObject *parent = 0);
   ~QExtM3uParser();

   m3u::t_type plType(const QString& pl);
   int getStreams(const QString& pl, m3u::StreamVector& sVec);
   int getStreamToks(const QString& pl, m3u::StreamTokVector& sTVec);

   void reset();
   bool endList();

   const int& mediaIndex();
   const int& targetDuration();

   void setMasterUrl(const QString& s);
   QString completeUri(const QString& sIn);

protected:
   bool isTag(const QString& line);
   bool isURI(const QString& line);

private:
   int     _iMediaIndex;
   int     _iMediaIdxUsed;
   int     _iTrgDuration;
   bool    _bEndList;
   QString _sMasterUrl;
};

#endif // __20131212_QEXTM3UPARSER_H
