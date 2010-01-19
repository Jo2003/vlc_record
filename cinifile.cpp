/*********************** Information *************************\
| $HeadURL: svn://172.25.1.40/joergn/trunk/vlc-record/cinifile.cpp $
| 
| Author: Joerg Neubert
|
| Begin: 18.01.2010 / 16:25:48
| 
| Last edited by: $Author: joergn $
| 
| $Id: cinifile.cpp 175 2010-01-19 14:22:13Z joergn $
\*************************************************************/
#include "cinifile.h"

/* -----------------------------------------------------------------\
|  Method: CIniFile / constructor
|  Begin: 18.01.2010 / 16:26:12
|  Author: Joerg Neubert
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
|  Author: Joerg Neubert
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
|  Author: Joerg Neubert
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
|  Author: Joerg Neubert
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
|  Author: Joerg Neubert
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
      while (sLine.length() > 0);

      fIni.close();
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: DelData
|  Begin: 18.01.2010 / 16:28:37
|  Author: Joerg Neubert
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

   for (cit = IniEntries.begin(); cit != IniEntries.end(); cit ++)
   {
      if (sKey != (*cit).sKey)
      {
         tmpVec.push_back(*cit);
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
|  Author: Joerg Neubert
|  Description: return string data from internal buffer
|
|  Parameters: key
|
|  Returns: value as string
\----------------------------------------------------------------- */
QString CIniFile::GetStringData(const QString &sKey)
{
   QString sRV = "";
   for (cit = IniEntries.begin(); cit != IniEntries.end(); cit ++)
   {
      if (sKey == (*cit).sKey)
      {
         sRV = (*cit).sData;
         break;
      }
   }

   return sRV;
}

/* -----------------------------------------------------------------\
|  Method: GetIntData
|  Begin: 18.01.2010 / 16:29:21
|  Author: Joerg Neubert
|  Description: return int data from internal buffer
|
|  Parameters: key
|
|  Returns: value as long
\----------------------------------------------------------------- */
long CIniFile::GetIntData (const QString &sKey)
{
   long lRV = 0;
   for (cit = IniEntries.begin(); cit != IniEntries.end(); cit ++)
   {
      if (sKey == (*cit).sKey)
      {
         lRV = (*cit).sData.toLong();
         break;
      }
   }

   return lRV;
}

/* -----------------------------------------------------------------\
|  Method: GetFloatData
|  Begin: 18.01.2010 / 16:29:21
|  Author: Joerg Neubert
|  Description: return float data from internal buffer
|
|  Parameters: key
|
|  Returns: value as double
\----------------------------------------------------------------- */
double CIniFile::GetFloatData (const QString &sKey)
{
   double dRV = 0;
   for (cit = IniEntries.begin(); cit != IniEntries.end(); cit ++)
   {
      if (sKey == (*cit).sKey)
      {
         dRV = (*cit).sData.toDouble();
         break;
      }
   }

   return dRV;
}

/* -----------------------------------------------------------------\
|  Method: SaveIni
|  Begin: 18.01.2010 / 16:30:44
|  Author: Joerg Neubert
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
   for (cit = IniEntries.begin(); cit != IniEntries.end(); cit ++)
   {
      str << (*cit).sKey << " = " << (*cit).sData << endl;
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

