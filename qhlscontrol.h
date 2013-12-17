/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qhlscontrol.h
 *
 *  @author   Jo2003
 *
 *  @date     16.12.2013
 *
 *  $Id$
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#ifndef __20131216_QHLSCONTROL_H
   #define __20131216_QHLSCONTROL_H

#include "qextm3uparser.h"
#include <QObject>
#include <QTimer>
#include <QFile>

#ifdef Q_OS_WIN32
   #define DEF_STREAM_FIFO "\\\\.\\pipe\\hlsfifo"
#else // Q_OS_UNIX
   #define DEF_STREAM_FIFO "/tmp/hlsfifo"
#endif

//---------------------------------------------------------------------------
//! \class   QHlsControl
//! \date    16.12.2013
//! \author  Jo2003
//! \brief   class to handle HLS downloads and piping
//---------------------------------------------------------------------------
class QHlsControl : public QObject
{
   Q_OBJECT

public:
   QHlsControl(QObject *parent = 0);
   ~QHlsControl();
   int fd();
   void startHls(const QString& sUrl, int iBuffSec);

protected:
   int mediaPlDuration();
   
signals:
   void sigPlay();
   
public slots:
   void slotM3uResp(int id, const QString& resp);
   void slotStreamTokResp (int id, const QByteArray& tok);
   void stop();

private slots:
   void slotNewMediaPl();

private:
   QExtM3uParser       *_pM3uParser;
   bool                 _bGo;
   bool                 _bPlay;
   m3u::StreamTokVector _stVec;
   QString              _sMasterPlUri;

   int   _iBytesWritten;
   int   _iBandWidth;
   int   _iBuffSecs;

   QFile _fVlcFifo;
};

#endif // __20131216_QHLSCONTROL_H
