/*********************** Information *************************\
| $HeadURL: https://vlc-record.googlecode.com/svn/branches/sunduk.tv/clogfile.cpp $
|
| Author: Jo2003
|
| Begin: 19.01.2010 / 15:35:41
|
| Last edited by: $Author: Olenka.Joerg $
|
| $Id: clogfile.cpp 881 2012-08-14 13:57:22Z Olenka.Joerg $
\*************************************************************/
#include "clogfile.h"
#include "defdef.h"

/* -----------------------------------------------------------------\
|  Method: CLogFile / constructor
|  Begin: 19.01.2010 / 15:36:07
|  Author: Jo2003
|  Description: construct object, init values
|
|  Parameters: log dir, log file, log level
|
|  Returns: --
\----------------------------------------------------------------- */
CLogFile::CLogFile (const QString &sDir, const QString &sFile, vlclog::eLogLevel ll)
   : bInit(false)
{
   level     = ll;
   sDirName  = sDir;
   sFileName = sFile;

   TouchLogFile();
}

/* -----------------------------------------------------------------\
|  Method: ~CLogFile / destructor
|  Begin: 19.01.2010 / 15:36:58
|  Author: Jo2003
|  Description: clean at destruction
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
CLogFile::~CLogFile()
{
   if (bInit)
   {
      fclose(stderr);
   }

   if (fLogFile.isOpen())
   {
      fLogFile.close();
   }
}

/* -----------------------------------------------------------------\
|  Method: TouchLogFile
|  Begin: 19.01.2010 / 15:37:46
|  Author: Jo2003
|  Description: open / clean log file if needed
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CLogFile::TouchLogFile ()
{
   // truncate log file ...
   if ((sDirName != "") && (sFileName != "") && (level > vlclog::LOG_NOTHING))
   {
      if (!bInit)
      {
         // normal logfile ...
         QString fileName = QString("%1/%2").arg(sDirName).arg(sFileName);

         if (fLogFile.isOpen())
         {
            fLogFile.close();
         }

         fLogFile.setFileName(fileName);

         if ((bInit = fLogFile.open(QIODevice::Text | QIODevice::WriteOnly | QIODevice::Truncate)) == true)
         {
            logStream.setDevice(&fLogFile);
            logStream.setCodec("UTF-8");

            // create own output file for libVlc ...
            fileName = QString("%1/%2").arg(sDirName).arg(LIBVLC_LOG_FILE);

            if (!freopen(fileName.toUtf8().constData(), "w", stderr))
            {
               WriteLog("Can't create logfile for libVLC!");
            }
         }
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: SetLogFile
|  Begin: 19.01.2010 / 15:38:07
|  Author: Jo2003
|  Description: set filename and dir name for logging
|
|  Parameters: log dir, log file
|
|  Returns: --
\----------------------------------------------------------------- */
void CLogFile::SetLogFile (const QString &sDir, const QString &sFile)
{
   sDirName  = sDir;
   sFileName = sFile;

   TouchLogFile();
}

/* -----------------------------------------------------------------\
|  Method: SetLogLevel
|  Begin: 19.01.2010 / 15:38:42
|  Author: Jo2003
|  Description: set log level
|
|  Parameters: new log level
|
|  Returns: --
\----------------------------------------------------------------- */
void CLogFile::SetLogLevel(vlclog::eLogLevel ll)
{
   level = ll;
   TouchLogFile();
}

/* -----------------------------------------------------------------\
|  Method: WriteLog
|  Begin: 19.01.2010 / 15:39:08
|  Author: Jo2003
|  Description: internal function to write string to log file
|
|  Parameters: string to write
|
|  Returns: --
\----------------------------------------------------------------- */
void CLogFile::WriteLog(const QString &str)
{
   if (bInit)
   {
      mutex.lock();
      logStream << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz") << ": " << str << endl;
      mutex.unlock();
   }
}

/* -----------------------------------------------------------------\
|  Method: GetLogLevel
|  Begin: 19.01.2010 / 15:39:38
|  Author: Jo2003
|  Description: get log level
|
|  Parameters: --
|
|  Returns: log level
\----------------------------------------------------------------- */
vlclog::eLogLevel CLogFile::GetLogLevel()
{
   return level;
}

/* -----------------------------------------------------------------\
|  Method: LogInfo
|  Begin: 19.01.2010 / 15:40:00
|  Author: Jo2003
|  Description: log string at info level
|
|  Parameters: string to log
|
|  Returns: --
\----------------------------------------------------------------- */
void CLogFile::LogInfo(const QString &str)
{
   if (level == vlclog::LOG_ALL)
   {
      WriteLog(str);
   }
}

/* -----------------------------------------------------------------\
|  Method: LogWarn
|  Begin: 19.01.2010 / 15:40:00
|  Author: Jo2003
|  Description: log string at warning level
|
|  Parameters: string to log
|
|  Returns: --
\----------------------------------------------------------------- */
void CLogFile::LogWarn(const QString &str)
{
   if (level >= vlclog::LOG_WARN)
   {
      WriteLog(str);
   }
}

/* -----------------------------------------------------------------\
|  Method: LogErr
|  Begin: 19.01.2010 / 15:40:00
|  Author: Jo2003
|  Description: log string at error level
|
|  Parameters: string to log
|
|  Returns: --
\----------------------------------------------------------------- */
void CLogFile::LogErr(const QString &str)
{
   if (level >= vlclog::LOG_ERR)
   {
      WriteLog(str);
   }
}

/************************* History ***************************\
| $Log$
\*************************************************************/
