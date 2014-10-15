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
#include "cplaystate.h"

// macro to print state ...
#define mCaseState(__state__) case __state__: sOut = #__state__; break

/* -----------------------------------------------------------------\
|  Method: CPlayState / constructor
|  Begin: 08.04.2010 / 15:05:00
|  Author: Jo2003
|  Description: create play state object, init values
|
|  Parameters: pointer to parent object
|
|  Returns: --
\----------------------------------------------------------------- */
CPlayState::CPlayState (QObject *parent) : QObject(parent)
{
   ulRunTime     = 0;
   bArchive      = false;
   bTimer        = false;
   eActState     = PlayState::PS_WTF;
   eBackupState  = PlayState::PS_WTF;
   pParentWidget = NULL;
}

/* -----------------------------------------------------------------\
|  Method: ~CPlayState / destructor
|  Begin: 08.04.2010 / 15:05:00
|  Author: Jo2003
|  Description: clean at object destruction
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
CPlayState::~CPlayState()
{
   // nothing to do so far ...
}

/* -----------------------------------------------------------------\
|  Method: setParentWidget
|  Begin: 08.04.2010 / 15:05:00
|  Author: Jo2003
|  Description: store a pointer to parent widget so message boxes
|               will be displayed at right place ...
|
|  Parameters: pointer to parent widget ...
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayState::setParentWidget(QWidget *parent)
{
   pParentWidget = parent;
}

/* -----------------------------------------------------------------\
|  Method: setInfo
|  Begin: 08.04.2010 / 15:05:00
|  Author: Jo2003
|  Description: set play state info
|
|  Parameters: title, channel, start time, end time, archive flag
|              timer record flag
|  Returns: --
\----------------------------------------------------------------- */
void CPlayState::setInfo(const QString &title, const QString &chan,
                         const QDateTime &start, const QDateTime &end,
                         bool bAr, bool bTimrec)
{
   sTitle    = title;
   sChannel  = chan;
   dtStart   = start;
   dtEnd     = end;
   bArchive  = bAr;
   bTimer    = bTimrec;
   ulRunTime = dtEnd.toTime_t() - dtStart.toTime_t();

   // new info --> reset state ...
   eActState = PlayState::PS_WTF;
}

/* -----------------------------------------------------------------\
|  Method: setState
|  Begin: 08.04.2010 / 15:05:00
|  Author: Jo2003
|  Description: set state without asking
|
|  Parameters: new state
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayState::setState(PlayState::ePlayState newState)
{
   PlayState::ePlayState stateBackup = eActState;
   eActState = newState;

   // was state changed ... ?
   if (stateBackup != eActState)
   {
      // notify about state change ...
      emit sigStateChange((int)stateBackup, (int)eActState);
   }
}

/* -----------------------------------------------------------------\
|  Method: doStateBackup
|  Begin: 09.04.2010 / 09:15:11
|  Author: Jo2003
|  Description: backup actual state
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayState::doStateBackup()
{
   eBackupState = eActState;
}

/* -----------------------------------------------------------------\
|  Method: WantToClose
|  Begin: 01.02.2010 / 15:05:00
|  Author: Jo2003
|  Description: ask if we want to clse vlc-record
|
|  Parameters: --
|
|  Returns: true --> close
|          false --> don't close
\----------------------------------------------------------------- */
bool CPlayState::WantToClose()
{
   QString sText = HTML_SITE;
   sText.replace(TMPL_CONT, tr("VLC is still running.<br />"
                               "<b>Closing VLC record will also close the started VLC-Player.</b>"
                               "<br /> <br />"
                               "Do you really want to close VLC Record now?"));

   if (QMessageBox::question(pParentWidget, tr("Question"), sText,
                             QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
   {
      return true;
   }
   else
   {
      return false;
   }
}

/* -----------------------------------------------------------------\
|  Method: WantToStopRec
|  Begin: 02.02.2010 / 10:05:00
|  Author: Jo2003
|  Description: ask if we want to stop recording
|
|  Parameters: --
|
|  Returns: true --> stop it
|          false --> don't stop me now ;-)
\----------------------------------------------------------------- */
bool CPlayState::WantToStopRec()
{
   QString sText = HTML_SITE;
   sText.replace(TMPL_CONT, tr("Pending Record!"
                               "<br /> <br />"
                               "Do you really want to stop recording now?"));

   if (QMessageBox::question(pParentWidget, tr("Question"), sText,
                             QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
   {
      return true;
   }
   else
   {
      return false;
   }
}

/* -----------------------------------------------------------------\
|  Method: permitAction
|  Begin: 08.04.2010 / 15:05:00
|  Author: Jo2003
|  Description: check if action is permitted, update state if so
|
|  Parameters: requested new state
|
|  Returns: true --> permitted
|          false --> denied
\----------------------------------------------------------------- */
bool CPlayState::permitAction(PlayState::ePlayState newState)
{
   bool bRV = false;
   PlayState::ePlayState stateBackup = eActState;

   switch (eActState)
   {
   case PlayState::PS_RECORD:
      // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
      {
         // pending record ...
         switch (newState)
         {
            // requested action stop or new record ...
         case PlayState::PS_STOP:
         case PlayState::PS_RECORD:
            // ask for permission ...
            if (WantToStopRec ())
            {
               // permission granted ...
               bRV        = true;

               // set new state ...
               eActState = newState;
            }
            break;
         default:
            // all other actions permitted ...
            break;
         }
      }
      // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
      break;

   case PlayState::PS_TIMER_RECORD:
   case PlayState::PS_TIMER_STBY:
      // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
      if (newState == PlayState::PS_STOP)
      {
         // ask for permission ...
         if (WantToStopRec ())
         {
            // permission granted ...
            bRV        = true;

            // set new state ...
            eActState = newState;
         }
      }
      // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
      break;

   default:
      // >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
      // don't ask for every fart.
      // Let the user decide what he wants to do!
      bRV        = true;

      // set new state ...
      eActState = newState;
      // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
      break;
   }

   // was state changed ... ?
   if (stateBackup != eActState)
   {
      // notify about state change ...
      emit sigStateChange((int)stateBackup, (int)eActState);
   }

   return bRV;
}

/* -----------------------------------------------------------------\
|  Method: permitClose
|  Begin: 08.04.2010 / 15:05:00
|  Author: Jo2003
|  Description: can we close program
|
|  Parameters: --
|
|  Returns: true --> yes
|          false --> no
\----------------------------------------------------------------- */
bool CPlayState::permitClose()
{
   bool bRV = true;

   switch (eActState)
   {
   case PlayState::PS_RECORD:
   case PlayState::PS_TIMER_RECORD:
   case PlayState::PS_TIMER_STBY:
      if (!WantToStopRec())
      {
         bRV = false;
      }
      break;

   default:
      break;
   }

   return bRV;
}

/* -----------------------------------------------------------------\
|  Method: permitStreamCtrl
|  Begin: 18.03.2010 / 15:07:12
|  Author: Jo2003
|  Description: only allow stream control on archive play
|
|  Parameters:
|
|  Returns: true --> allowed
|          false --> not allowed
\----------------------------------------------------------------- */
bool CPlayState::permitStreamCtrl()
{
   if (bArchive && (eActState == PlayState::PS_PLAY))
   {
      return true;
   }
   else
   {
      return false;
   }
}

/* -----------------------------------------------------------------\
|  Method: slotProcStateChanged
|  Begin: 08.04.2010 / 16:25:12
|  Author: Jo2003
|  Description: state change of external process
|
|  Parameters: new state
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayState::slotProcStateChanged(QProcess::ProcessState newState)
{
   // external player was stopped ...
   if (newState == QProcess::NotRunning)
   {
      // means was running, has now stopped ...
      if (eActState != PlayState::PS_STOP)
      {
         PlayState::ePlayState stateBackup = eActState;
         eActState = PlayState::PS_STOP;
         emit sigStateChange((int)stateBackup, (int)eActState);
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: title
|  Begin: 08.04.2010 / 15:05:00
|  Author: Jo2003
|  Description: get actual title playing ...
|
|  Parameters: --
|
|  Returns: ref. to title
\----------------------------------------------------------------- */
const QString& CPlayState::title()
{
   return sTitle;
}

/* -----------------------------------------------------------------\
|  Method: channel
|  Begin: 08.04.2010 / 15:05:00
|  Author: Jo2003
|  Description: get actual channel playing ...
|
|  Parameters: --
|
|  Returns: ref. to channel
\----------------------------------------------------------------- */
const QString& CPlayState::channel()
{
   return sChannel;
}

/* -----------------------------------------------------------------\
|  Method: start
|  Begin: 08.04.2010 / 15:05:00
|  Author: Jo2003
|  Description: get start time of actual show
|
|  Parameters: --
|
|  Returns: ref. to start time
\----------------------------------------------------------------- */
const QDateTime& CPlayState::start()
{
   return dtStart;
}

/* -----------------------------------------------------------------\
|  Method: end
|  Begin: 08.04.2010 / 15:05:00
|  Author: Jo2003
|  Description: get end time of actual show
|
|  Parameters: --
|
|  Returns: ref. to end time
\----------------------------------------------------------------- */
const QDateTime& CPlayState::end()
{
   return dtEnd;
}

/* -----------------------------------------------------------------\
|  Method: runtime
|  Begin: 08.04.2010 / 15:05:00
|  Author: Jo2003
|  Description: get run time of actual show
|
|  Parameters: --
|
|  Returns: runtime in seconds
\----------------------------------------------------------------- */
ulong CPlayState::runtime()
{
   return ulRunTime;
}

/* -----------------------------------------------------------------\
|  Method: archive
|  Begin: 08.04.2010 / 15:05:00
|  Author: Jo2003
|  Description: is archive in use
|
|  Parameters: --
|
|  Returns: archive flag
\----------------------------------------------------------------- */
bool CPlayState::archive()
{
   return bArchive;
}

/* -----------------------------------------------------------------\
|  Method: timerRecord
|  Begin: 08.04.2010 / 15:05:00
|  Author: Jo2003
|  Description: is timer in use
|
|  Parameters: --
|
|  Returns: timer flag
\----------------------------------------------------------------- */
bool CPlayState::timerRecord()
{
   return bTimer;
}

/* -----------------------------------------------------------------\
|  Method: state
|  Begin: 08.04.2010 / 15:05:00
|  Author: Jo2003
|  Description: get actual play state
|
|  Parameters: --
|
|  Returns: play state
\----------------------------------------------------------------- */
PlayState::ePlayState CPlayState::state()
{
   return eActState;
}

/* -----------------------------------------------------------------\
|  Method: backupState
|  Begin: 09.04.2010 / 09:15:11
|  Author: Jo2003
|  Description: get backuped state
|
|  Parameters: --
|
|  Returns: backup state
\----------------------------------------------------------------- */
PlayState::ePlayState CPlayState::backupState()
{
   return eBackupState;
}

/* -----------------------------------------------------------------\
|  Method: setTitle
|  Begin: 09.04.2010 / 09:15:11
|  Author: Jo2003
|  Description: set show title
|
|  Parameters: title
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayState::setTitle (const QString& title)
{
   sTitle = title;
}

/* -----------------------------------------------------------------\
|  Method: setChannel
|  Begin: 09.04.2010 / 09:15:11
|  Author: Jo2003
|  Description: set channel name
|
|  Parameters: channel
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayState::setChannel (const QString& chan)
{
   sChannel = chan;
}

/* -----------------------------------------------------------------\
|  Method: setStartEnd
|  Begin: 09.04.2010 / 09:15:11
|  Author: Jo2003
|  Description: set start and end time
|
|  Parameters: start time, end time
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayState::setStartEnd (const QDateTime& start, const QDateTime& end)
{
   dtStart = start;
   dtEnd   = end;
}

/* -----------------------------------------------------------------\
|  Method: setArchive
|  Begin: 09.04.2010 / 09:15:11
|  Author: Jo2003
|  Description: set archive info
|
|  Parameters: archive bool
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayState::setArchive (bool bAr)
{
   bArchive = bAr;
}

/* -----------------------------------------------------------------\
|  Method: setTimerRec
|  Begin: 09.04.2010 / 09:15:11
|  Author: Jo2003
|  Description: set timer record info
|
|  Parameters: timer record bool
|
|  Returns: --
\----------------------------------------------------------------- */
void CPlayState::setTimerRec (bool bTimrec)
{
   bTimer = bTimrec;
}

/* -----------------------------------------------------------------\
|  Method: stateText
|  Begin: 09.04.2010 / 12:45:11
|  Author: Jo2003
|  Description: get string for state
|
|  Parameters: playstate
|
|  Returns: state as string
\----------------------------------------------------------------- */
QString CPlayState::stateText(PlayState::ePlayState state)
{
   QString sOut;

   switch (state)
   {
      // use macro to make all the state cases ...
      mCaseState(PlayState::PS_BUFFER);
      mCaseState(PlayState::PS_END);
      mCaseState(PlayState::PS_ERROR);
      mCaseState(PlayState::PS_OPEN);
      mCaseState(PlayState::PS_PAUSE);
      mCaseState(PlayState::PS_PLAY);
      mCaseState(PlayState::PS_READY);
      mCaseState(PlayState::PS_RECORD);
      mCaseState(PlayState::PS_STOP);
      mCaseState(PlayState::PS_TIMER_RECORD);
      mCaseState(PlayState::PS_TIMER_STBY);
      mCaseState(PlayState::PS_WTF);
   default:
      break;
   }

   return sOut;
}

/************************* History ***************************\
| $Log$
\*************************************************************/
