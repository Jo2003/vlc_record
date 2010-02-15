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
#include <QRegExp>
#include "defdef.h"

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

   /* -----------------------------------------------------------------\
   |  Method: GetHttpTime [static]
   |  Begin: 15.02.2010 / 11:05:24
   |  Author: Jo2003
   |  Description: get http conform time string
   |
   |  Parameters: --
   |
   |  Returns: time string
   \----------------------------------------------------------------- */
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

   /* -----------------------------------------------------------------\
   |  Method: fromEnString [static]
   |  Begin: 15.02.2010 / 11:05:24
   |  Author: Jo2003
   |  Description: get date time generated from english string
   |
   |  Parameters: english time string in format MMM dd, yyyy hh:mm:ss
   |
   |  Returns: QDateTime from string
   \----------------------------------------------------------------- */
   static QDateTime fromEnString (const QString & string)
   {
      QRegExp   rx("^([^ ]*) (.*)$");
      QString   locTimeString = string;

      if (rx.indexIn(string) > -1)
      {
         // find month name ...
         for (int i = 0; i < 12; i++)
         {
            if (rx.cap(1) == httptime::pMonths[i])
            {
               // create localized time string ...
               locTimeString = QString("%1 %2").arg(QDate::shortMonthName(i + 1))
                               .arg(rx.cap(2));
               break;
            }
         }
      }

      return QDateTime::fromString(locTimeString, DEF_TIME_FORMAT);
   }
};

#endif /* __011810__CHTTPTIME_H */
/************************* History ***************************\
| $Log$
\*************************************************************/

