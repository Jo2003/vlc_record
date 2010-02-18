/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 01.02.2010 / 10:50:35
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#ifndef __020110__CVLCCTRL_H
   #define __020110__CVLCCTRL_H

#include <QProcess>
#include <QObject>
#include <QTimer>
#include <QMessageBox>
#include <QTextStream>
#include <QRegExp>
#include <QFile>
#include <QFileInfo>

#include "clogfile.h"
#include "defdef.h"
#include "templates.h"
#include "ctranslit.h"

namespace vlcctrl
{
   class CVlcCtrl;
   enum eVlcAct
   {
      VLC_PLAY_HTTP,
      VLC_PLAY_RTSP,
      VLC_REC_HTTP,
      VLC_REC_RTSP,
      VLC_REC_HTTP_SILENT,
      VLC_REC_RTSP_SILENT,
      VLC_UNKNOWN = 255
   };
}

/********************************************************************\
|  Class: CVlcCtrl
|  Date:  01.02.2010 / 10:00:28
|  Author: Jo2003
|  Description: class to control vlc, inherits QProcess
|
\********************************************************************/
class CVlcCtrl : public QProcess
{
Q_OBJECT

public:
   CVlcCtrl(const QString &path = QString(), QObject * parent = 0);
   virtual ~CVlcCtrl();

   void    SetProgPath(const QString &str);
   Q_PID   start (const QString& sCmdLine, int iRunTime = -1);
   void    CancelTimer ();
   void    SetTimer (uint uiTime);
   void    stop ();
   bool    IsRunning ();
   void    SetTranslitPointer (CTranslit *pTr);
   void    SetTranslitSettings (bool bTr);
   QString CreateClArgs (vlcctrl::eVlcAct eAct, const QString &sPlayer,
                         const QString &url, int iCacheTime, const QString &dst = QString(),
                         const QString &mux = QString());

   int     LoadPlayerModule (const QString &sPath);

private:
   QTimer     tRunTime;
   QString    sHttpPlay;
   QString    sRtspPlay;
   QString    sHttpRec;
   QString    sRtspRec;
   QString    sHttpSilentRec;
   QString    sRtspSilentRec;
   bool       bForcedTranslit;
   bool       bTranslit;
   CTranslit *pTranslit;
   QString    sFrcMx;

private slots:
   void slotStateChanged (QProcess::ProcessState newState);

signals:
   void sigVlcEnds ();
   void sigVlcStarts ();

public slots:
};

#endif /* __020110__CVLCCTRL_H */
/************************* History ***************************\
| $Log$
\*************************************************************/
