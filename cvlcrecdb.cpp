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
#include "externals_inc.h"


/* -----------------------------------------------------------------\
|  Method: CVlcRecDB / constructor
|  Begin: 13.06.2010 / 16:17:51
|  Author: Jo2003
|  Description: init values ...
|
|  Parameters: pointer to parent object
|
|  Returns: --
\----------------------------------------------------------------- */
CVlcRecDB::CVlcRecDB(QObject *parent) : QObject(parent)
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

      // store current data base version ...
      setValue("db_version", REC_DB_VER);
   }

   if (!lAllTabs.contains("timerrec"))
   {
      iRV |= query.exec(TAB_TIMERREC) ? 0 : -1;
   }

   if (!lAllTabs.contains("shortcuts"))
   {
      iRV |= query.exec(TAB_SHORTCUTS) ? 0 : -1;
   }

   if (!lAllTabs.contains("watchlist"))
   {
      iRV |= query.exec(TAB_WATCHLIST) ? 0 : -1;
   }

   if (!lAllTabs.contains("astream"))
   {
      iRV |= query.exec(TAB_ASTREAM) ? 0 : -1;
   }

   if (!lAllTabs.contains("vodseen"))
   {
      iRV |= query.exec(TAB_VIDEO_SEEN) ? 0 : -1;
   }

   // db update ...
   iRV |= updateDB();

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: updateDB
|  Begin: 30.08.2012
|  Author: Jo2003
|  Description: check if db needs update, update if needed
|
|  Parameters: --
|
|  Returns: 0 --> ok
|          -1 --> error
\----------------------------------------------------------------- */
int CVlcRecDB::updateDB()
{
   int       iRV      = 0;
   int       iVer     = intValue("db_version");
   bool      bUpdVer  = (iVer != REC_DB_VER) ? true : false;

   QSqlQuery query;

   if (iVer > REC_DB_VER)
   {
      // Downgrade --> delete all settings!
      QStringList lAllTabs = db.tables();

      for (int i = 0; i < lAllTabs.count(); i++)
      {
         query.prepare("DELETE from ?");
         query.addBindValue(lAllTabs.at(i));
         iRV |= query.exec() ? 0 : -1;
      }
   }

   while (iVer++ < REC_DB_VER)
   {
      // Upgrade ...
      switch (iVer)
      {
      // version change 0 ... 1
      case 1:
         // clear aspect table since there may be
         // many unwanted values ...
         iRV |= query.exec("DELETE FROM aspect") ? 0 : -1;
         break;

      // Add any changes needed for version update here.
      // E.g. 'case 2:' for changes from 1 ... 2
      // ...

      // per default nothing to do ...
      default:
         break;
      }
   }

   if (bUpdVer)
   {
      iRV |= setValue("db_version", REC_DB_VER);
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
|  Returns: 0
\----------------------------------------------------------------- */
int CVlcRecDB::aspect(int iCid, QString &sAspect, QString &sCrop)
{
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
      // if not found return std as default.
      sAspect = "std";
      sCrop   = "std";
   }

   return 0;
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
|  Method: delAspect
|  Begin: 15.08.2012
|  Author: Jo2003
|  Description: delete one aspect entry
|
|  Parameters: channel id
|
|  Returns: 0 --> ok
|          -1 --> error
\----------------------------------------------------------------- */
int CVlcRecDB::delAspect(int iCid)
{
   int       iRV = 0;
   QSqlQuery query;
   query.prepare("DELETE FROM aspect WHERE cid=?");
   query.addBindValue(iCid);

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

/* -----------------------------------------------------------------\
|  Method: setBlob
|  Begin: 11.07.2012
|  Author: Jo2003
|  Description: store BLOB value as string in db
|
|  Parameters: ref. to key string, blob value to store
|
|  Returns: 0 --> ok
|          -1 --> any error
\----------------------------------------------------------------- */
int CVlcRecDB::setBlob(const QString &sKey, const QByteArray &blob)
{
   QStringList byteStream;
   uchar       ch;

   for (int i = 0; i < blob.count(); i++)
   {
      ch = (uchar)blob[i];
      byteStream << QString("%1").arg((uint)ch, 2, 16, QChar('0'));
   }

   return setValue(sKey, byteStream.join(":"));
}

/* -----------------------------------------------------------------\
|  Method: blobValue
|  Begin: 11.07.2012
|  Author: Jo2003
|  Description: get stored blob value
|
|  Parameters: ref. to key string, pointer to errornbuffer
|
|  Returns: value as byte array
\----------------------------------------------------------------- */
QByteArray CVlcRecDB::blobValue(const QString &sKey, int *pErr)
{
   int         err;
   QString     s    = stringValue(sKey, &err);
   QByteArray  blob;

   if (pErr)
   {
      *pErr = -1;
   }

   if ((s.size() > 0) && !err)
   {
      QStringList byteStream = s.split(":");

      for (int i = 0; i < byteStream.count(); i++)
      {
         blob += (char)byteStream.at(i).toUInt(NULL, 16);
      }

      if (pErr)
      {
         *pErr = 0;
      }
   }

   return blob;
}

//---------------------------------------------------------------------------
//
//! \brief   add a new watchentry to watchlist
//
//! \author  Jo2003
//! \date    06.08.2013
//
//! \param   entry (const cparser::SChan&) new entry to add
//
//! \return  0 -> ok; -1 -> query not successful
//---------------------------------------------------------------------------
int CVlcRecDB::addWatchEntry (const cparser::SChan& entry)
{
   QSqlQuery query;

   query.prepare("REPLACE INTO watchlist VALUES(?, ?, ?, ?, ?)");
   query.addBindValue(entry.iId);
   query.addBindValue(entry.uiStart);
   query.addBindValue(entry.uiEnd);
   query.addBindValue(entry.sName);
   query.addBindValue(entry.sProgramm);
   return query.exec() ? 0 : -1;
}

//---------------------------------------------------------------------------
//
//! \brief   get all entries from watchlist
//
//! \author  Jo2003
//! \date    06.08.2013
//
//! \param   vE (QVector<cparser::SChan>&) ref. to store vector
//
//! \return  0 -> ok; -1 -> query not successful
//---------------------------------------------------------------------------
int CVlcRecDB::getWatchEntries (QVector<cparser::SChan> &vE)
{
   QSqlQuery      query;
   cparser::SChan entry;
   int            iRet;

   vE.clear();

   query.prepare("SELECT cid, t_start, t_end, name, prog FROM watchlist ORDER BY t_start");

   iRet = query.exec() ? 0 : -1;

   if (!iRet)
   {
      if (query.first())
      {
         entry.iId       = query.value(0).toInt();
         entry.uiStart   = query.value(1).toUInt();
         entry.uiEnd     = query.value(2).toUInt();
         entry.sName     = query.value(3).toString();
         entry.sProgramm = query.value(4).toString();

         vE.append(entry);

         while (query.next())
         {
            entry.iId       = query.value(0).toInt();
            entry.uiStart   = query.value(1).toUInt();
            entry.uiEnd     = query.value(2).toUInt();
            entry.sName     = query.value(3).toString();
            entry.sProgramm = query.value(4).toString();

            vE.append(entry);
         }
      }
   }

   return iRet;
}

//---------------------------------------------------------------------------
//
//! \brief   delete specified entry from watchlist
//
//! \author  Jo2003
//! \date    06.08.2013
//
//! \param   cid (int) channel id
//! \param   uiGmt (uint) timestamp
//
//! \return  0 -> ok; -1 -> query not successful
//---------------------------------------------------------------------------
int CVlcRecDB::delWatchEntry (int cid, uint uiGmt)
{
   QSqlQuery query;

   query.prepare("DELETE FROM watchlist WHERE cid=? AND t_start=?");
   query.addBindValue(cid);
   query.addBindValue(uiGmt);
   return query.exec() ? 0 : -1;
}

//---------------------------------------------------------------------------
//
//! \brief   store default audio stream index for a channel
//
//! \author  Jo2003
//! \date    02.12.2013
//
//! \param   cid (int) channel id
//! \param   idx (int) audio stream index
//
//! \return  0 -> ok; -1 -> query not successful
//---------------------------------------------------------------------------
int CVlcRecDB::setDefAStream (int cid, int idx)
{
   QSqlQuery query;

   query.prepare("REPLACE INTO astream VALUES(?, ?)");
   query.addBindValue(cid);
   query.addBindValue(idx);
   return query.exec() ? 0 : -1;
}

//---------------------------------------------------------------------------
//
//! \brief   get default audio stream index for a channel
//
//! \author  Jo2003
//! \date    02.12.2013
//
//! \param   cid (int) channel id
//
//! \return  -1 -> no value stored; >-1 stream index
//---------------------------------------------------------------------------
int CVlcRecDB::defAStream (int cid)
{
   int       iRet = -1;
   QSqlQuery query;

   query.prepare("SELECT aidx FROM astream WHERE cid=?");
   query.addBindValue(cid);
   query.exec();

   if (query.first())
   {
      iRet = query.value(0).toInt();
   }

   return iRet;
}

//---------------------------------------------------------------------------
//
//! \brief   mark video as seen (ad related)
//
//! \author  Jo2003
//! \date    06.08.2014
//
//! \param   videoId [in] (int) video id
//
//! \return  -1 -> error; else OK
//---------------------------------------------------------------------------
int CVlcRecDB::markVod(int videoId)
{
   QSqlQuery query;

   query.prepare("REPLACE INTO vodseen VALUES(?, strftime('%s','now'))");
   query.addBindValue(videoId);
   return query.exec() ? 0 : -1;
}

//---------------------------------------------------------------------------
//
//! \brief   was video already seen (ad related)?
//
//! \author  Jo2003
//! \date    06.08.2014
//
//! \param   videoId [in] (int) video id
//
//! \return  true -> was seen; else -> wasn't seen
//---------------------------------------------------------------------------
bool CVlcRecDB::videoSeen(int videoId)
{
   bool bSeen = false;
   QSqlQuery query;

   // clean "old" entries ...
   cleanVodSeen();

   query.prepare("SELECT COUNT(*) as NUMB FROM vodseen WHERE videoid=?");
   query.addBindValue(videoId);
   query.exec();

   if (query.first())
   {
      bSeen = !!query.value(0).toInt();
   }

   // no post trigger ...
   if (!bSeen)
   {
      markVod(videoId);
   }

   return bSeen;
}

//---------------------------------------------------------------------------
//
//! \brief   delete old seen markers (re-enable ads for this video)
//
//! \author  Jo2003
//! \date    06.08.2014
//
//---------------------------------------------------------------------------
void CVlcRecDB::cleanVodSeen()
{
   QSqlQuery query;

   // timestamp one week ago ...
   uint ulTs = QDateTime::currentDateTime().toTime_t() - ADBLOCK_ACTIVE;

   // delete all entries older ADBLOCK_ACTIVE seconds ...
   query.prepare("DELETE FROM vodseen WHERE t_stamp < ?");
   query.addBindValue(ulTs);
   query.exec();
}

/************************* History ***************************\
| $Log$
\*************************************************************/
