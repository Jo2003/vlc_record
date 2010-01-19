/*=============================================================================\
| $HeadURL$
|
| Author: Joerg Neubert
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

#include "clogfile.h"
#include "defdef.h"

//===================================================================
// namespace
//===================================================================
namespace cparser 
{
   struct SChan
   {
      QString sName;
      QString sProgramm;
      QString sStart;
      QString sEnd;
      int     iId;
      int     iIdx;
   };

   struct SEpg
   {
      QString sName;
      QString sDescr;
      uint    uiGmt;
   };
}

/********************************************************************\
|  Class: CKartinaXMLParser
|  Date:  19.01.2010 / 15:25:59
|  Author: Joerg Neubert
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
   QString ParseArchivURL();
   int FixTime (QString &sTime);
   int GetTimeShift () { return iTimeShift; }
   int GetFixTime () { return iOffset; }

protected:
   void CheckTimeOffSet (const QString &str);

private:
   QByteArray stream;
   QXmlStreamReader xml;
   int iOffset;
   int iTimeShift;
};

#endif /* __201005075459_CKARTINAXMLPARSER_H */
/*=============================================================================\
|                                    History:
| ---------------------------------------------------------------------------
| 05.Jan.2010 - 
\=============================================================================*/

