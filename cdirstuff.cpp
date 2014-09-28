/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 22.03.2010 / 08:45:22
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#include "cdirstuff.h"
#include <QLibraryInfo>

/* -----------------------------------------------------------------\
|  Method: CDirStuff / constructor
|  Begin: 22.03.2010 / 08:50:51
|  Author: Jo2003
|  Description: init directory values
|
|  Parameters: pointer to parent object
|
|  Returns: --
\----------------------------------------------------------------- */
CDirStuff::CDirStuff(QObject *parent) : QObject(parent)
{
   iInitState = -1;
   bPortable  = false;

   if(!fillSysEnvMap())
   {
      iInitState = initDirectories(false);
   }
}

/* -----------------------------------------------------------------\
|  Method: ~CDirStuff / destructor
|  Begin: 22.03.2010 / 08:50:51
|  Author: Jo2003
|  Description: clean on destruction
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
CDirStuff::~CDirStuff()
{
   // nothing to do so far ...
}

/* -----------------------------------------------------------------\
|  Method: fillSysEnvMap
|  Begin: 22.03.2010 / 08:55:51
|  Author: Jo2003
|  Description: get sys environment and store it in mSysEnv map
|
|  Parameters: --
|
|  Returns: 0 ==> ok
|          -1 ==> any error
\----------------------------------------------------------------- */
int CDirStuff::fillSysEnvMap()
{
   int iRV = -1;

   // one string contains environment in form:
   // KEY=VALUE
   // parse it using regular expression ...
   QRegExp rx("^(.*)=(.*)$");

   // get sys environment stringlist ...
   QStringList slSysEnv = QProcess::systemEnvironment();
   QStringList::const_iterator cit;

   // get string by string, parse and fill in into map ...
   for (cit = slSysEnv.constBegin(); cit != slSysEnv.constEnd(); cit ++)
   {
      if (rx.indexIn(*cit) > -1)
      {
         mSysEnv.insert(rx.cap(1), rx.cap(2));
      }
   }

   if (mSysEnv.count() > 0)
   {
      iRV = 0;
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: initDirectories
|  Begin: 22.03.2010 / 08:50:51
|  Author: Jo2003
|  Description: init / fill all directory variables
|               dependent on OS
|  Parameters: create flag
|
|  Returns: 0 ==> ok
|          -1 ==> ana error
\----------------------------------------------------------------- */
int CDirStuff::initDirectories(bool bCreate)
{
   int iRV = 0;
   QMap<QString, QString>::const_iterator cit;
   QDir    helpDir;
   QString homeFolder;

   sBinName   = QFileInfo(QApplication::applicationFilePath()).baseName();
   cit        = mSysEnv.constFind(DATA_DIR_ENV);

   // app folder
   sAppDir    = QApplication::applicationDirPath();

   // check for portable version ...
   if (QFile(QString("%1/%2").arg(sAppDir).arg(PORTABLE_MARKER)).exists())
   {
      bPortable = true;
   }

   if (bPortable)
   {
      homeFolder = QString("%1/%2").arg(sAppDir).arg(PORTABLE_DATA_DIR);
   }
   else
   {
      homeFolder = (cit != mSysEnv.constEnd()) ? (*cit) : "";
   }

   if (homeFolder != "")
   {
#ifdef Q_OS_WIN32
      sDataDir   = QString("%1/%2").arg(homeFolder).arg((sAppName == "" ) ? sBinName : sAppName);
      sDataDir.replace("\\", "/");
#else
      sDataDir   = QString("%1/%2").arg(homeFolder).arg("." + ((sAppName == "" ) ? sBinName : sAppName));
#endif
      sLogoDir   = QString("%1/%2").arg(sDataDir).arg(LOGO_DIR);
      sVodPixDir = QString("%1/%2").arg(sDataDir).arg(VOD_DIR);

      if (bCreate)
      {
         // check, if dir exists ...
         helpDir.setPath(sDataDir);
         if (!helpDir.exists())
         {
            helpDir.mkpath(sDataDir);
         }

         helpDir.setPath(sLogoDir);
         if (!helpDir.exists())
         {
            helpDir.mkpath(sLogoDir);
         }

         helpDir.setPath(sVodPixDir);
         if (!helpDir.exists())
         {
            helpDir.mkpath(sVodPixDir);
         }
      }
   }
   else
   {
      iRV = -1;
   }

   // temp folder ...
   sTmpFolder = QDir::tempPath();

   // qt languages dir ...
   sQtLangDir = QLibraryInfo::location(QLibraryInfo::TranslationsPath);

#ifdef QT_NO_DEBUG // normal folder structure ...

#ifdef Q_OS_WIN32
   // -----------------------------------------------------
   //                      Windows
   // -----------------------------------------------------

   // make sure to convert backslash into slash ...
   sAppDir.replace("\\", "/");
   sQtLangDir.replace("\\", "/");

   // language dir, modules dir and translations dir
   // will be right in installation folder
   sLangDir   = QString("%1/%2").arg(sAppDir).arg(LANG_DIR);
   sModDir    = QString("%1/%2").arg(sAppDir).arg(MOD_DIR);
   sQtLangDir = QString("%1/%2").arg(sAppDir).arg(LANG_DIR_QT);
   sDocDir    = QString("%1/%2").arg(sAppDir).arg(DOC_DIR);
   sResDir    = QString("%1/%2").arg(sAppDir).arg(RES_DIR);

#elif defined Q_OS_MAC
   // -----------------------------------------------------
   //                      MacOS
   // -----------------------------------------------------

   // In MacOS we use application bundles. The structure
   // of the vlc-record app bundle should look like this:
   // vlc-record.app
   //   \__Contents
   //       \__MacOS
   //       \   \__vlc-record (binary)
   //       \__Resources
   //       \   \__language
   //       \       \__*.qm (own language files)
   //       \__PlugIns
   //       \   \__modules
   //       \       \__*.mod (player modules)
   //       \__translations
   //           \__*.qm (Qt language files)

   // find bundle dir ...
   QRegExp rx("^(.*)/MacOS");
   if (rx.indexIn(sAppDir) > -1)
   {
      // found section --> create path names ...

      // language path ...
      sLangDir = QString("%1/Resources/%2").arg(rx.cap(1)).arg(LANG_DIR);

      // resources path ...
      sResDir  = QString("%1/Resources").arg(rx.cap(1));

      // modules path ...
      sModDir  = QString("%1/PlugIns/%2").arg(rx.cap(1)).arg(MOD_DIR);

      // docu folder ...
      sDocDir  = QString("%1/Resources/%2").arg(rx.cap(1)).arg(DOC_DIR);
   }
   else
   {
      iRV = -1;
   }

#else
   // -----------------------------------------------------
   //                      Linux / Unix
   // -----------------------------------------------------

   // in linux / unix we have to use 'bin' and 'share' to cleanly
   // separate application from shared stuff ...
   // so the directory structure should look like this:
   // \__${PREFIX}
   //   \__bin/vlc-record (binary)
   //   \__share/vlc-record/modules/*.mod (player modules)
   //   \__share/vlc-record/language/*.qm (language files)
   //   \__share/vlc-record/resources/*.cust (language files)
   //   \__share/vlc-record/*.png         (logo)

   // find out prefix ...
   QRegExp rx("^(.*)/bin$");
   if (rx.indexIn(sAppDir) > -1)
   {
      // found bin section --> create path names ...

      // language path ...
      sLangDir   = QString("%1/share/%2/%3").arg(rx.cap(1)).arg(sBinName).arg(LANG_DIR);

      // modules path ...
      sModDir    = QString("%1/share/%2/%3").arg(rx.cap(1)).arg(sBinName).arg(MOD_DIR);

      // docu folder ...
      sDocDir    = QString("%1/share/%2/%3").arg(rx.cap(1)).arg(sBinName).arg(DOC_DIR);

      // resources folder ...
      sResDir    = QString("%1/share/%2/%3").arg(rx.cap(1)).arg(sBinName).arg(RES_DIR);
   }
   else
   {
      iRV = -1;
   }

#endif // OS dependent

#else // debug case ...
   QRegExp rx("^(.*)/debug$");
   if (rx.indexIn(sAppDir) > -1)
   {
      sLangDir = rx.cap(1);
      sModDir  = QString("%1/%2").arg(rx.cap(1)).arg(MOD_DIR);
      sDocDir  = sLangDir + "/documentation";
      sResDir  = sLangDir;
   }
#endif // QT_NO_DEBUG

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: setAppName
|  Begin: 11.09.2012
|  Author: Jo2003
|  Description: set application name (got from customization)
|
|  Parameters: ref. to app name
|
|  Returns: --
\----------------------------------------------------------------- */
void CDirStuff::setAppName(const QString &name)
{
   sAppName   = name;
   iInitState = initDirectories(true);
}

/* -----------------------------------------------------------------\
|  Method: getAppDir
|  Begin: 22.03.2010 / 10:55:51
|  Author: Jo2003
|  Description: get application directory
|
|  Parameters: --
|
|  Returns: ref. to app dir string
\----------------------------------------------------------------- */
const QString& CDirStuff::getAppDir()
{
   return sAppDir;
}

/* -----------------------------------------------------------------\
|  Method: getDataDir
|  Begin: 22.03.2010 / 10:55:51
|  Author: Jo2003
|  Description: get data directory
|
|  Parameters: --
|
|  Returns: ref. to data dir string
\----------------------------------------------------------------- */
const QString& CDirStuff::getDataDir()
{
   return sDataDir;
}

/* -----------------------------------------------------------------\
|  Method: getBinName
|  Begin: 11.09.2012
|  Author: Jo2003
|  Description: get bin name
|
|  Parameters: --
|
|  Returns: ref. to bin name string
\----------------------------------------------------------------- */
const QString& CDirStuff::getBinName ()
{
   return sBinName;
}

/* -----------------------------------------------------------------\
|  Method: getDocDir
|  Begin: 09.07.2012
|  Author: Jo2003
|  Description: get doc directory
|
|  Parameters: --
|
|  Returns: ref. to doc dir string
\----------------------------------------------------------------- */
const QString& CDirStuff::getDocDir()
{
   return sDocDir;
}

/* -----------------------------------------------------------------\
|  Method: getLangDir
|  Begin: 22.03.2010 / 10:55:51
|  Author: Jo2003
|  Description: get language directory
|
|  Parameters: --
|
|  Returns: ref. to language dir string
\----------------------------------------------------------------- */
const QString& CDirStuff::getLangDir()
{
   return sLangDir;
}

/* -----------------------------------------------------------------\
|  Method: getQtLangDir
|  Begin: 18.04.2012
|  Author: Jo2003
|  Description: get Qt language directory
|
|  Parameters: --
|
|  Returns: ref. to Qt language dir string
\----------------------------------------------------------------- */
const QString& CDirStuff::getQtLangDir()
{
   return sQtLangDir;
}

/* -----------------------------------------------------------------\
|  Method: getResDir
|  Begin: 11.09.2012
|  Author: Jo2003
|  Description: get resources directory
|
|  Parameters: --
|
|  Returns: ref. to resources dir string
\----------------------------------------------------------------- */
const QString& CDirStuff::getResDir()
{
   return sResDir;
}

/* -----------------------------------------------------------------\
|  Method: getLogoDir
|  Begin: 22.03.2010 / 10:55:51
|  Author: Jo2003
|  Description: get logo directory
|
|  Parameters: --
|
|  Returns: ref. to logo dir string
\----------------------------------------------------------------- */
const QString& CDirStuff::getLogoDir()
{
   return sLogoDir;
}

/* -----------------------------------------------------------------\
|  Method: getVodPixDir
|  Begin: 21.12.2010 / 11:55
|  Author: Jo2003
|  Description: get vod directory
|
|  Parameters: --
|
|  Returns: ref. to vod dir string
\----------------------------------------------------------------- */
const QString& CDirStuff::getVodPixDir()
{
   return sVodPixDir;
}

/* -----------------------------------------------------------------\
|  Method: getModDir
|  Begin: 22.03.2010 / 10:55:51
|  Author: Jo2003
|  Description: get modules directory
|
|  Parameters: --
|
|  Returns: ref. to modules dir string
\----------------------------------------------------------------- */
const QString& CDirStuff::getModDir()
{
   return sModDir;
}

/* -----------------------------------------------------------------\
|  Method: fillSysEnvMap
|  Begin: 22.03.2010 / 08:55:51
|  Author: Jo2003
|  Description: get sys environment and store it in mSysEnv mapQDir::tempPath()
|
|  Parameters: --
|
|  Returns: 0 ==> ok
|          -1 ==> any error
\----------------------------------------------------------------- */
bool CDirStuff::isInitialized()
{
   return (iInitState) ? false : true;
}

/* -----------------------------------------------------------------\
|  Method: getTmpFolder
|  Begin: 17.12.2013
|  Author: Jo2003
|  Description: get temp folder
|
|  Parameters: --
|
|  Returns: temp folder
\----------------------------------------------------------------- */
const QString&  CDirStuff::getTmpFolder()
{
   return sTmpFolder;
}

//---------------------------------------------------------------------------
//
//! \brief   is portable version running?
//
//! \author  Jo2003
//! \date    15.03.2013
//
//! \return  true -> is portable; false -> non portable version
//---------------------------------------------------------------------------
bool CDirStuff::portable()
{
   return bPortable;
}

/************************* History ***************************\
| $Log$
\*************************************************************/
