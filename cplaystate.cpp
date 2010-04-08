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
   ulRunTime = 0;
   bArchive  = false;
   bTimer    = false;
   eActState = PlayState::PS_WTF;
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

   return bRV;
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

/************************* History ***************************\
| $Log$
\*************************************************************/
