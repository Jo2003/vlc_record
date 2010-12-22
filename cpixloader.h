/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 18.01.2010 / 16:16:25
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#ifndef __011810__PIXLOADER_H
   #define __011810__PIXLOADER_H

#include <QHttp>
#include <QHttpRequestHeader>
#include <QHttpResponseHeader>
#include <QBuffer>
#include <QDir>
#include <QStringList>
#include <QFileInfo>


/********************************************************************\
|  Class: CPixLoader
|  Date:  21.12.2010 / 12:00
|  Author: Jo2003
|  Description: helper class to download pictures
|
\********************************************************************/
class CPixLoader : public QHttp
{
   Q_OBJECT

public:
   CPixLoader();
   virtual ~CPixLoader();
   void setPictureList (const QStringList &list);
   void setHostAndFolder (const QString &host, const QString &folder);
   bool IsRunning () { return bRun; }

signals:
   void sigPixReady ();

protected:
   void startDownLoad ();

private:
   QBuffer     dataBuffer;
   QStringList lPicList;
   QStringList::const_iterator cit;
   QString     sLocalFolder;
   QString     sHost;
   bool bRun;
   bool bIsAnswer;
   int  iReq;

private slots:
   void slotCheckResp (int iReqID, bool err);
};

#endif // __011810__PIXLOADER_H
/************************* History ***************************\
| $Log$
\*************************************************************/

