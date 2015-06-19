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
#ifdef __QT_5
   #include <QtWidgets/QApplication>
#else
   #include <QtGui/QApplication>
#endif

#include <QTranslator>
#include <QFont>
#include <QSysInfo>
#include "recorder.h"
#include "cvlcrecdb.h"
#include "cshowinfo.h"
#include "qftsettings.h"
#include "qfusioncontrol.h"
#include "qcustparser.h"
#include "chtmlwriter.h"
#include "qchannelmap.h"
#include "qdatetimesyncro.h"
#include "qstatemessage.h"
#include "cdirstuff.h"
#include "qwatchstats.h"
#include "qurlex.h"

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

// we need syncronized time ...
QDateTimeSyncro tmSync;

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

// global html writer ...
CHtmlWriter *pHtml;

// global channel map ...
QChannelMap *pChanMap;

// global state message engine ...
QStateMessage *pStateMsg;

// global watch statistics
QWatchStats   *pWatchStats;

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

   qRegisterMetaType<vlcvid::SContextAction>("vlcvid::SContextAction");
   qRegisterMetaType<QLangVector>("QLangVector");
   qRegisterMetaType<QUrlEx>("QUrlEx");

#ifdef Q_OS_MACX
   if ( QSysInfo::MacintoshVersion > QSysInfo::MV_10_8 )
   {
       // fix Mac OS X 10.9 (mavericks) font issue
       // https://bugreports.qt-project.org/browse/QTBUG-32789
       QFont::insertSubstitution(".Lucida Grande UI", "Lucida Grande");
   }
#endif

   int          iRV = -1;
   QApplication app(argc, argv);
   Recorder    *pRec;
   QFTSettings *pFTSet;

   // Setting "app" as parent puts the new generated objects into Qt's memory management,
   // so no delete is needed since Qt takes care ...
   pAppTransl  = new QTranslator(&app);
   pQtTransl   = new QTranslator(&app);
   pFolders    = new CDirStuff(&app);
   pHtml       = new CHtmlWriter(&app);
   pWatchStats = new QWatchStats(&app);
   pChanMap    = new QChannelMap();
   pStateMsg   = new QStateMessage(); // will be parented in recorder.cpp::Recorder()!

   if (pFolders && pAppTransl && pQtTransl && pHtml && pChanMap && pWatchStats)
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

            pApiClient = new ApiClient(&app);
            pApiParser = new ApiParser(&app);

            // The database is the last service used.
            // Make sure it destroyed latest!
            // Therefore we don't set app as parent!
            pDb        = new CVlcRecDB();

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

            // delete database ...
            if (pDb)
            {
               delete pDb;
               pDb = NULL;
            }
         }
      }
   }

   if (pChanMap)
   {
      delete pChanMap;
      pChanMap = NULL;
   }

   return iRV;
}

/************************* History ***************************\
| $Log$
\*************************************************************/
