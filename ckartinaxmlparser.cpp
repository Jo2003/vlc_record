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
#include "ckartinaxmlparser.h"

// log file functions ...
extern CLogFile VlcLog;

/* -----------------------------------------------------------------\
|  Method: CKartinaXMLParser / constructor
|  Begin: 19.01.2010 / 15:27:01
|  Author: Joerg Neubert
|  Description: construct object and init values
|
|  Parameters: data to parse, pointer to parent object
|
|  Returns: --
\----------------------------------------------------------------- */
CKartinaXMLParser::CKartinaXMLParser(const QByteArray &ba, QObject * parent) : QObject(parent)
{
   iOffset    = 0;
   iTimeShift = 0;
   stream     = ba;
}

/* -----------------------------------------------------------------\
|  Method: SetByteArray
|  Begin: 19.01.2010 / 15:27:54
|  Author: Joerg Neubert
|  Description: set data we have to parse
|
|  Parameters: data
|
|  Returns: --
\----------------------------------------------------------------- */
void CKartinaXMLParser::SetByteArray(const QByteArray &ba)
{
   stream  = ba;
}

/* -----------------------------------------------------------------\
|  Method: ParseChannelList
|  Begin: 19.01.2010 / 15:28:20
|  Author: Joerg Neubert
|  Description: parse channel list
|
|  Parameters: flag for time fixing
|
|  Returns: vector with channel elements
\----------------------------------------------------------------- */
QVector<cparser::SChan> CKartinaXMLParser::ParseChannelList(bool bFixTime)
{
   cparser::SChan          chan;
   QVector<cparser::SChan> chanlist;
   QXmlStreamAttributes    attrs;
   QRegExp                 rx("^.*-([0-9]+)$");
   bool                    bGotTimeShift = false;

   // reset timeshift ...
   iTimeShift = 0;

   xml.clear();
   xml.addData(stream);

   while(!xml.atEnd() && !xml.hasError())
   {
      switch (xml.readNext())
      {
      // we aren't interested in ...
      case QXmlStreamReader::StartDocument:
      case QXmlStreamReader::EndElement:
      case QXmlStreamReader::EndDocument:
         break;

      case QXmlStreamReader::StartElement:
         if (xml.name() == "channel")
         {
            attrs          = xml.attributes();
            chan.sName     = attrs.value(QString("title").toUtf8()).toString();
            chan.iId       = attrs.value(QString("id").toUtf8()).toString().toInt();
            chan.iIdx      = attrs.value(QString("idx").toUtf8()).toString().toInt();
            chan.sProgramm = attrs.value(QString("programm").toUtf8()).toString();
            chan.sStart    = attrs.value(QString("sprog").toUtf8()).toString();
            chan.sEnd      = attrs.value(QString("eprog").toUtf8()).toString();

            // try to get time shift from 1-j channel ...
            if (!bGotTimeShift)
            {
               if (chan.iId > 0) // any channel name ...
               {
                  // try to get timeshift value ...
                  if (rx.indexIn(chan.sName) > -1)
                  {
                     iTimeShift    = rx.cap(1).toInt();
                     bGotTimeShift = true;

                     VlcLog.LogInfo(tr("TimeShift is set to %1 hours.\n").arg(iTimeShift));
                  }
               }
            }

            // fix time offset ...
            if (bFixTime)
            {
               FixTime(chan.sStart);
               FixTime(chan.sEnd);
            }

            if ((chan.iId) && (chan.iIdx) && (chan.sName != ""))
            {
               chanlist.push_back(chan);
            }
         }
         else if(xml.name() == "channelgroup")
         {
            attrs          = xml.attributes();
            chan.sName     = attrs.value(QString("title").toUtf8()).toString();
            chan.iId       = -1;
            chan.iIdx      = -1;
            chan.sProgramm = attrs.value(QString("color").toUtf8()).toString();
            chan.sStart    = "";
            chan.sEnd      = "";

            chanlist.push_back(chan);
         }
         else if (xml.name() == "channels")
         {
            attrs          = xml.attributes();
            QString sTime  = attrs.value(QString("clienttime").toUtf8()).toString();

            if (sTime != "")
            {
               CheckTimeOffSet(sTime);
            }
         }
         break;

      default:
         break;
      }
   }

   xml.clear();

   /* Error handling. */
   if(xml.hasError())
   {
      QMessageBox::critical(NULL, tr("Error in %1").arg(__FUNCTION__),
                            tr("XML Error String: %1").arg(xml.errorString()));
   }

   return chanlist;
}

/* -----------------------------------------------------------------\
|  Method: ParseEpg
|  Begin: 19.01.2010 / 15:29:07
|  Author: Joerg Neubert
|  Description: parse epg content
|
|  Parameters: ref. to channel id (out), ref. to timestamp (out),
|              ref. to archiv flag (out)
|
|  Returns: vector with epg elements
\----------------------------------------------------------------- */
QVector<cparser::SEpg> CKartinaXMLParser::ParseEpg(int &iChanID, uint &uiGmt, bool &bArchiv)
{
   cparser::SEpg           epg;
   QVector<cparser::SEpg>  epglist;
   QXmlStreamAttributes    attrs;

   xml.clear();
   xml.addData(stream);

   while(!xml.atEnd() && !xml.hasError())
   {
      switch (xml.readNext())
      {
      // we aren't interested in ...
      case QXmlStreamReader::StartDocument:
      case QXmlStreamReader::EndElement:
      case QXmlStreamReader::EndDocument:
         break;

      case QXmlStreamReader::StartElement:
         if (xml.name() == "programm")
         {
            attrs         =  xml.attributes();
            bArchiv       = (attrs.value(QString("have_archive").toUtf8()).toString().toInt() == 1) ? true : false;
            iChanID       =  attrs.value(QString("channelid").toUtf8()).toString().toInt();
            uiGmt         =  attrs.value(QString("stime").toUtf8()).toString().toUInt();
         }
         else if (xml.name() == "item")
         {
            attrs         =  xml.attributes();
            epg.sName     =  attrs.value(QString("progname").toUtf8()).toString();
            epg.sDescr    =  attrs.value(QString("pdescr").toUtf8()).toString();
            epg.uiGmt     =  attrs.value(QString("t_start").toUtf8()).toString().toUInt();

            if (epg.uiGmt && (epg.sName != ""))
            {
               epglist.push_back(epg);
            }
         }
         break;

      default:
         break;
      }
   }

   xml.clear();

   /* Error handling. */
   if(xml.hasError())
   {
      QMessageBox::critical(NULL, tr("Error in %1").arg(__FUNCTION__),
                            tr("XML Error String: %1").arg(xml.errorString()));
   }

   return epglist;
}

/* -----------------------------------------------------------------\
|  Method: ParseURL
|  Begin: 19.01.2010 / 15:30:51
|  Author: Joerg Neubert
|  Description: parse stream url (xml)
|
|  Parameters: --
|
|  Returns: stream url
\----------------------------------------------------------------- */
QString CKartinaXMLParser::ParseURL(int &iCacheTime)
{
   QString              url;
   QXmlStreamAttributes attrs;
   QRegExp              rx("^.*//([^ ]*) :.*=([0-9]*) .*$");

   // :http-caching=15000 :no-http-reconnect
   // use whole url with params ...
//QRegExp              rx("^.*//(.*)$");

   xml.clear();
   xml.addData(stream);

   while(!xml.atEnd() && !xml.hasError())
   {
      switch (xml.readNext())
      {
      // we aren't interested in ...
      case QXmlStreamReader::StartDocument:
      case QXmlStreamReader::EndElement:
      case QXmlStreamReader::EndDocument:
         break;

      case QXmlStreamReader::StartElement:
         if (xml.name() == "url")
         {
            attrs = xml.attributes();
            url   = attrs.value(QString("url").toUtf8()).toString();
         }
         break;

      default:
         break;
      }
   }

   xml.clear();

   /* Error handling. */
   if(xml.hasError())
   {
      QMessageBox::critical(NULL, tr("Error in %1").arg(__FUNCTION__),
                            tr("XML Error String: %1").arg(xml.errorString()));
   }

   if (rx.indexIn(url) > -1)
   {
      url        = QString("http://%1").arg(rx.cap(1));
      iCacheTime = rx.cap(2).toInt();
   }
   else
   {
      url        = "";
      iCacheTime = 0;
   }

   return url;
}

/* -----------------------------------------------------------------\
|  Method: ParseArchivURL
|  Begin: 19.01.2010 / 15:31:18
|  Author: Joerg Neubert
|  Description: parse url for archiv (xml)
|
|  Parameters: --
|
|  Returns: url
\----------------------------------------------------------------- */
QString CKartinaXMLParser::ParseArchivURL()
{
   QString              url;
   QXmlStreamAttributes attrs;
   QRegExp              rx("^.*//([^ ]*).*$");

   xml.clear();
   xml.addData(stream);

   while(!xml.atEnd() && !xml.hasError())
   {
      switch (xml.readNext())
      {
      // we aren't interested in ...
      case QXmlStreamReader::StartDocument:
      case QXmlStreamReader::EndElement:
      case QXmlStreamReader::EndDocument:
         break;

      case QXmlStreamReader::StartElement:
         if (xml.name() == "url")
         {
            attrs = xml.attributes();
            url   = attrs.value(QString("url").toUtf8()).toString();
         }
         break;

      default:
         break;
      }
   }

   xml.clear();

   /* Error handling. */
   if(xml.hasError())
   {
      QMessageBox::critical(NULL, tr("Error in %1").arg(__FUNCTION__),
                            tr("XML Error String: %1").arg(xml.errorString()));
   }

   if (rx.indexIn(url) > -1)
   {
      url = QString("rtsp://%1").arg(rx.cap(1));
   }
   else
   {
      url = "";
   }

   return url;
}

/* -----------------------------------------------------------------\
|  Method: CheckTimeOffSet
|  Begin: 19.01.2010 / 15:32:03
|  Author: Joerg Neubert
|  Description: try to get offset between client and
|               kartina.tv server
|
|  Parameters: date time in string format
|
|  Returns: --
\----------------------------------------------------------------- */
void CKartinaXMLParser::CheckTimeOffSet (const QString &str)
{
   /*
     This function is a little tricky ...
     Try to find out the real difference between the
     time kartina.tv assumes for us and the real time
     running on this machine ...
     Round offset to full 30 minutes (min. timezone step)
     */

   VlcLog.LogInfo(tr("%1 / %2():%3 Kartina.tv reports client time as %4\n")
                  .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__).arg(str));


   // get difference between kartina.tv and our time ...
   int iOffSec    = (int)(QDateTime::currentDateTime().toTime_t()
                          - QDateTime::fromString(str, DEF_TIME_FORMAT).toTime_t());

   // round offset to full timezone step ...
   int iHalfHours = qRound ((double)iOffSec / (double)DEF_TZ_STEP);

   if (iHalfHours)
   {
      iOffset = iHalfHours * DEF_TZ_STEP;
   }
   else
   {
      iOffset = 0;
   }

   VlcLog.LogInfo(tr("%1 / %2():%3 Set time offset to %4 seconds!\n")
                  .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__).arg(iOffset));
}

/* -----------------------------------------------------------------\
|  Method: FixTime
|  Begin: 19.01.2010 / 15:32:54
|  Author: Joerg Neubert
|  Description: fix time sent from kartina.tv as part from
|               channel list
|
|  Parameters: ref. to time string (in, out)
|
|  Returns: 0 ==> not touched
|           1 ==> fixed
\----------------------------------------------------------------- */
int CKartinaXMLParser::FixTime (QString &sTime)
{
   if (iOffset)
   {
      // get time from string ...
      QDateTime ts = QDateTime::fromString(sTime, DEF_TIME_FORMAT);

      // add offset ...
      ts = ts.addSecs(iOffset);

      // create new time string ...
      sTime = ts.toString(DEF_TIME_FORMAT);
      return 1;
   }

   return 0;
}

/*=============================================================================\
|                                    History:
| ---------------------------------------------------------------------------
| 05.Jan.2010 -
\=============================================================================*/
