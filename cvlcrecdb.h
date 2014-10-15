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
#ifndef __120610_VLC_REC_DB_H
   #define __120610_VLC_REC_DB_H

#include <QObject>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QStringList>
#include <QSqlError>
#include <QSqlQuery>
#include <QFile>
#include <QVariant>

#include "cparser.h"

#define VLC_REC_DB "vlcrec.db.sqlite"
#define REC_DB_VER  1

/********************************************************************\
|  Class: CVlcRecDB
|  Date:  13.06.2010 / 16:00:28
|  Author: Jo2003
|  Description: class for database storage, inherits QProcess
|
\********************************************************************/
class CVlcRecDB : public QObject
{
   Q_OBJECT

public:
   CVlcRecDB(QObject *parent = 0);
   ~CVlcRecDB();
   int aspect (int iCid, QString &sAspect, QString &sCrop);
   int addAspect (int iCid, const QString &sAspect, const QString &sCrop);
   int delAspect (int iCid);
   int updateDB ();
   QString sqlError();

   /* -----------------------------------------------------------------\
   |  Method: setValue
   |  Begin: 13.06.2010 / 16:17:51
   |  Author: Jo2003
   |  Description: store setting in db
   |
   |  Parameters: ref. to key, ref. to value
   |
   |  Returns: 0 --> ok
   |          -1 --> error
   \----------------------------------------------------------------- */
   template <typename T> int setValue (const QString &sKey, const T& val)
   {
      QSqlQuery query;
      query.prepare("INSERT OR REPLACE INTO settings (name, val) VALUES (?, ?)");
      query.addBindValue(sKey);
      query.addBindValue(val);

      return query.exec() ? 0 : -1;
   }

   int        setBlob (const QString &sKey, const QByteArray &blob);
   QString    stringValue(const QString &sKey, int *pErr = NULL);
   int        intValue(const QString &sKey, int *pErr = NULL);
   float      floatValue(const QString &sKey, int *pErr = NULL);
   int        ask(const QString &question, QSqlQuery &query);
   int        ask(QSqlQuery &query);
   QString    getShortCut (const QString &sTarget, const QString &sSlot);
   int        setShortCut (const QString &sTarget, const QString &sSlot, const QString &sShortCut);
   int        setPassword(const QString& key, const QString &pass);
   QString    password(const QString &sKey);
   int        removeSetting(const QString &sKey);
   QByteArray blobValue(const QString &sKey, int *pErr = NULL);
   int        addWatchEntry (const cparser::SChan& entry);
   int        getWatchEntries (QVector<cparser::SChan> &vE);
   int        delWatchEntry (int cid, uint uiGmt);
   int        setDefAStream (int cid, int idx);
   int        defAStream (int cid);
   int        markVod (int videoId);
   bool       videoSeen (int videoId);
   void       cleanVodSeen ();

protected:
   int checkDb();
   int deleteDb();

private:
   QSqlDatabase db;
   QSqlQuery    qExp;
};

#endif // __120610_VLC_REC_DB
/************************* History ***************************\
| $Log$
\*************************************************************/

