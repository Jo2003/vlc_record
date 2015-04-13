/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/qhlscontrol.h $
 *
 *  @file     qhlscontrol.h
 *
 *  @author   Jo2003
 *
 *  @date     16.12.2013
 *
 *  $Id: qhlscontrol.h 1277 2013-12-20 10:46:06Z Olenka.Joerg $
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#ifndef __20131216_QHLSCONTROL_H
   #define __20131216_QHLSCONTROL_H

#include "qextm3uparser.h"
#include <QObject>
#include <QTimer>
#include <QFile>
#include "cdirstuff.h"

#define DEF_STREAM_FIFO (pFolders->getTmpFolder() + "/_hlsFifo")

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
   void startHls(const QString& sUrl, int iBuffSec, const QString& sPath = QString());
   bool isActive();

protected:
   int mediaPlDuration();
   
signals:
   void sigPlay(const QString& s);
   
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

   int    _iBytesWritten;
   int    _iBandWidth;
   int    _iBuffSecs;

   QFile  _fVlcFifo;
   QTimer _tReloadPl;
};

#endif // __20131216_QHLSCONTROL_H
