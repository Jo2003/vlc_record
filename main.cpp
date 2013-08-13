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
#include "qfusioncontrol.h"
#include "qcustparser.h"
#include "ctimeshift.h"
#include "chtmlwriter.h"

#ifdef DINCLUDEPLUGS
#include <QtPlugin>
Q_IMPORT_PLUGIN(qsqlite)
#endif // DINCLUDEPLUGS

#if ((defined Q_WS_X11) && (defined INCLUDE_LIBVLC))
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

// fusion control ...
QFusionControl missionControl;

// customization ...
QCustParser *pCustomization;

// global api client and parser ...
ApiClient   *pApiClient;
ApiParser   *pApiParser;

// global translaters ...
QTranslator *pAppTransl;
QTranslator *pQtTransl;

// global timeshift class ...
CTimeShift *pTs;

// global html writer ...
CHtmlWriter *pHtml;

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
#if ((defined Q_WS_X11) && (defined INCLUDE_LIBVLC))
   XInitThreads();
#endif

   qRegisterMetaType<vlcvid::SContextAction>("vlcvid::SContextAction");
   qRegisterMetaType<QLangVector>("QLangVector");

   int          iRV = -1;
   QApplication app(argc, argv);
   Recorder    *pRec;
   QFTSettings *pFTSet;

   // Setting "app" as parent puts the new generated objects into Qt's memory management,
   // so no delete is needed since Qt takes care ...
   pAppTransl = new QTranslator(&app);
   pQtTransl  = new QTranslator(&app);
   pFolders   = new CDirStuff(&app);
   pTs        = new CTimeShift(&app);
   pHtml      = new CHtmlWriter(&app);

   if (pFolders && pAppTransl && pQtTransl && pTs && pHtml)
   {
      if (pFolders->isInitialized ())
      {
         if ((pCustomization = new QCustParser(&app)) != NULL)
         {
            pCustomization->parseCust();

            pFolders->setAppName(pCustomization->strVal("APP_NAME"));

            // make sure debug stuff is written from the very begining ...
            VlcLog.SetLogFile(pFolders->getDataDir(), QString("%1.log").arg(pFolders->getBinName()));
            VlcLog.SetLogLevel(vlclog::LOG_ALL);

            QApplication::installTranslator (pQtTransl);
            QApplication::installTranslator (pAppTransl);

            pDb        = new CVlcRecDB(&app);
            pApiClient = new ApiClient(&app);
            pApiParser = new ApiParser(&app);

            if (pDb && pApiClient && pApiParser)
            {
               // check if needed settings are there ...
               if ((pDb->stringValue("User") == "")
                  && (pDb->stringValue("PasswdEnc") == ""))
               {
                  if ((pFTSet = new QFTSettings()) != NULL)
                  {
                     pFTSet->exec();
                     delete pFTSet;
                     pFTSet = NULL;
                  }
               }

               if ((pRec = new Recorder()) != NULL)
               {
                  pRec->show();
                  iRV = app.exec ();
                  delete pRec;
                  pRec = NULL;
               }
            }
         }
      }
   }

   return iRV;
}

/************************* History ***************************\
| $Log$
\*************************************************************/
