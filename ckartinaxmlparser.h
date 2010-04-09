/*=============================================================================\
| $HeadURL$
|
| Author: Jo2003
|
| last changed by: $Author$
|
| Begin: Tuesday, January 05, 2010 07:54:59
|
| $Id$
|
\=============================================================================*/
#ifndef __201005075459_CKARTINAXMLPARSER_H
   #define __201005075459_CKARTINAXMLPARSER_H

#include <QXmlStreamReader>
#include <QVector>
#include <QMessageBox>
#include <QRegExp>
#include <QDateTime>
#include <QString>
#include <QMutex>

#include "clogfile.h"
#include "defdef.h"
#include "customization.h"
#include "chttptime.h"

//===================================================================
// namespace
//===================================================================
namespace cparser
{
   struct SChan
   {
      QString sName;
      QString sProgramm;
      uint    uiStart;
      uint    uiEnd;
      int     iId;
      int     iIdx;
   };

   struct SEpg
   {
      QString sName;
      QString sDescr;
      uint    uiGmt;
   };

   struct SArchInfo
   {
      QString sTitle;
      uint    uiStart;
      uint    uiEnd;
   };
}

/********************************************************************\
|  Class: CKartinaXMLParser
|  Date:  19.01.2010 / 15:25:59
|  Author: Jo2003
|  Description: parser for kartina.tv sent xml content
|
\********************************************************************/
class CKartinaXMLParser : public QObject
{
   Q_OBJECT

public:
   CKartinaXMLParser(const QByteArray &ba = QByteArray (), QObject * parent = 0);
   void SetByteArray (const QByteArray &ba);
   QVector<cparser::SChan> ParseChannelList (bool bFixTime);
   QVector<cparser::SEpg> ParseEpg (int &iChanID, uint &uiGmt, bool &bArchiv);
   QString ParseURL();
   QString ParseArchivURL(cparser::SArchInfo *pArchInf = NULL);
   int FixTime (uint &uiTime);
   int GetTimeShift () { return iTimeShift; }
   int GetFixTime () { return iOffset; }
   int GetSelectOptions (const QString &src, QVector<int> &lOpts, int &iActOpt);

protected:
   void CheckTimeOffSet (const QString &str);

private:
   QByteArray stream;
   QXmlStreamReader xml;
   int iOffset;
   int iTimeShift;
   QMutex mutex;
};

#endif /* __201005075459_CKARTINAXMLPARSER_H */
/*=============================================================================\
|                                    History:
| ---------------------------------------------------------------------------
| 05.Jan.2010 -
\=============================================================================*/

