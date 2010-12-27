/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 24.01.2010 / 15:41:34
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#ifndef __040910__CTIMEREX_H
   #define __040910__CTIMEREX_H

#include <QTime>

/********************************************************************\
|  Class: CTimerEx
|  Date:  09.04.2010 / 10:02:24
|  Author: Jo2003
|  Description: extend QTime class to add pause and offset
|
\********************************************************************/
class CTimerEx : public QTime
{
public:
   /* -----------------------------------------------------------------\
   |  Method: reset
   |  Begin: 09.04.2010 / 10:25:00
   |  Author: Jo2003
   |  Description: reset offset and pause stuff
   |
   |  Parameters: --
   |
   |  Returns: --
   \----------------------------------------------------------------- */
   void reset ()
   {
      uiMsPlayed = 0;
      bPaused    = false;
   }

   /* -----------------------------------------------------------------\
   |  Method: CTimerEx / constructor
   |  Begin: 09.04.2010 / 10:05:00
   |  Author: Jo2003
   |  Description: create CTimerEx object, init values
   |
   |  Parameters: --
   |
   |  Returns: --
   \----------------------------------------------------------------- */
   CTimerEx () : QTime ()
   {
      reset();
   }

   /* -----------------------------------------------------------------\
   |  Method: CTimerEx / constructor
   |  Begin: 09.04.2010 / 10:05:00
   |  Author: Jo2003
   |  Description: create CTimerEx object, init values
   |
   |  Parameters: hours, minutes, seconds, msecs
   |
   |  Returns: --
   \----------------------------------------------------------------- */
   CTimerEx (int h, int m, int s = 0, int ms = 0) : QTime (h, m, s, ms)
   {
      reset();
   }

   /* -----------------------------------------------------------------\
   |  Method: setStartGmt
   |  Begin: 27.12.2010 / 12:35
   |  Author: Jo2003
   |  Description: set start time from unix timestamp
   |
   |  Parameters: unix timestamp
   |
   |  Returns: --
   \----------------------------------------------------------------- */
   void setStartGmt (uint uiTime)
   {
      uiGmtStart = uiTime;
   }

   /* -----------------------------------------------------------------\
   |  Method: start
   |  Begin: 09.04.2010 / 10:25:00
   |  Author: Jo2003
   |  Description: override start function to support "unpause" ...
   |
   |  Parameters: --
   |
   |  Returns: --
   \----------------------------------------------------------------- */
   virtual void start ()
   {
      if (bPaused)
      {
         bPaused = false;
         QTime::restart();
      }
      else
      {
         QTime::start();
      }
   }

   /* -----------------------------------------------------------------\
   |  Method: addSecsEx
   |  Begin: 09.04.2010 / 10:05:00
   |  Author: Jo2003
   |  Description: add offset to timer
   |
   |  Parameters: offset in seconds (note: can be < 0)
   |
   |  Returns: --
   \----------------------------------------------------------------- */
   void addSecsEx (int secs)
   {
      if (secs)
      {
         uiMsPlayed += secs * 1000;
      }
   }

   /* -----------------------------------------------------------------\
   |  Method: pause
   |  Begin: 09.04.2010 / 10:05:00
   |  Author: Jo2003
   |  Description: pause timer
   |
   |  Parameters: --
   |
   |  Returns: --
   \----------------------------------------------------------------- */
   void pause ()
   {
      if (!bPaused)
      {
         uiMsPlayed += elapsed();
         bPaused     = true;
      }
   }

   /* -----------------------------------------------------------------\
   |  Method: elapsedEx
   |  Begin: 09.04.2010 / 10:05:00
   |  Author: Jo2003
   |  Description: get elapsed time in ms, take care for pause and
   |               offset
   |  Parameters: --
   |
   |  Returns: elapsed time in msecs.
   \----------------------------------------------------------------- */
   int elapsedEx ()
   {
      if (bPaused)
      {
         return uiMsPlayed;
      }
      else
      {
         return elapsed() + uiMsPlayed;
      }
   }

   /* -----------------------------------------------------------------\
   |  Method: gmtPosition
   |  Begin: 27.12.2010 / 13:00
   |  Author: Jo2003
   |  Description: get actual position outgoing from
   |               start gmt timestamp
   |  Parameters: --
   |
   |  Returns: gmt timestamp
   \----------------------------------------------------------------- */
   uint gmtPosition ()
   {
      return uiGmtStart + elapsedEx() / 1000;
   }

private:
   uint  uiMsPlayed;
   uint  uiGmtStart;
   bool  bPaused;
};

#endif // __040910__CTIMEREX_H
/************************* History ***************************\
| $Log$
\*************************************************************/

