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
#include <QTimer>

#include "clogfile.h"

/********************************************************************\
|  Class: CVlcCtrl
|  Date:  01.02.2010 / 10:00:28
|  Author: Joerg Neubert
|  Description: class to control vlc, inherits QProcess
|
\********************************************************************/
class CVlcCtrl : QProcess
{
Q_OBJECT

public:
   CVlcCtrl(const QString &path = QString(), QObject * parent = 0);
   virtual ~CVlcCtrl();

   void    SetProgPath(const QString &str);
   Q_PID   start (const QString& clargs, int iRunTime = -1);
   void    CancelTimer ();
   void    stop ();
   bool    IsRunning ();

private:
   QString sProgPath;
   QTimer  tRunTime;

signals:

public slots:
};

#endif /* __020110__CVLCCTRL_H */
/************************* History ***************************\
| $Log$
\*************************************************************/
