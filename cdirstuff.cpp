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

/* -----------------------------------------------------------------\
|  Method: CDirStuff / constructor
|  Begin: 22.03.2010 / 08:50:51
|  Author: Jo2003
|  Description: init directory values
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
CDirStuff::CDirStuff()
{
   iInitState = -1;

   if(!fillSysEnvMap())
   {
      iInitState = initDirectories();
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
|  Parameters: --
|
|  Returns: 0 ==> ok
|          -1 ==> ana error
\----------------------------------------------------------------- */
int CDirStuff::initDirectories()
{
   int iRV = 0;
   QMap<QString, QString>::const_iterator cit;
   QDir helpDir;

   cit = mSysEnv.constFind(DATA_DIR_ENV);

   if (cit != mSysEnv.constEnd())
   {
      sDataDir   = QString("%1/%2").arg(*cit).arg(DATA_DIR);
      sLogoDir   = QString("%1/%2").arg(sDataDir).arg(LOGO_DIR);
      sVodPixDir = QString("%1/%2").arg(sDataDir).arg(VOD_DIR);

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
   }
   else
   {
      iRV = -1;
   }

   sAppDir = QApplication::applicationDirPath();

#ifdef QT_NO_DEBUG // normal folder structure ...

#ifdef Q_OS_WIN32
   // -----------------------------------------------------
   //                      Windows
   // -----------------------------------------------------

   // language dir and modules dir will be right
   // in installation folder
   sLangDir = QString("%1/%2").arg(sAppDir).arg(LANG_DIR);
   sModDir  = QString("%1/%2").arg(sAppDir).arg(MOD_DIR);

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
   //       \       \__*.qm (language files)
   //       \__PlugIns
   //           \__modules
   //               \__*.mod (player modules)

   // find bundle dir ...
   QRegExp rx("^(.*)/MacOS");
   if (rx.indexIn(sAppDir) > -1)
   {
      // found section --> create path names ...

      // language path ...
      sLangDir = QString("%1/Resources/%2").arg(rx.cap(1)).arg(LANG_DIR);

      // modules path ...
      sModDir  = QString("%1/PlugIns/%2").arg(rx.cap(1)).arg(MOD_DIR);
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
   //   \__share/vlc-record/*.png         (logo)

   // find out prefix ...
   QRegExp rx("^(.*)/bin$");
   if (rx.indexIn(sAppDir) > -1)
   {
      // found bin section --> create path names ...

      // language path ...
      sLangDir = QString("%1/share/%2/%3").arg(rx.cap(1)).arg(BIN_NAME).arg(LANG_DIR);

      // modules path ...
      sModDir  = QString("%1/share/%2/%3").arg(rx.cap(1)).arg(BIN_NAME).arg(MOD_DIR);
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
   }
#endif // QT_NO_DEBUG

   return iRV;
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
|  Description: get sys environment and store it in mSysEnv map
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

/************************* History ***************************\
| $Log$
\*************************************************************/
