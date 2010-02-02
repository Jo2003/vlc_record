/*********************** Information *************************\
| $HeadURL$
|
| Author: Joerg Neubert
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

#include "clogfile.h"
#include "templates.h"

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
|  Author: Joerg Neubert
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
   void    SetCache (int iTime);
   Q_PID   start (const QString& clargs, int iRunTime = -1);
   void    CancelTimer ();
   void    SetTimer (uint uiTime);
   void    stop ();
   bool    IsRunning ();
   QString CreateClArgs (vlcctrl::eVlcAct eAct, const QString &url, const QString &dst = QString(), const QString &mux = QString());

private:
   int     iCacheTime;
   QString sProgPath;
   QTimer  tRunTime;

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
