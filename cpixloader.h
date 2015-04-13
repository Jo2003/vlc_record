/*********************** Information *************************\
| $HeadURL: https://vlc-record.googlecode.com/svn/branches/sunduk.tv/cpixloader.h $
|
| Author: Jo2003
|
| Begin: 18.01.2010 / 16:16:25
|
| Last edited by: $Author: Olenka.Joerg $
|
| $Id: cpixloader.h 1215 2013-11-11 09:56:46Z Olenka.Joerg $
\*************************************************************/
#ifndef __011810__PIXLOADER_H
   #define __011810__PIXLOADER_H

#include <QFileInfo>
#include <QVector>
#include <QMutex>

#include "csettingsdlg.h"

namespace PixCache {
   struct SPixDesc
   {
      QString sRemote;
      QString sLocal;
   };
}

typedef QVector<PixCache::SPixDesc> PixVector;

/********************************************************************\
|  Class: CPixLoader
|  Date:  21.12.2010 / 12:00
|  Author: Jo2003
|  Description: helper class to download pictures
|
\********************************************************************/
class CPixLoader : public QObject
{
   Q_OBJECT

public:
   explicit CPixLoader(QObject* parent = 0);
   virtual ~CPixLoader();
   void enqueuePic (const QString& sRemote, const QString &sLocal);
   bool busy();
   void importSettings(CSettingsDlg *pSetDlg);

protected:
   void startDownLoad ();

signals:
   void allDone();
   void sigLoadImage(const QString& url);

public slots:
   void slotImage(const QByteArray& ba);

private:
   CSettingsDlg *_pSettings;
   QMutex        mtxCacheQueue;
   PixVector     cacheQueue;
   bool          bRun;
};

#endif // __011810__PIXLOADER_H
/************************* History ***************************\
| $Log$
\*************************************************************/

