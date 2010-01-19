/*********************** Information *************************\
| $HeadURL: svn://172.25.1.40/joergn/trunk/vlc-record/main.cpp $
| 
| Author: Joerg Neubert
|
| Begin: 19.01.2010 / 15:57:06
| 
| Last edited by: $Author: joergn $
| 
| $Id: main.cpp 176 2010-01-19 15:29:52Z joergn $
\*************************************************************/
#include <QtGui/QApplication>
#include <QTranslator>
#include "recorder.h"

#ifdef DINCLUDEPLUGS
#include <QtPlugin>
Q_IMPORT_PLUGIN(qgif)
Q_IMPORT_PLUGIN(qico)
#endif // DINCLUDEPLUGS

// make logging class available everywhere ...
CLogFile VlcLog;

/* -----------------------------------------------------------------\
|  Method: main / program entry
|  Begin: 19.01.2010 / 15:57:36
|  Author: Joerg Neubert
|  Description: program entry point
|
|  Parameters: command line parameters
|
|  Returns: 0 ==> ok
|        else ==> any error
\----------------------------------------------------------------- */
int main(int argc, char *argv[])
{
   QTranslator trans;
   QApplication a(argc, argv);
   a.installTranslator(&trans);
   Recorder w(&trans);
   w.show();
   return a.exec();
}

/************************* History ***************************\
| $Log$
\*************************************************************/

