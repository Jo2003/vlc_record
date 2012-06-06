/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 13.06.2010 / 14:50:35
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#include "cvlcrecdb.h"
#include "tables.h"
#include "small_helpers.h"

// for folders ...
extern CDirStuff *pFolders;

/* -----------------------------------------------------------------\
|  Method: CVlcRecDB / constructor
|  Begin: 13.06.2010 / 16:17:51
|  Author: Jo2003
|  Description: init values ...
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
CVlcRecDB::CVlcRecDB()
{
   db = QSqlDatabase::addDatabase("QSQLITE");
   db.setDatabaseName(QString("%1/%2").arg(pFolders->getDataDir()).arg(VLC_REC_DB));
   if(!db.open())
   {
      QMessageBox::critical(NULL, tr("Error!"), tr("Can't create / open SQLite database ..."));
   }
   else
   {
      checkDb();
   }
}

/* -----------------------------------------------------------------\
|  Method: ~CVlcRecDB / destructor
|  Begin: 13.06.2010 / 16:17:51
|  Author: Jo2003
|  Description: clean on destruction ...
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
CVlcRecDB::~CVlcRecDB()
{
   if (db.isOpen())
   {
      QSqlQuery query("FLUSH TABLES");

      query.exec();

      db.close();
   }
}

/* -----------------------------------------------------------------\
|  Method: checkDb
|  Begin: 13.06.2010 / 16:17:51
|  Author: Jo2003
|  Description: check if db is ready for use
|
|  Parameters: --
|
|  Returns: 0 --> ok
|          -1 --> error
\----------------------------------------------------------------- */
int CVlcRecDB::checkDb()
{
   int         iRV      = 0;
   QStringList lAllTabs = db.tables();
   QSqlQuery   query;

   if (!lAllTabs.contains("aspect"))
   {
      iRV |= query.exec(TAB_ASPECT) ? 0 : -1;
   }

   if (!lAllTabs.contains("settings"))
   {
      iRV |= query.exec(TAB_SETTINGS) ? 0 : -1;
   }

   if (!lAllTabs.contains("timerrec"))
   {
      iRV |= query.exec(TAB_TIMERREC) ? 0 : -1;
   }

   if (!lAllTabs.contains("shortcuts"))
   {
      iRV |= query.exec(TAB_SHORTCUTS) ? 0 : -1;
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: ask
|  Begin: 14.06.2010 / 16:17:51
|  Author: Jo2003
|  Description: exec a query, return result
|
|  Parameters: ref. to question, ref. to query
|
|  Returns: 0 --> ok
|          -1 --> error
\----------------------------------------------------------------- */
int CVlcRecDB::ask(const QString &question, QSqlQuery &query)
{
   return query.exec(question) ? 0 : -1;
}

/* -----------------------------------------------------------------\
|  Method: ask
|  Begin: 13.04.2011 / 10:45
|  Author: Jo2003
|  Description: exec a query, return result
|
|  Parameters: ref. to query
|
|  Returns: 0 --> ok
|          -1 --> error
\----------------------------------------------------------------- */
int CVlcRecDB::ask(QSqlQuery &query)
{
   return query.exec() ? 0 : -1;
}

/* -----------------------------------------------------------------\
|  Method: deleteDb
|  Begin: 13.06.2010 / 16:17:51
|  Author: Jo2003
|  Description: delete database
|
|  Parameters: --
|
|  Returns: 0 --> ok
|          -1 --> error
\----------------------------------------------------------------- */
int CVlcRecDB::deleteDb()
{
   if (db.isOpen())
   {
      db.close();
   }

   return QFile::remove(QString("%1/%2").arg(pFolders->getDataDir()).arg(VLC_REC_DB)) ? 0 : -1;
}

/* -----------------------------------------------------------------\
|  Method: aspect
|  Begin: 13.06.2010 / 16:17:51
|  Author: Jo2003
|  Description: get stored aspect values to channel id
|
|  Parameters: channel id, ref. for aspect, ref. for crop
|
|  Returns: 0 --> ok
|          -1 --> error
\----------------------------------------------------------------- */
int CVlcRecDB::aspect(int iCid, QString &sAspect, QString &sCrop)
{
   int       iRV = 0;
   QSqlQuery query;
   query.prepare("SELECT asp, crop FROM aspect WHERE cid=?");
   query.addBindValue(iCid);
   query.exec();
   if(query.next())
   {
      sAspect = query.value(0).toString();
      sCrop   = query.value(1).toString();
   }
   else
   {
      iRV = -1;
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: addAspect
|  Begin: 13.06.2010 / 16:17:51
|  Author: Jo2003
|  Description: store aspect values to channel id
|
|  Parameters: channel id, aspect ratio, crop ratio
|
|  Returns: 0 --> ok
|          -1 --> error
\----------------------------------------------------------------- */
int CVlcRecDB::addAspect(int iCid, const QString &sAspect, const QString &sCrop)
{
   int       iRV = 0;
   QSqlQuery query;
   query.prepare("INSERT OR REPLACE INTO aspect (cid, asp, crop) VALUES (?, ?, ?)");
   query.addBindValue(iCid);
   query.addBindValue(sAspect);
   query.addBindValue(sCrop);

   if (!query.exec())
   {
      iRV = -1;
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: sqlError
|  Begin: 13.06.2010 / 16:17:51
|  Author: Jo2003
|  Description: get last sql error as string
|
|  Parameters: --
|
|  Returns: sql error string
\----------------------------------------------------------------- */
QString CVlcRecDB::sqlError()
{
   QSqlError err = db.lastError();
   return QString("Driver: %1\nDatabase: %2")
         .arg(err.driverText())
         .arg(err.databaseText());
}

/* -----------------------------------------------------------------\
|  Method: stringValue
|  Begin: 13.06.2010 / 16:17:51
|  Author: Jo2003
|  Description: get stored settings value
|
|  Parameters: ref. to key string, pointer to errornbuffer
|
|  Returns: value as string
\----------------------------------------------------------------- */
QString CVlcRecDB::stringValue(const QString &key, int *pErr)
{
   QString   rv;
   QSqlQuery query;

   query.prepare("SELECT val FROM settings WHERE name=?");
   query.addBindValue(key);
   query.exec();

   if (query.first())
   {
      if (pErr)
      {
         *pErr = 0;
      }
      rv = query.value(0).toString();
   }
   else
   {
      if (pErr)
      {
         *pErr = -1;
      }
   }

   return rv;
}

/* -----------------------------------------------------------------\
|  Method: intValue
|  Begin: 13.06.2010 / 16:17:51
|  Author: Jo2003
|  Description: get stored settings value
|
|  Parameters: ref. to key string, pointer to errornbuffer
|
|  Returns: value as int
\----------------------------------------------------------------- */
int CVlcRecDB::intValue(const QString &key, int *pErr)
{
   int       rv = 0;
   QSqlQuery query;

   query.prepare("SELECT val FROM settings WHERE name=?");
   query.addBindValue(key);
   query.exec();

   if (query.first())
   {
      if (pErr)
      {
         *pErr = 0;
      }
      rv = query.value(0).toInt();
   }
   else
   {
      if (pErr)
      {
         *pErr = -1;
      }
   }

   return rv;
}

/* -----------------------------------------------------------------\
|  Method: floatValue
|  Begin: 13.06.2010 / 16:17:51
|  Author: Jo2003
|  Description: get stored settings value
|
|  Parameters: ref. to key string, pointer to errornbuffer
|
|  Returns: value as float
\----------------------------------------------------------------- */
float CVlcRecDB::floatValue(const QString &key, int *pErr)
{
   float     rv = 0;
   QSqlQuery query;

   query.prepare("SELECT val FROM settings WHERE name=?");
   query.addBindValue(key);
   query.exec();

   if (query.first())
   {
      if (pErr)
      {
         *pErr = 0;
      }
      rv = query.value(0).toFloat();
   }
   else
   {
      if (pErr)
      {
         *pErr = -1;
      }
   }

   return rv;
}

/* -----------------------------------------------------------------\
|  Method: setShortCut
|  Begin: 11.02.2011 / 15:45
|  Author: Jo2003
|  Description: set shortcut
|
|  Parameters: target, slot and shortcut
|
|  Returns: 0 --> ok
|          -1 --> any error
\----------------------------------------------------------------- */
int CVlcRecDB::setShortCut(const QString &sTarget, const QString &sSlot, const QString &sShortCut)
{
   QSqlQuery query;
   query.prepare("INSERT OR REPLACE INTO shortcuts (target, slot, key_sequence) VALUES (?, ?, ?)");
   query.addBindValue(sTarget);
   query.addBindValue(sSlot);
   query.addBindValue(sShortCut);

   return query.exec() ? 0 : -1;
}

/* -----------------------------------------------------------------\
|  Method: getShortCut
|  Begin: 11.02.2011 / 15:45
|  Author: Jo2003
|  Description: get a shortcut
|
|  Parameters: target and slot
|
|  Returns: "" --> no shortcut stored
|         else --> shortcut as string
\----------------------------------------------------------------- */
QString CVlcRecDB::getShortCut(const QString &sTarget, const QString &sSlot)
{
   QString   rv;
   QSqlQuery query;

   query.prepare("SELECT key_sequence FROM shortcuts WHERE target=? AND slot=?");
   query.addBindValue(sTarget);
   query.addBindValue(sSlot);
   query.exec();

   if (query.first())
   {
      rv = query.value(0).toString();
   }

   return rv;
}

/* -----------------------------------------------------------------\
|  Method: setPassword
|  Begin: 04.06.2012
|  Author: Jo2003
|  Description: encrypt and save a password
|
|  Parameters: key to store password, plaintext password
|
|  Returns: 0 --> ok
|        else --> any error
\----------------------------------------------------------------- */
int CVlcRecDB::setPassword(const QString& key, const QString &pass)
{
   return setValue(key, CSmallHelpers::streamCipher(pass));
}

/* -----------------------------------------------------------------\
|  Method: password
|  Begin: 04.06.2012
|  Author: Jo2003
|  Description: get and decrypt a password
|
|  Parameters: key where the password is stored below
|
|  Returns: "" --> any error
|         else --> decrypted password
\----------------------------------------------------------------- */
QString CVlcRecDB::password(const QString &sKey)
{
   return CSmallHelpers::streamDecipher(stringValue(sKey));
}

/* -----------------------------------------------------------------\
|  Method: removeSetting
|  Begin: 04.06.2012
|  Author: Jo2003
|  Description: remove setting from settings table
|
|  Parameters: key for setting to remove
|
|  Returns: 0 --> ok
|          -1 --> any error
\----------------------------------------------------------------- */
int CVlcRecDB::removeSetting(const QString &sKey)
{
   QSqlQuery query;

   query.prepare("DELETE FROM settings WHERE name=?");
   query.addBindValue(sKey);
   return query.exec() ? 0 : -1;
}

/************************* History ***************************\
| $Log$
\*************************************************************/
