/*********************** Information *************************\
| $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/clogfile.h $
|
| Author: Jo2003
|
| Begin: 19.01.2010 / 15:34:39
|
| Last edited by: $Author: Olenka.Joerg $
|
| $Id: clogfile.h 880 2012-08-14 13:50:59Z Olenka.Joerg $
\*************************************************************/
#ifndef __011910__CLOGFILE_H
   #define __011910__CLOGFILE_H

#include <QString>
#include <QMutex>
#include <QTextStream>
#include <QDateTime>
#include <cstdio>
#include <QFile>

//===================================================================
// some macros to make logging easier ...
//===================================================================
#define mInfo(a) VlcLog.LogInfo (QString("%1 / %2():%3: %4").arg(__FILE__).arg(__FUNCTION__).arg(__LINE__).arg(a))
#define mWarn(a) VlcLog.LogWarn (QString("%1 / %2():%3: %4").arg(__FILE__).arg(__FUNCTION__).arg(__LINE__).arg(a))
#define mErr(a) VlcLog.LogErr (QString("%1 / %2():%3: %4").arg(__FILE__).arg(__FUNCTION__).arg(__LINE__).arg(a))
#define mLog(a) mErr(a)

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
   CLogFile(const QString& sDir = QString(), const QString& sFile = QString(),
            vlclog::eLogLevel ll = vlclog::LOG_NOTHING);

   ~CLogFile ();

   void    SetLogFile (const QString& sDir, const QString& sFile);
   void    SetLogLevel (vlclog::eLogLevel ll = vlclog::LOG_NOTHING);
   void    LogErr (const QString &str);
   void    LogWarn (const QString &str);
   void    LogInfo (const QString &str);

   vlclog::eLogLevel GetLogLevel ();

private:
   bool bInit;
   vlclog::eLogLevel level;
   void    WriteLog (const QString &str);
   void    TouchLogFile ();

   QString     sFileName;
   QString     sDirName;
   QTextStream logStream;
   QFile       fLogFile;
   QMutex      mutex;
};

#endif /* __011910__CLOGFILE_H */
/************************* History ***************************\
| $Log$
\*************************************************************/

