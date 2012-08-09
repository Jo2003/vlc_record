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
#endif // DINCLUDEPLUGS

#if ((defined Q_WS_X11) && (defined INCLUDE_LIBVLC))
   #include <X11/Xlib.h>
#endif

#ifdef Q_OS_MAC
   #include "CocoaInitializer.h"
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
#ifdef Q_OS_MAC
   // in case we need an autoreleasepool ...
   CocoaInitializer *pCocoaInit = new CocoaInitializer();
#endif

   // bugfix for crash on exit on *nix ...
#if ((defined Q_WS_X11) && (defined INCLUDE_LIBVLC))
   XInitThreads();
#endif

   int          iRV = -1;
   QTranslator  trans[Translators::TRANS_MAX];
   QApplication app(argc, argv);
   QApplication::installTranslator (&trans[Translators::TRANS_QT]);
   QApplication::installTranslator (&trans[Translators::TRANS_OWN]);

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
               && (pDb->stringValue("PasswdEnc") == ""))
            {
               QFTSettings ftSet(NULL, trans);
               ftSet.exec();
            }

            Recorder rec(trans);

            rec.show ();

            iRV = app.exec ();

            delete pDb;
         }
      }

      delete pFolders;
   }

#ifdef Q_OS_MAC
   if (pCocoaInit != NULL)
   {
      delete pCocoaInit;
      pCocoaInit = NULL;
   }
#endif

   return iRV;
}

/************************* History ***************************\
| $Log$
\*************************************************************/
