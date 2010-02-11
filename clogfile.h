/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 19.01.2010 / 15:34:39
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#ifndef __011910__CLOGFILE_H
   #define __011910__CLOGFILE_H

#include <QFile>
#include <QString>
#include <QMutex>
#include <QTextStream>
#include <QDateTime>

//===================================================================
// some macros to make logging easier ...
//===================================================================
#define mInfo(a) VlcLog.LogInfo (QString("%1 / %2():%3: %4").arg(__FILE__).arg(__FUNCTION__).arg(__LINE__).arg(a))
#define mWarn(a) VlcLog.LogWarn (QString("%1 / %2():%3: %4").arg(__FILE__).arg(__FUNCTION__).arg(__LINE__).arg(a))
#define mErr(a) VlcLog.LogErr (QString("%1 / %2():%3: %4").arg(__FILE__).arg(__FUNCTION__).arg(__LINE__).arg(a))

//===================================================================
// namespace
//===================================================================
namespace vlclog
{
   class CLogFile;
   enum eLogLevel
   {
      LOG_NOTHING,
      LOG_ERR,
      LOG_WARN,
      LOG_ALL
   };
}

/********************************************************************\
|  Class: CLogFile
|  Date:  19.01.2010 / 15:35:20
|  Author: Jo2003
|  Description: a simple logging class
|
\********************************************************************/
class CLogFile
{
public:
   CLogFile(const char *pDirName = "", const char *pFileName = "", vlclog::eLogLevel ll = vlclog::LOG_NOTHING);
   ~CLogFile ();

   void    SetLogFile (const char *pDirName, const char *pFileName);
   void    SetLogLevel (vlclog::eLogLevel ll = vlclog::LOG_NOTHING);
   void    LogErr (const QString &str);
   void    LogWarn (const QString &str);
   void    LogInfo (const QString &str);

   vlclog::eLogLevel GetLogLevel ();

private:
   vlclog::eLogLevel level;
   void    WriteLog (const QString &str);
   void    TouchLogFile ();
   QString sFileName;
   QString sDirName;
   QFile   logFile;
   QMutex  mutex;
};

#endif /* __011910__CLOGFILE_H */
/************************* History ***************************\
| $Log$
\*************************************************************/

