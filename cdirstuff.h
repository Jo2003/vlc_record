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
#ifndef __032210__CDIRSTUFF_H
   #define __032210__CDIRSTUFF_H

#include <QtGlobal>
#include <QProcess>
#include <QString>
#include <QStringList>
#include <QApplication>
#include <QRegExp>
#include <QMap>
#include <QDir>

#include "defdef.h"

/********************************************************************\
|  Class: CDirStuff
|  Date:  22.03.2010 / 11:06:32
|  Author: Jo2003
|  Description: class to handle directory name stuff
|
\********************************************************************/
class CDirStuff
{
public:
   CDirStuff();
   ~CDirStuff ();
   const QString& getDataDir ();
   const QString& getLogoDir ();
   const QString& getVodPixDir ();
   const QString& getLangDir ();
   const QString& getModDir ();
   const QString& getAppDir ();
   bool  isInitialized ();

protected:
   int initDirectories ();
   int fillSysEnvMap ();

private:
   QString                sDataDir;
   QString                sLogoDir;
   QString                sVodPixDir;
   QString                sLangDir;
   QString                sModDir;
   QString                sAppDir;
   QMap<QString, QString> mSysEnv;
   int                    iInitState;
};

#endif // __032210__CDIRSTUFF_H
/************************* History ***************************\
| $Log$
\*************************************************************/
