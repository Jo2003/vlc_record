/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 18.01.2010 / 16:25:48
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#include "cinifile.h"

/* -----------------------------------------------------------------\
|  Method: CIniFile / constructor
|  Begin: 18.01.2010 / 16:26:12
|  Author: Jo2003
|  Description: construct element
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
CIniFile::CIniFile()
{
   // nothing to do ...
}

/* -----------------------------------------------------------------\
|  Method: ~CIniFile / destructor
|  Begin: 18.01.2010 / 16:26:36
|  Author: Jo2003
|  Description: clean on dstruction
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
CIniFile::~CIniFile()
{
   // nothing to do ...
}

/* -----------------------------------------------------------------\
|  Method: CIniFile / constructor
|  Begin: 18.01.2010 / 16:27:07
|  Author: Jo2003
|  Description: create object, set values
|
|  Parameters: ini file dir, file name
|
|  Returns: --
\----------------------------------------------------------------- */
CIniFile::CIniFile (const char *sDir, const char *sFile)
{
   sIniFile = sFile;
   sAppDir  = sDir;
}

/* -----------------------------------------------------------------\
|  Method: SetFileName
|  Begin: 18.01.2010 / 16:27:07
|  Author: Jo2003
|  Description: set file / path values
|
|  Parameters: ini file dir, file name
|
|  Returns: --
\----------------------------------------------------------------- */
void CIniFile::SetFileName(const char *sDir, const char *sFile)
{
   sIniFile = sFile;
   sAppDir  = sDir;
}

/* -----------------------------------------------------------------\
|  Method: ReadIni
|  Begin: 18.01.2010 / 16:28:01
|  Author: Jo2003
|  Description: read ini file and store values in internal buffer
|
|  Parameters: --
|
|  Returns: 0 ==> ok
|          -1 ==> any error
\----------------------------------------------------------------- */
int CIniFile::ReadIni()
{
   QString        sLine;
   QRegExp        rx(QString("^([^ \t]*) = (.*)[ \t]*$"));
   QString        iniFileName = QString("%1/%2").arg(sAppDir).arg(sIniFile);
   QFile          fIni(iniFileName);
   Ini::SIniEntry entry;
   int            iRV  = -1;

   if (fIni.open(QIODevice::ReadOnly | QIODevice::Text))
   {
      QTextStream str(&fIni);

      do
      {
         iRV   = 0;
         sLine = str.readLine();

         if (sLine.length() > 0)
         {
            if (rx.indexIn(sLine) > -1)
            {
               // found entry ...
               entry.sKey  = rx.cap(1);
               entry.sData = rx.cap(2);
               IniEntries.push_back(entry);
            }
         }
      }
      while (!str.atEnd());

      fIni.close();
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: DelData
|  Begin: 18.01.2010 / 16:28:37
|  Author: Jo2003
|  Description: delete data from internal buffer
|
|  Parameters: key
|
|  Returns: 0 ==> key not found
|        else ==> deleted
\----------------------------------------------------------------- */
int CIniFile::DelData(const QString &sKey)
{
   int iRV = 0;
   QVector<Ini::SIniEntry> tmpVec;

   for (int i = 0; i < IniEntries.size(); i++)
   {
      if (sKey != IniEntries[i].sKey)
      {
         tmpVec.push_back(IniEntries[i]);
         iRV ++;
      }
   }

   IniEntries.clear();
   IniEntries = tmpVec;

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: GetStringData
|  Begin: 18.01.2010 / 16:29:21
|  Author: Jo2003
|  Description: return string data from internal buffer
|
|  Parameters: key
|
|  Returns: value as string
\----------------------------------------------------------------- */
QString CIniFile::GetStringData(const QString &sKey)
{
   QString sRV = "";
   for (int i = 0; i < IniEntries.size(); i++)
   {
      if (sKey == IniEntries[i].sKey)
      {
         sRV = IniEntries[i].sData;
         break;
      }
   }

   return sRV;
}

/* -----------------------------------------------------------------\
|  Method: GetIntData
|  Begin: 18.01.2010 / 16:29:21
|  Author: Jo2003
|  Description: return int data from internal buffer
|
|  Parameters: key
|
|  Returns: value as long
\----------------------------------------------------------------- */
long CIniFile::GetIntData (const QString &sKey)
{
   long lRV = 0;
   for (int i = 0; i < IniEntries.size(); i++)
   {
      if (sKey == IniEntries[i].sKey)
      {
         lRV = IniEntries[i].sData.toLong();
         break;
      }
   }

   return lRV;
}

/* -----------------------------------------------------------------\
|  Method: GetFloatData
|  Begin: 18.01.2010 / 16:29:21
|  Author: Jo2003
|  Description: return float data from internal buffer
|
|  Parameters: key
|
|  Returns: value as double
\----------------------------------------------------------------- */
double CIniFile::GetFloatData (const QString &sKey)
{
   double dRV = 0;
   for (int i = 0; i < IniEntries.size(); i++)
   {
      if (sKey == IniEntries[i].sKey)
      {
         dRV = IniEntries[i].sData.toDouble();
         break;
      }
   }

   return dRV;
}

/* -----------------------------------------------------------------\
|  Method: SaveIni
|  Begin: 18.01.2010 / 16:30:44
|  Author: Jo2003
|  Description: save ini file
|
|  Parameters: --
|
|  Returns: 0 ==> ok
|          -1 ==> any error
\----------------------------------------------------------------- */
int CIniFile::SaveIni()
{
   int         iRV = 0;
   QDir        AppDir(sAppDir);
   QString     IniFileName = QString("%1/%2").arg(sAppDir).arg(sIniFile);
   QString     IniContent;
   QTextStream str(&IniContent);
   QFile       fIni(IniFileName);

   // create file content ...
   for (int i = 0; i < IniEntries.size(); i++)
   {
      str << IniEntries[i].sKey << " = " << IniEntries[i].sData << endl;
   }

   if (!AppDir.exists())
   {
      if(!AppDir.mkpath(sAppDir))
      {
         iRV = -1;
      }
   }

   if (!iRV)
   {
      if (fIni.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
      {
         if(fIni.write(IniContent.toUtf8()) != IniContent.length())
         {
            iRV = -1;
         }

         fIni.close();
      }
   }

   return iRV;
}

/************************* History ***************************\
| $Log$
\*************************************************************/

