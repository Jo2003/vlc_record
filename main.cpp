/*********************** Information *************************\
| $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/main.cpp $
|
| Author: Jo2003
|
| Begin: 19.01.2010 / 15:57:06
|
| Last edited by: $Author: Olenka.Joerg $
|
| $Id: main.cpp 1275 2013-12-19 12:33:03Z Olenka.Joerg $
\*************************************************************/
#include <QtGui/QApplication>
#include <QTranslator>
#include <QFont>
#include <QSysInfo>
#include "recorder.h"
#include "cvlcrecdb.h"
#include "cshowinfo.h"
#include "qftsettings.h"
#include "qfusioncontrol.h"
#include "qcustparser.h"
#include "ctimeshift.h"
#include "chtmlwriter.h"
#include "qchannelmap.h"

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

// global channel map ...
QChannelMap *pChanMap;

/// build timestamp ...
extern const char* pBuilt;

int iExpiresInDays;

//---------------------------------------------------------------------------
//
//! \brief   check if program has expired
//
//! \author  Jo2003
//! \date    30.03.2016
//
//! \return  true -> expired; false -> not expired
//---------------------------------------------------------------------------
bool expired()
{
    bool ret;

    if (EXPIRES_IN == -1)
    {
        ret            = false;
        iExpiresInDays = -1;
    }
    else
    {
        // build stamp is done in english locale ...
        QLocale engLoc(QLocale::English);

        // when program will expire?
        time_t  compare = engLoc.toDateTime(pBuilt, "MMM dd yyyy, hh:mm:ss").toTime_t() + (time_t)(EXPIRES_IN * 24 * 3600);

        ret            = (QDateTime::currentDateTime().toTime_t() > compare);
        iExpiresInDays = QDateTime::currentDateTime().daysTo(QDateTime::fromTime_t(compare));

        QString msg;

        if (ret)
        {
            msg = QString("This BETA has expired! If you are a legal user you know where to get an update from!");
        }
        else if (iExpiresInDays <= 5)
        {
            msg = QString("This BETA version expires in %1 day(s)!").arg(iExpiresInDays);
        }

        if (!msg.isEmpty())
        {
            QMessageBox::warning(NULL, "Note", msg);
        }
    }

    return ret;
}

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
   pAppTransl = new QTranslator(&app);
   pQtTransl  = new QTranslator(&app);
   pFolders   = new CDirStuff(&app);
   pTs        = new CTimeShift(&app);
   pHtml      = new CHtmlWriter(&app);
   pChanMap   = new QChannelMap();

   if (pFolders && pAppTransl && pQtTransl && pTs && pHtml && pChanMap && !expired())
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

            if (iExpiresInDays > -1) mLog(QString("Program expires in %1 day(s)!").arg(iExpiresInDays));

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
