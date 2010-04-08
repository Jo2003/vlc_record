/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 19.01.2010 / 15:57:06
|
| Last edited by: $Author$
|
| $Id$
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

// make directory names available globally ...
CDirStuff *pFolders;

// make playstate available globally ...
CPlayState playState;

/* -----------------------------------------------------------------\
|  Method: main / program entry
|  Begin: 19.01.2010 / 15:57:36
|  Author: Jo2003
|  Description: program entry point
|
|  Parameters: command line parameters
|
|  Returns: 0 ==> ok
|        else ==> any error
\----------------------------------------------------------------- */
int main(int argc, char *argv[])
{
   int          iRV = -1;
   QTranslator  trans;
   QApplication a(argc, argv);
   a.installTranslator(&trans);

   // create directory stuff ...
   pFolders = new CDirStuff;

   if (pFolders)
   {
      // is folder stuff initialized successfully ...?
      if (pFolders->isInitialized())
      {
         Recorder w(&trans);
         w.show();
         iRV = a.exec();
      }

      // free mem ...
      delete pFolders;
   }

   return iRV;
}

/************************* History ***************************\
| $Log$
\*************************************************************/
