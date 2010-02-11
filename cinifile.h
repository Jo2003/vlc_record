/*********************** Information *************************\
| $HeadURL$
| 
| Author: Jo2003
|
| Begin: 18.01.2010 / 16:23:13
| 
| Last edited by: $Author$
| 
| $Id$
\*************************************************************/
#ifndef __011810__CINIFILE_H
   #define __011810__CINIFILE_H

#include <QFile>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QTextStream>
#include <QRegExp>
#include <QDir>

namespace Ini
{
   class CIniFile;

   struct SIniEntry
   {
      QString   sKey;
      QString   sData;
   };
}

/********************************************************************\
|  Class: CIniFile
|  Date:  18.01.2010 / 16:23:41
|  Author: Jo2003
|  Description: class to handle ini file access
|
\********************************************************************/
class CIniFile
{
public:
   CIniFile();
   CIniFile(const char *sDir, const char *sFile);
   ~CIniFile ();

   void SetFileName (const char *sDir, const char *sFile);

   int ReadIni ();
   int SaveIni ();

   QString GetStringData (const QString &sKey);
   long GetIntData (const QString &sKey);
   double GetFloatData (const QString &sKey);

   /* -----------------------------------------------------------------\
   |  Method: AddData
   |  Begin: 18.01.2010 / 16:24:35
   |  Author: Jo2003
   |  Description: add data to stored values
   |
   |  Parameters: key, value
   |
   |  Returns: 0 ==> added
   |           1 ==> updated
   \----------------------------------------------------------------- */
   template <typename T>
         int AddData (const QString &sKey, const T& val)
   {
      int iRV = 0;

      // check if we should update an entry ...
      for (int i = 0; i < IniEntries.size(); i++)
      {
         if (sKey == IniEntries[i].sKey)
         {
            // update data ...
            IniEntries[i].sData = QString("%1").arg(val);
            iRV = 1;
            break;
         }
      }

      if (!iRV)
      {
         // not updated --> add new entry ...
         Ini::SIniEntry entry;
         entry.sKey  = sKey;
         entry.sData = QString("%1").arg(val);
         IniEntries.push_back(entry);
      }

      return iRV;
   }

   /* -----------------------------------------------------------------\
   |  Method: UpdateData
   |  Begin: 18.01.2010 / 16:25:09
   |  Author: Jo2003
   |  Description: update data stored in internal data struct
   |
   |  Parameters: key, value
   |
   |  Returns: 0 ==> added
   |           1 ==> updated
   \----------------------------------------------------------------- */
   template <typename T>
         int UpdateData (const QString &sKey, const T& val)
   {
      return AddData (sKey, val);
   }

   int DelData (const QString &sKey);

protected:
   QVector<Ini::SIniEntry> IniEntries;
   QString sIniFile;
   QString sAppDir;
};

#endif /* __011810__CINIFILE_H */
/************************* History ***************************\
| $Log$
\*************************************************************/

