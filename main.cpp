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

#ifdef DINCLUDEPLUGS
#include <QtPlugin>
Q_IMPORT_PLUGIN(qgif)
Q_IMPORT_PLUGIN(qico)
#endif // DINCLUDEPLUGS

// make logging class available everywhere ...
CLogFile VlcLog;

// make directory names available globally ...
CDirStuff *pFolders;

// db storage class must be global ...
CVlcRecDB *pDb;

// make show info global available ...
CShowInfo showInfo;

#ifdef Q_OS_WIN32
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

#else

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
   int           iRV = -1;
   QTranslator  *pTrans = NULL;
   QApplication *pApp   = NULL;
   Recorder     *pRec   = NULL;

   pApp   = new QApplication(argc, argv);
   pTrans = new QTranslator();

   if (pApp && pTrans)
   {
      // install translator ...
      pApp->installTranslator(pTrans);

      // create directory stuff ...
      pFolders = new CDirStuff;

      if (pFolders)
      {
         // is folder stuff initialized successfully ...?
         if (pFolders->isInitialized())
         {
            pDb = new CVlcRecDB();

            if (pDb)
            {
               pRec = new Recorder(pTrans);

               if (pRec)
               {
                  pRec->show();

                  iRV = pApp->exec();

                  pRec->deleteLater();
               }

               delete pDb;
            }
         }

         delete pFolders;
      }
   }

   if (pApp)
   {
      // delete pApp;
      pApp->deleteLater();
   }

   if (pTrans)
   {
      delete pTrans;
   }

   return iRV;
}
#endif

/************************* History ***************************\
| $Log$
\*************************************************************/
