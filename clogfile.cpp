/*********************** Information *************************\
| $HeadURL$
|
| Author: Joerg Neubert
|
| Begin: 19.01.2010 / 15:35:41
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#include "clogfile.h"

/* -----------------------------------------------------------------\
|  Method: CLogFile / constructor
|  Begin: 19.01.2010 / 15:36:07
|  Author: Joerg Neubert
|  Description: construct object, init values
|
|  Parameters: log dir, log file, log level
|
|  Returns: --
\----------------------------------------------------------------- */
CLogFile::CLogFile(const char *pDirName, const char *pFileName, vlclog::eLogLevel ll)
{
   level     = ll;
   sDirName  = pDirName;
   sFileName = pFileName;

   TouchLogFile();
}

/* -----------------------------------------------------------------\
|  Method: ~CLogFile / destructor
|  Begin: 19.01.2010 / 15:36:58
|  Author: Joerg Neubert
|  Description: clean at destruction
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
CLogFile::~CLogFile()
{
   // nothing to do ...
   if (logFile.isOpen())
   {
      logFile.close();
   }
}

/* -----------------------------------------------------------------\
|  Method: TouchLogFile
|  Begin: 19.01.2010 / 15:37:46
|  Author: Joerg Neubert
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
      if (logFile.isOpen())
      {
         logFile.close();
      }

      logFile.setFileName(QString("%1/%2").arg(sDirName).arg(sFileName));
      logFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text);
   }
}

/* -----------------------------------------------------------------\
|  Method: SetLogFile
|  Begin: 19.01.2010 / 15:38:07
|  Author: Joerg Neubert
|  Description: set filename and dir name for logging
|
|  Parameters: log dir, log file
|
|  Returns: --
\----------------------------------------------------------------- */
void CLogFile::SetLogFile(const char *pDirName, const char *pFileName)
{
   sDirName  = pDirName;
   sFileName = pFileName;

   TouchLogFile();
}

/* -----------------------------------------------------------------\
|  Method: SetLogLevel
|  Begin: 19.01.2010 / 15:38:42
|  Author: Joerg Neubert
|  Description: set log level
|
|  Parameters: new log level
|
|  Returns: --
\----------------------------------------------------------------- */
void CLogFile::SetLogLevel(vlclog::eLogLevel ll)
{
   level = ll;

   if (!logFile.isOpen())
   {
      TouchLogFile();
   }
}

/* -----------------------------------------------------------------\
|  Method: WriteLog
|  Begin: 19.01.2010 / 15:39:08
|  Author: Joerg Neubert
|  Description: internal function to write string to log file
|
|  Parameters: string to write
|
|  Returns: --
\----------------------------------------------------------------- */
void CLogFile::WriteLog(const QString &str)
{
   if (logFile.isOpen())
   {
      mutex.lock();
      QTextStream stream(&logFile);
      stream.setCodec("UTF-8");
      stream << QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss.zzz") << ": " << str.toUtf8();
      mutex.unlock();
   }
}

/* -----------------------------------------------------------------\
|  Method: GetLogLevel
|  Begin: 19.01.2010 / 15:39:38
|  Author: Joerg Neubert
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
|  Author: Joerg Neubert
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
|  Author: Joerg Neubert
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
|  Author: Joerg Neubert
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
