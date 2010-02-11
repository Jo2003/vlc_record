/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 18.01.2010 / 16:21:44
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#ifndef __011810__CHTTPTIME_H
   #define __011810__CHTTPTIME_H

#include <QDateTime>
#include <QTextStream>


namespace httptime {
   class CHttpTime;
   static const char *pDays[] = {
      "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
   };

   static const char *pMonths[] = {
      "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
   };
}

/********************************************************************\
|  Class: CHttpTime
|  Date:  18.01.2010 / 16:22:18
|  Author: Jo2003
|  Description: advanced QDateTime to give needed http timestamp
|
\********************************************************************/
class CHttpTime : public QDateTime
{
public:
    CHttpTime();

    static QString GetHttpTime ()
    {
       // Format: Tue, 15 Nov 1994 08:12:31 GMT
       QString     s;
       QTextStream str(&s);
       QDateTime   now = QDateTime::currentDateTime().toUTC();

       str << httptime::pDays[now.date().dayOfWeek() - 1]
             << ", " << now.date().day() << " "
             << httptime::pMonths[now.date().month() - 1]
             << " " << now.date().year() << " "
             << now.toString("hh:mm:ss") << " GMT";

       return s;
    }
};

#endif /* __011810__CHTTPTIME_H */
/************************* History ***************************\
| $Log$
\*************************************************************/

