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
#include "ckartinaxmlparser.h"

// log file functions ...
extern CLogFile VlcLog;

/* -----------------------------------------------------------------\
|  Method: CKartinaXMLParser / constructor
|  Begin: 19.01.2010 / 15:27:01
|  Author: Jo2003
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
|  Author: Jo2003
|  Description: set data we have to parse
|
|  Parameters: data
|
|  Returns: --
\----------------------------------------------------------------- */
void CKartinaXMLParser::SetByteArray(const QByteArray &ba)
{
   mutex.lock();
   stream  = ba;
}

/* -----------------------------------------------------------------\
|  Method: ParseChannelList
|  Begin: 19.01.2010 / 15:28:20
|  Author: Jo2003
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
   QString                 sStart, sEnd;

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
            chan.sName     = attrs.value("title").toString();
            chan.iId       = attrs.value("id").toString().toInt();
            chan.iIdx      = attrs.value("idx").toString().toInt();
            chan.sProgramm = attrs.value("programm").toString();
            sStart         = attrs.value("sprog").toString();
            sEnd           = attrs.value("eprog").toString();

            chan.uiStart   = CHttpTime::fromEnString(sStart).toTime_t();
            chan.uiEnd     = CHttpTime::fromEnString(sEnd).toTime_t();

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

                     mInfo(tr("TimeShift is set to %1 hours.").arg(iTimeShift));
                  }
               }
            }

            // fix time offset ...
            if (bFixTime)
            {
               FixTime(chan.uiStart);
               FixTime(chan.uiEnd);
            }

            if ((chan.iId) && (chan.iIdx) && (chan.sName != ""))
            {
               chanlist.push_back(chan);
            }
         }
         else if(xml.name() == "channelgroup")
         {
            attrs          = xml.attributes();
            chan.sName     = attrs.value("title").toString();
            chan.iId       = -1;
            chan.iIdx      = -1;
            chan.sProgramm = attrs.value("color").toString();
            chan.uiStart   = 0;
            chan.uiEnd     = 0;

            chanlist.push_back(chan);
         }
         else if (xml.name() == "channels")
         {
            attrs          = xml.attributes();
            QString sTime  = attrs.value("clienttime").toString();

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

   mutex.unlock();

   return chanlist;
}

/* -----------------------------------------------------------------\
|  Method: ParseEpg
|  Begin: 19.01.2010 / 15:29:07
|  Author: Jo2003
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
            bArchiv       = (attrs.value("have_archive").toString().toInt() == 1) ? true : false;
            iChanID       =  attrs.value("channelid").toString().toInt();
            uiGmt         =  attrs.value("stime").toString().toUInt();
         }
         else if (xml.name() == "item")
         {
            attrs         =  xml.attributes();
            epg.sName     =  attrs.value("progname").toString();
            epg.sDescr    =  attrs.value("pdescr").toString();
            epg.uiGmt     =  attrs.value("t_start").toString().toUInt();

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

   mutex.unlock();

   return epglist;
}

/* -----------------------------------------------------------------\
|  Method: ParseURL
|  Begin: 19.01.2010 / 15:30:51
|  Author: Jo2003
|  Description: parse stream url (xml)
|
|  Parameters: --
|
|  Returns: stream url
\----------------------------------------------------------------- */
QString CKartinaXMLParser::ParseURL()
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
            url   = attrs.value("url").toString();
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
   }
   else
   {
      url        = "";
   }

   mutex.unlock();

   return url;
}

/* -----------------------------------------------------------------\
|  Method: ParseArchivURL
|  Begin: 19.01.2010 / 15:31:18
|  Author: Jo2003
|  Description: parse url for archiv (xml)
|
|  Parameters: optional pointer to archive info struct
|
|  Returns: url
\----------------------------------------------------------------- */
QString CKartinaXMLParser::ParseArchivURL(cparser::SArchInfo *pArchInf)
{
   QString              url;
   QXmlStreamAttributes attrs;
   QRegExp              rx("^([^ ]*).*$");

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
            url   = attrs.value("url").toString();

            if (pArchInf)
            {
               pArchInf->sTitle  = attrs.value("programm").toString();
               pArchInf->uiStart = attrs.value("start").toString().toUInt();
               pArchInf->uiEnd   = attrs.value("next").toString().toUInt();
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

   if (rx.indexIn(url) > -1)
   {
      url = rx.cap(1);
   }
   else
   {
      url = "";
   }

   mutex.unlock();

   return url;
}

/* -----------------------------------------------------------------\
|  Method: CheckTimeOffSet
|  Begin: 19.01.2010 / 15:32:03
|  Author: Jo2003
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

   mInfo(tr("%1 reports client time as %2").arg(COMPANY_NAME).arg(str));

   // get difference between kartina.tv and our time ...
   int iOffSec    = (int)(QDateTime::currentDateTime().toTime_t()
                          - CHttpTime::fromEnString(str).toTime_t());

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

   mInfo(tr("Set time offset to %1 seconds!").arg(iOffset));
}

/* -----------------------------------------------------------------\
|  Method: FixTime
|  Begin: 19.01.2010 / 15:32:54
|  Author: Jo2003
|  Description: fix time sent from kartina.tv as part from
|               channel list
|
|  Parameters: ref. to time stamp
|
|  Returns: 0 ==> not touched
|           1 ==> fixed
\----------------------------------------------------------------- */
int CKartinaXMLParser::FixTime (uint &uiTime)
{
   if (iOffset)
   {
      // add offset ...
      // note that offset can be negative ...
      uiTime += iOffset;

      return 1;
   }

   return 0;
}

/* -----------------------------------------------------------------\
|  Method: GetSelectOptions
|  Begin: 27.02.2010 / 18:42:54
|  Author: Jo2003
|  Description: parse srv form and get server list and active server
|
|  Parameters: html src, srv vector, act server
|
|  Returns: 0 ==> ok
|          -1 ==> any error
\----------------------------------------------------------------- */
int CKartinaXMLParser::GetSelectOptions(const QString &src,
                                        QVector<int> &lOpts,
                                        int &iActOpt)
{
   QRegExp rx("<option value=\"([0-9]+)\"(.*)>");
   int     iIdx = 0;
   int     iRV  = -1;

   while (iIdx > -1)
   {
      iIdx = src.indexOf(rx, iIdx);

      if (iIdx > -1)
      {
         lOpts.push_back(rx.cap(1).toInt());

         if (rx.cap(2).contains("selected"))
         {
            iActOpt = rx.cap(1).toInt();
            iRV     = 0;
         }

         iIdx ++;
      }
   }

   return iRV;
}

/*=============================================================================\
|                                    History:
| ---------------------------------------------------------------------------
| 05.Jan.2010 -
\=============================================================================*/

