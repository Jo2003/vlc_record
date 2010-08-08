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
      iMsOffset = 0;
      iMsPaused = 0;
      bPaused   = false;
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
         iMsPaused += pauseTimer.elapsed();
         bPaused = false;
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
      iMsOffset += secs * 1000;
   }

   /* -----------------------------------------------------------------\
   |  Method: addMsecsEx
   |  Begin: 09.04.2010 / 10:05:00
   |  Author: Jo2003
   |  Description: add offset to timer
   |
   |  Parameters: offset in milliseconds (note: can be < 0)
   |
   |  Returns: --
   \----------------------------------------------------------------- */
   void addMsecsEx (int msecs)
   {
      iMsOffset += msecs;
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
         pauseTimer.start();
         bPaused = true;
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
      return elapsed() + iMsOffset - iMsPaused
            - ((bPaused) ? pauseTimer.elapsed() : 0);
   }

private:
   QTime pauseTimer;
   uint  iMsPaused;
   int   iMsOffset;
   bool  bPaused;
};

#endif // __040910__CTIMEREX_H
/************************* History ***************************\
| $Log$
\*************************************************************/

