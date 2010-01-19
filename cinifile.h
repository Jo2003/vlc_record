/*********************** Information *************************\
| $HeadURL: svn://172.25.1.40/joergn/trunk/vlc-record/cinifile.h $
| 
| Author: Joerg Neubert
|
| Begin: 18.01.2010 / 16:23:13
| 
| Last edited by: $Author: joergn $
| 
| $Id: cinifile.h 173 2010-01-18 15:43:19Z joergn $
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
|  Author: Joerg Neubert
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
   |  Author: Joerg Neubert
   |  Description: add data to stored values
   |
   |  Parameters: key, value
   |
   |  Returns: 0 ==> ok
   |          -1 ==> any error
   \----------------------------------------------------------------- */
   template <typename T>
         int AddData (const QString &sKey, const T& val)
   {
      int         iRV = 0;
      QTextStream str;

      // check if we should update an entry ...
      for (it = IniEntries.begin(); it != IniEntries.end(); it ++)
      {
         if (sKey == (*it).sKey)
         {
            // delete old data ...
            (*it).sData = "";

            // create text stream to store value ...
            str.setString(&(*it).sData);
            str << val;
            iRV = 1;
            break;
         }
      }

      if (!iRV)
      {
         Ini::SIniEntry entry;
         entry.sKey = sKey;
         str.setString(&entry.sData);
         str << val;

         IniEntries.push_back(entry);
      }

      return iRV;
   }

   /* -----------------------------------------------------------------\
   |  Method: UpdateData
   |  Begin: 18.01.2010 / 16:25:09
   |  Author: Joerg Neubert
   |  Description: update data stored in internal data struct
   |
   |  Parameters: key, value
   |
   |  Returns: 0 ==> ok
   |          -1 ==> any error
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


private:
   QVector<Ini::SIniEntry>::const_iterator cit;
   QVector<Ini::SIniEntry>::iterator it;
};

#endif /* __011810__CINIFILE_H */
/************************* History ***************************\
| $Log$
\*************************************************************/

