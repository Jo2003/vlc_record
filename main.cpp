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
#include "cvlcrecdb.h"
#include "cshowinfo.h"
#include "qftsettings.h"

#ifdef DINCLUDEPLUGS
#include <QtPlugin>
Q_IMPORT_PLUGIN(qsqlite)
// Q_IMPORT_PLUGIN(qgif)
// Q_IMPORT_PLUGIN(qico)
#endif // DINCLUDEPLUGS

#ifdef Q_WS_X11
   #include <X11/Xlib.h>
#endif

// make logging class available everywhere ...
CLogFile VlcLog;

// make directory names available globally ...
CDirStuff *pFolders;

// db storage class must be global ...
CVlcRecDB *pDb;

// make show info global available ...
CShowInfo showInfo;

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
   // bugfix for crash on exit on *nix ...
#ifdef Q_WS_X11
   XInitThreads();
#endif

   int          iRV = -1;
   QTranslator  trans;
   QApplication app(argc, argv);
   QApplication::installTranslator (&trans);

   pFolders = new CDirStuff();

   if (pFolders)
   {
      if (pFolders->isInitialized ())
      {
         pDb = new CVlcRecDB();

         if (pDb)
         {
            // check if needed settings are there ...
            if ((pDb->stringValue("User") == "")
               && (pDb->stringValue("Passwd") == ""))
            {
               QFTSettings ftSet(NULL, &trans);
               ftSet.exec();
            }

            Recorder rec(&trans);

            rec.show ();

            iRV = app.exec ();

            delete pDb;
         }
      }

      delete pFolders;
   }

   return iRV;
}

/************************* History ***************************\
| $Log$
\*************************************************************/
