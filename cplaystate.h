/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 24.02.2010 / 10:41:34
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#ifndef __030410__CPLAYSTATE_H
   #define __030410__CPLAYSTATE_H

#include <QString>
#include <QDateTime>
#include <QtGui>
#include <QMessageBox>
#include <QProcess>

#include "templates.h"

//--------------------------------------------------------------
// name space for playstate ...
//--------------------------------------------------------------
namespace PlayState
{
   enum ePlayState
   {
      PS_READY,
      PS_OPEN,
      PS_BUFFER,
      PS_PLAY,
      PS_PAUSE,
      PS_STOP,
      PS_END,
      PS_ERROR,
      PS_RECORD,
      PS_TIMER_RECORD,
      PS_TIMER_STBY,
      PS_WTF    = 255 // ;-)
   };

   class CPlayState;
}

/********************************************************************\
|  Class: CPlayState
|  Date:  08.04.2010 / 14:22:18
|  Author: Jo2003
|  Description: represent the play state ...
|
\********************************************************************/
class CPlayState : public QObject
{
   Q_OBJECT

public:
   CPlayState(QObject *parent = 0);
   ~CPlayState ();

   void setParentWidget (QWidget *parent);

   void setInfo (const QString& title, const QString& chan = QString(),
                 const QDateTime& start = QDateTime(),
                 const QDateTime& end = QDateTime(),
                 bool bAr = false, bool bTimrec = false);

   bool permitAction (PlayState::ePlayState newState);
   bool permitClose ();
   bool permitStreamCtrl ();

   void setState (PlayState::ePlayState newState);

   const QString& title();
   const QString& channel();
   const QDateTime& start();
   const QDateTime& end();
   ulong runtime();
   bool  archive();
   bool  timerRecord();
   PlayState::ePlayState state();

protected:
   bool WantToStopRec ();
   bool WantToClose ();

private:
   QDateTime dtStart;
   QDateTime dtEnd;
   QString   sTitle;
   QString   sChannel;
   ulong     ulRunTime;
   bool      bArchive;
   bool      bTimer;
   QWidget  *pParentWidget;
   PlayState::ePlayState eActState;

public slots:
   void slotProcStateChanged (QProcess::ProcessState newState);

signals:
   void sigStateChange (int iOldState, int iNewState);
};

#endif // __030410__CPLAYSTATE_H
/************************* History ***************************\
| $Log$
\*************************************************************/

