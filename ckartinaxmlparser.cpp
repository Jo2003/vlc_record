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
CKartinaXMLParser::CKartinaXMLParser(QObject * parent) : QObject(parent)
{
   iOffset    = 0;
}

/* -----------------------------------------------------------------\
|  Method: checkTimeOffSet
|  Begin: 19.01.2010 / 15:32:03
|  Author: Jo2003
|  Description: try to get offset between client and
|               kartina.tv server
|
|  Parameters: servertime as unix timestamp
|
|  Returns: --
\----------------------------------------------------------------- */
void CKartinaXMLParser::checkTimeOffSet (const uint &uiSrvTime)
{
   /*
     This function is a little tricky ...
     Try to find out the real difference between the
     time kartina.tv assumes for us and the real time
     running on this machine ...
     Round offset to full 30 minutes (min. timezone step)
     */

   // get difference between kartina.tv and our time ...
   int iOffSec    = (int)(QDateTime::currentDateTime().toTime_t() - uiSrvTime);

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

   if (iOffset)
   {
      mInfo(tr("Set time offset to %1 seconds!").arg(iOffset));
   }
}

/* -----------------------------------------------------------------\
|  Method: fixTime
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
int CKartinaXMLParser::fixTime (uint &uiTime)
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
|  Method: initChanEntry
|  Begin: 13.01.2011 / 16:28:20
|  Author: Jo2003
|  Description: init chanEntry struct
|
|  Parameters: ref. to entry to init, isChan flag
|
|  Returns: --
\----------------------------------------------------------------- */
void CKartinaXMLParser::initChanEntry(cparser::SChan &entry, bool bIsChan)
{
   // new item starts --> init chan struct ...
   entry.bHasArchive  = false;
   entry.bIsGroup     = (bIsChan) ? false : true;
   entry.bIsProtected = false;
   entry.bIsVideo     = true;
   entry.iId          = -1;
   entry.sIcon        = "";
   entry.sName        = "";
   entry.sProgramm    = "";
   entry.uiEnd        = 0;
   entry.uiStart      = 0;
   entry.bIsHidden    = false;
   entry.vTs.clear();
}

/* -----------------------------------------------------------------\
|  Method: parseChannelList
|  Begin: 29.07.2010 / 11:28:20 (rewrite 13.01.2011)
|  Author: Jo2003
|  Description: parse channel list
|
|  Parameters: ref. to response, ref. to chanList, fixTime flag
|
|  Returns: 0 --> ok
|        else --> any error
\----------------------------------------------------------------- */
int CKartinaXMLParser::parseChannelList (const QString &sResp,
                                         QVector<cparser::SChan> &chanList,
                                         bool bFixTime)
{
   int iRV = 0;

   // lock parser ...
   mutex.lock();

   // clear channel list ...
   chanList.clear();

   xmlSr.clear();
   xmlSr.addData(sResp);

   while(!xmlSr.atEnd() && !xmlSr.hasError())
   {
      switch (xmlSr.readNext())
      {
      // any xml element starts ...
      case QXmlStreamReader::StartElement:
         if (xmlSr.name() == "groups")
         {
            // go into next level and parse groups ...
            parseGroups(xmlSr, chanList, bFixTime);
         }
         break;

      default:
         break;

      } // end switch ...

   } // end while ...

   // check for xml errors ...
   if(xmlSr.hasError())
   {
      QMessageBox::critical(NULL, tr("Error in %1").arg(__FUNCTION__),
                            tr("XML Error String: %1").arg(xmlSr.errorString()));

      iRV = -1;
   }

   // unlock parser ...
   mutex.unlock();

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: parseGroups
|  Begin: 14.01.2011 / 16:50
|  Author: Jo2003
|  Description: parse group part of channel list
|
|  Parameters: ref. to xml parser, ref. to chanList, fixTime flag
|
|  Returns: 0
\----------------------------------------------------------------- */
int CKartinaXMLParser::parseGroups (QXmlStreamReader &xml, QVector<cparser::SChan> &chanList,
                                    bool bFixTime)
{
   QString        sUnknown;
   cparser::SChan groupEntry;

   // while not end groups ...
   while (!((xml.readNext() == QXmlStreamReader::EndElement)
      && (xml.name() == "groups")))
   {
      if (xml.tokenType() == QXmlStreamReader::StartElement)
      {
         // item start --> initialize struct ...
         if (xml.name() == "item")
         {
            initChanEntry(groupEntry, false);
         }
         else if (xml.name() == "name")
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               groupEntry.sName = xml.text().toString();
            }
         }
         else if (xml.name() == "id")
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               groupEntry.iId = xml.text().toString().toInt();
            }
         }
         else if (xml.name() == "color")
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               groupEntry.sProgramm = xml.text().toString();
            }
         }
         else if (xml.name() == "channels")
         {
            // store group entry ...
            chanList.push_back(groupEntry);

            // go into next level (channels)
            parseChannels(xml, chanList, bFixTime);
         }
         else
         {
            // any unknown element shouldn't break our parser ...
            sUnknown = xml.name().toString();

#ifndef QT_NO_DEBUG
            mInfo(tr("Found unused element %1 ...").arg(sUnknown));
#endif
            while (!((xml.readNext() == QXmlStreamReader::EndElement)
               && (xml.name().toString() == sUnknown)))
            {
#ifndef QT_NO_DEBUG
               mInfo(tr("Found unused child %1: %2 ...")
                    .arg(xml.name().toString()).arg(xml.text().toString()));
#endif
            }
         }
      }
   }

   return 0;
}

/* -----------------------------------------------------------------\
|  Method: parseChannels
|  Begin: 14.01.2011 / 16:50
|  Author: Jo2003
|  Description: parse channels part of channel list
|
|  Parameters: ref. to xml parser, ref. to chanList, fixTime flag
|
|  Returns: 0
\----------------------------------------------------------------- */
int CKartinaXMLParser::parseChannels(QXmlStreamReader &xml, QVector<cparser::SChan> &chanList,
                                     bool bFixTime)
{
   QString        sUnknown;
   cparser::SChan chanEntry;

   // while no end of channels ...
   while (!((xml.readNext() == QXmlStreamReader::EndElement)
      && (xml.name() == "channels")))
   {
      if (xml.tokenType() == QXmlStreamReader::StartElement)
      {
         // item start -> init struct ...
         if (xml.name() == "item")
         {
            initChanEntry(chanEntry);
         }
         else if (xml.name() == "name")
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               chanEntry.sName = xml.text().toString();
            }
         }
         else if (xml.name() == "id")
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               chanEntry.iId = xml.text().toString().toInt();
            }
         }
         else if (xml.name() == "is_video")
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               chanEntry.bIsVideo = (xml.text().toString().toInt() == 1) ? true : false;
            }
         }
         else if (xml.name() == "have_archive")
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               chanEntry.bHasArchive = (xml.text().toString().toInt() == 1) ? true : false;
            }
         }
         else if (xml.name() == "protected")
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               chanEntry.bIsProtected = (xml.text().toString().toInt() == 1) ? true : false;
            }
         }
         else if (xml.name() == "icon")
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               chanEntry.sIcon = xml.text().toString();
            }
         }
         else if (xml.name() == "epg_progname")
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               chanEntry.sProgramm = xml.text().toString();
            }
         }
         else if (xml.name() == "epg_start")
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               chanEntry.uiStart = xml.text().toString().toUInt();

               if (bFixTime)
               {
                  fixTime(chanEntry.uiStart);
               }
            }
         }
         else if (xml.name() == "epg_end")
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               chanEntry.uiEnd = xml.text().toString().toUInt();

               if (bFixTime)
               {
                  fixTime(chanEntry.uiEnd);
               }
            }
         }
         else if (xml.name() == "hide")
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               chanEntry.bIsHidden = (xml.text().toString().toInt() == 1) ? true : false;
            }
         }
         else if (xml.name() == "stream_params")
         {
            // go into next level ... parse stream params ...
            parseStreamParams(xml, chanEntry.vTs);
         }
         else
         {
            // any unknown element shouldn't break our parser ...
            sUnknown = xml.name().toString();

#ifndef QT_NO_DEBUG
            mInfo(tr("Found unused element %1 ...").arg(sUnknown));
#endif
            while (!((xml.readNext() == QXmlStreamReader::EndElement)
               && (xml.name().toString() == sUnknown)))
            {
#ifndef QT_NO_DEBUG
               mInfo(tr("Found unused child %1: %2 ...")
                    .arg(xml.name().toString()).arg(xml.text().toString()));
#endif
            }
         }
      }
      else if (xml.tokenType() == QXmlStreamReader::EndElement)
      {
         // item end -> save entry ...
         if (xml.name() == "item")
         {
            chanList.push_back(chanEntry);
         }
      }
   }

   return 0;
}

/* -----------------------------------------------------------------\
|  Method: parseStreamParams
|  Begin: 14.01.2011 / 16:50
|  Author: Jo2003
|  Description: parse stream params part of channel list
|
|  Parameters: ref. to xml parser, ref. to stream params vector
|
|  Returns: 0
\----------------------------------------------------------------- */
int CKartinaXMLParser::parseStreamParams (QXmlStreamReader &xml, QVector<cparser::STimeShift>& vTs)
{
   QString             sUnknown;
   cparser::STimeShift sTs;
   bool                bEndSub;

   sTs.iBitRate   = 0;
   sTs.iTimeShift = 0;

   while (!((xml.readNext() == QXmlStreamReader::EndElement)
      && (xml.name() == "stream_params")))
   {
      if (xml.tokenType() == QXmlStreamReader::StartElement)
      {
         // item start -> init struct ...
         if (xml.name() == "item")
         {
            sTs.iBitRate   = 0;
            sTs.iTimeShift = 0;
         }
         else if (xml.name() == "rate")
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               sTs.iBitRate = xml.text().toString().toInt();
            }
         }
         else if (xml.name() == "ts")
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               sTs.iTimeShift = xml.text().toString().toInt();
            }
         }
         else
         {
            // starttag unknown element ...
            bEndSub  = false;
            sUnknown = xml.name().toString();

#ifndef QT_NO_DEBUG
            mInfo(tr("Found unused element %1 ...").arg(sUnknown));
#endif

            // search for endtag of unknown element ...
            while(!xml.atEnd() && !xml.hasError() && !bEndSub)
            {
               if ((xml.readNext() == QXmlStreamReader::EndElement)
                  && (xml.name().toString() == sUnknown))
               {
                  // found end tag of unknown element ...
                  bEndSub = true;
               }
            }
         }
      }
      else if (xml.tokenType() == QXmlStreamReader::EndElement)
      {
         // item end -> save entry ...
         if (xml.name() == "item")
         {
            vTs.push_back(sTs);
         }
      }
   }

   return 0;
}

/* -----------------------------------------------------------------\
|  Method: parseSServers
|  Begin: 29.07.2010 / 15:52:54
|  Author: Jo2003
|  Description: parse stream server resp.
|
|  Parameters: XML in, ref. to srv vector, ref. to act value
|
|  Returns: 0 ==> ok
|        else ==> any error
\----------------------------------------------------------------- */
int CKartinaXMLParser::parseSServers(const QString &sResp, QVector<cparser::SSrv> &vSrv,
                                     QString &sActIp)
{
   cparser::SSrv srv;
   int           iRV = 0;

   // clear epg list ...
   vSrv.clear();

   // lock parser ...
   mutex.lock();

   xmlSr.clear();
   xmlSr.addData(sResp);

   while(!xmlSr.atEnd() && !xmlSr.hasError())
   {
      switch (xmlSr.readNext())
      {
      // we aren't interested in ...
      case QXmlStreamReader::StartDocument:
      case QXmlStreamReader::EndDocument:
         break;

      // any xml element ends ...
      case QXmlStreamReader::EndElement:
         if (xmlSr.name() == "item")
         {
            vSrv.push_back(srv);
         }
         break;

      // any xml element starts ...
      case QXmlStreamReader::StartElement:
         if (xmlSr.name() == "item")
         {
            srv.sName = "";
            srv.sIp   = "";
         }
         else if (xmlSr.name() == "ip")
         {
            // read srv ip address ...
            if (xmlSr.readNext() == QXmlStreamReader::Characters)
            {
               srv.sIp = xmlSr.text().toString();
            }
         }
         else if (xmlSr.name() == "descr")
         {
            // read srv name ...
            if (xmlSr.readNext() == QXmlStreamReader::Characters)
            {
               srv.sName = xmlSr.text().toString();
            }
         }
         else if (xmlSr.name() == "value")
         {
            // read actual srv ip ...
            if (xmlSr.readNext() == QXmlStreamReader::Characters)
            {
               sActIp = xmlSr.text().toString();
            }
         }
         break;

      default:
         break;

      } // end switch ...

   } // end while ...

   // check for xml errors ...
   if(xmlSr.hasError())
   {
      QMessageBox::critical(NULL, tr("Error in %1").arg(__FUNCTION__),
                            tr("XML Error String: %1").arg(xmlSr.errorString()));

      iRV = -1;
   }

   // unlock parser ...
   mutex.unlock();

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: parseSServersLogin
|  Begin: 11.05.2012
|  Author: Jo2003
|  Description: parse stream server from login response
|
|  Parameters: XML in, ref. to srv vector, ref. to act value
|
|  Returns: 0 ==> ok
|        else ==> any error
\----------------------------------------------------------------- */
int CKartinaXMLParser::parseSServersLogin(const QString &sResp, QVector<cparser::SSrv> &vSrv,
                                     QString &sActIp)
{
   cparser::SSrv srv;
   int           iRV      = 0;
   bool          bStarted = false;
   bool          bAtEnd   = false;

   // clear epg list ...
   vSrv.clear();

   // lock parser ...
   mutex.lock();

   xmlSr.clear();
   xmlSr.addData(sResp);

   while(!xmlSr.atEnd() && !xmlSr.hasError() && !bAtEnd)
   {
      switch (xmlSr.readNext())
      {
      // we aren't interested in ...
      case QXmlStreamReader::StartDocument:
      case QXmlStreamReader::EndDocument:
         break;

      // any xml element ends ...
      case QXmlStreamReader::EndElement:
         if (xmlSr.name() == "stream_server")
         {
            bStarted = false;
            bAtEnd   = true;
         }

         if ((xmlSr.name() == "item") && bStarted)
         {
            vSrv.push_back(srv);
         }
         break;

      // any xml element starts ...
      case QXmlStreamReader::StartElement:
         if (xmlSr.name() == "stream_server")
         {
            bStarted = true;
         }

         if (bStarted)
         {
            if (xmlSr.name() == "item")
            {
               srv.sName = "";
               srv.sIp   = "";
            }
            else if (xmlSr.name() == "ip")
            {
               // read srv ip address ...
               if (xmlSr.readNext() == QXmlStreamReader::Characters)
               {
                  srv.sIp = xmlSr.text().toString();
               }
            }
            else if (xmlSr.name() == "descr")
            {
               // read srv name ...
               if (xmlSr.readNext() == QXmlStreamReader::Characters)
               {
                  srv.sName = xmlSr.text().toString();
               }
            }
            else if (xmlSr.name() == "value")
            {
               // read actual srv ip ...
               if (xmlSr.readNext() == QXmlStreamReader::Characters)
               {
                  sActIp = xmlSr.text().toString();
               }
            }
         }
         break;

      default:
         break;

      } // end switch ...

   } // end while ...

   // check for xml errors ...
   if(xmlSr.hasError())
   {
      QMessageBox::critical(NULL, tr("Error in %1").arg(__FUNCTION__),
                            tr("XML Error String: %1").arg(xmlSr.errorString()));

      iRV = -1;
   }

   // unlock parser ...
   mutex.unlock();

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: parseCookie
|  Begin: 28.07.2010 / 18:42:54
|  Author: Jo2003
|  Description: parse cookie
|
|  Parameters: XML in, buffer for cookie, buffer for expire string
|
|  Returns: 0 ==> ok
|        else ==> any error
\----------------------------------------------------------------- */
int CKartinaXMLParser::parseCookie (const QString &sResp, QString &sCookie, cparser::SAccountInfo &sInf)
{
   int                    iRV = 0;
   QString                sSid, sSidName;
   QStringList            slNeeded;
   QMap<QString, QString> mResults;
   sCookie = "";

   // lock parser ...
   mutex.lock();

   xmlSr.clear();
   xmlSr.addData(sResp);

   while(!xmlSr.atEnd() && !xmlSr.hasError())
   {
      switch (xmlSr.readNext())
      {
      // any xml element starts ...
      case QXmlStreamReader::StartElement:
         if (xmlSr.name() == "sid")
         {
            // read sid ...
            if (xmlSr.readNext() == QXmlStreamReader::Characters)
            {
               sSid = xmlSr.text().toString();
            }
         }
         else if (xmlSr.name() == "sid_name")
         {
            // read sid_name ...
            if (xmlSr.readNext() == QXmlStreamReader::Characters)
            {
               sSidName = xmlSr.text().toString();
            }
         }
         else if (xmlSr.name() == "account")
         {
            mResults.clear();
            slNeeded.clear();
            slNeeded << "login" << "packet_name" << "packet_expire";

            // get expires value ...
            oneLevelParser("account", slNeeded, mResults);

            // format into string ...
            sInf.sExpires = QDateTime::fromTime_t(mResults.value("packet_expire").toUInt())
                  .toString(DEF_TIME_FORMAT);
         }
         else if (xmlSr.name() == "services")
         {
            mResults.clear();
            slNeeded.clear();
            slNeeded << "vod" << "archive";

            // get values ...
            oneLevelParser("services", slNeeded, mResults);

            sInf.bHasVOD     = mResults.value("vod").toInt() ? true : false;
            sInf.bHasArchive = mResults.value("archive").toInt() ? true : false;
         }
         else if (xmlSr.name() == "servertime")
         {
            // read server time ...
            if (xmlSr.readNext() == QXmlStreamReader::Characters)
            {
               // check offset ...
               checkTimeOffSet (xmlSr.text().toString().toUInt());
            }
         }
         break;

      default:
         break;

      } // end switch ...

   } // end while ...

   if ((sSid != "") && (sSidName != ""))
   {
      sCookie = QString("%1=%2").arg(sSidName).arg(sSid);
   }
   else
   {
      iRV = -1;
   }

   // check for xml errors ...
   if(xmlSr.hasError())
   {
      QMessageBox::critical(NULL, tr("Error in %1").arg(__FUNCTION__),
                            tr("XML Error String: %1").arg(xmlSr.errorString()));

      iRV = -1;
   }

   // unlock parser ...
   mutex.unlock();

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: parseGenres
|  Begin: 09.12.2010 / 13:27
|  Author: Jo2003
|  Description: parse genre XML
|
|  Parameters: ref. to response, ref. to genres vector
|
|  Returns: 0 --> ok
|        else --> any error
\----------------------------------------------------------------- */
int CKartinaXMLParser::parseGenres (const QString& sResp, QVector<cparser::SGenre>& vGenres)
{
   int             iRV = 0;
   cparser::SGenre sGenre;
   vGenres.clear ();

   // lock parser ...
   mutex.lock();

   xmlSr.clear();
   xmlSr.addData(sResp);

   while(!xmlSr.atEnd() && !xmlSr.hasError())
   {
      switch (xmlSr.readNext())
      {
      // any xml element starts ...
      case QXmlStreamReader::StartElement:
         if (xmlSr.name() == "id")
         {
            // read id ...
            if (xmlSr.readNext() == QXmlStreamReader::Characters)
            {
               sGenre.uiGid = xmlSr.text().toString().toUInt();
            }
         }
         else if (xmlSr.name() == "name")
         {
            // read name ...
            if (xmlSr.readNext() == QXmlStreamReader::Characters)
            {
               sGenre.sGName = xmlSr.text().toString();

               // add genre to vector ...
               vGenres.push_back(sGenre);
            }
         }
         break;

      default:
         break;

      } // end switch ...

   } // end while ...

   // check for xml errors ...
   if(xmlSr.hasError())
   {
      QMessageBox::critical(NULL, tr("Error in %1").arg(__FUNCTION__),
                            tr("XML Error String: %1").arg(xmlSr.errorString()));

      iRV = -1;
   }

   // unlock parser ...
   mutex.unlock();

   return iRV;
}

//---------------------------------------------------------------------------
//
//! \brief   parse current epg anser
//
//! \author  Jo2003
//! \date    30.05.2012
//
//! \param   parent pointer to parent widget
//
//! \return  --
//---------------------------------------------------------------------------
int CKartinaXMLParser::parseEpgCurrent (const QString& sResp, QCurrentMap &currentEpg)
{
   int                           iRV =  0;
   int                           cid = -1;
   QStringList                   slNeeded;
   QMap<QString, QString>        mResults;
   cparser::SEpgCurrent          entry;
   QVector<cparser::SEpgCurrent> chanEntries;

   currentEpg.clear();

   slNeeded << "progname" << "ts";

   // lock parser ...
   mutex.lock();

   xmlSr.clear();
   xmlSr.addData(sResp);

   while(!xmlSr.atEnd() && !xmlSr.hasError())
   {
      switch (xmlSr.readNext())
      {
      // any xml element starts ...
      case QXmlStreamReader::StartElement:
         if (xmlSr.name() == "cid")
         {
            // read cid ...
            if (xmlSr.readNext() == QXmlStreamReader::Characters)
            {
               cid = xmlSr.text().toString().toInt();
            }
         }
         else if((xmlSr.name() == "item") && (cid != -1))
         {
            // read epg entry ...
            oneLevelParser("item", slNeeded, mResults);

            entry.sShow   = mResults.value("progname");
            entry.uiStart = mResults.value("ts").toUInt();

            chanEntries.append(entry);
         }
         break;

      case QXmlStreamReader::EndElement:
         if ((xmlSr.name() == "epg") && (cid != -1))
         {
            if (!chanEntries.isEmpty())
            {
               currentEpg.insert(cid, chanEntries);
            }

            cid = -1;
            chanEntries.clear();
         }
         break;
      default:
         break;
      }
   }

   // check for xml errors ...
   if(xmlSr.hasError())
   {
      QMessageBox::critical(NULL, tr("Error in %1").arg(__FUNCTION__),
                            tr("XML Error String: %1").arg(xmlSr.errorString()));

      iRV = -1;
   }

   // unlock parser ...
   mutex.unlock();

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: parseVodManager
|  Begin: 23.05.2012
|  Author: Jo2003
|  Description: parse Vod Manager data (genre rating)
|
|  Parameters: ref. to response, ref. to rates vector
|
|  Returns: 0 --> ok
|        else --> any error
\----------------------------------------------------------------- */
int CKartinaXMLParser::parseVodManager (const QString &sResp, QVector<cparser::SVodRate> &vRates)
{
   int               iRV = 0;
   cparser::SVodRate sRate;
   vRates.clear ();

   // lock parser ...
   mutex.lock();

   xmlSr.clear();
   xmlSr.addData(sResp);

   while(!xmlSr.atEnd() && !xmlSr.hasError())
   {
      switch (xmlSr.readNext())
      {
      // any xml element starts ...
      case QXmlStreamReader::StartElement:
         if (xmlSr.name() == "item")
         {
            sRate.iRateID = -1;
            sRate.sAccess = "";
            sRate.sGenre  = "";
         }
         else if (xmlSr.name() == "id_rate")
         {
            // read id ...
            if (xmlSr.readNext() == QXmlStreamReader::Characters)
            {
               sRate.iRateID = xmlSr.text().toString().toUInt();
            }
         }
         else if (xmlSr.name() == "rate_name")
         {
            // read genre name  ...
            if (xmlSr.readNext() == QXmlStreamReader::Characters)
            {
               sRate.sGenre = xmlSr.text().toString();
            }
         }
         else if (xmlSr.name() == "action")
         {
            // read access  ...
            if (xmlSr.readNext() == QXmlStreamReader::Characters)
            {
               sRate.sAccess = xmlSr.text().toString();
            }
         }
         break;

      // any xml element ends ...
      case QXmlStreamReader::EndElement:
         if (xmlSr.name() == "item")
         {
            // add rate ...
            vRates.append(sRate);
         }
         break;

      default:
         break;

      } // end switch ...

   } // end while ...

   // check for xml errors ...
   if(xmlSr.hasError())
   {
      QMessageBox::critical(NULL, tr("Error in %1").arg(__FUNCTION__),
                            tr("XML Error String: %1").arg(xmlSr.errorString()));

      iRV = -1;
   }

   // unlock parser ...
   mutex.unlock();

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: parseVodList
|  Begin: 09.12.2010 / 13:27
|  Author: Jo2003
|  Description: parse vod list
|
|  Parameters: ref. to response, ref. to vod list vector,
|              ref. to genre info struct
|
|  Returns: 0 --> ok
|        else --> any error
\----------------------------------------------------------------- */
int CKartinaXMLParser::parseVodList(const QString &sResp, QVector<cparser::SVodVideo> &vVodList, cparser::SGenreInfo &gInfo)
{
   int                    iRV = 0;
   QStringList            slNeeded;
   QMap<QString, QString> mResults;
   cparser::SVodVideo     vod;

   // clear vod list ...
   vVodList.clear();

   // lock parser ...
   mutex.lock();

   xmlSr.clear();
   xmlSr.addData(sResp);

   while(!xmlSr.atEnd() && !xmlSr.hasError())
   {
      switch (xmlSr.readNext())
      {
      // any xml element starts ...
      case QXmlStreamReader::StartElement:
         if (xmlSr.name() == "response")
         {
            mResults.clear();
            slNeeded.clear();

            // we need following data ...
            slNeeded << "type" << "total" << "count" << "page";

            oneLevelParser("page", slNeeded, mResults);

            gInfo.sType  = mResults.value("type");
            gInfo.iCount = mResults.value("count").toInt();
            gInfo.iPage  = mResults.value("page").toInt();
            gInfo.iTotal = mResults.value("total").toInt();

            mInfo(tr("Got Type: %1, Count: %2, Page: %3, Total: %4")
                  .arg(gInfo.sType)
                  .arg(gInfo.iCount)
                  .arg(gInfo.iPage)
                  .arg(gInfo.iTotal));
         }
         else if (xmlSr.name() == "item")
         {
            mResults.clear();
            slNeeded.clear();

            // we need following data ...
            slNeeded << "id" << "name" << "description" << "year" << "country" << "poster" << "pass_protect" << "favorite";

            oneLevelParser("item", slNeeded, mResults);

            vod.uiVidId    =   mResults.value("id").toUInt();
            vod.sName      =   mResults.value("name");
            vod.sDescr     =   mResults.value("description");
            vod.sYear      =   mResults.value("year");
            vod.sCountry   =   mResults.value("country");
            vod.sImg       =   mResults.value("poster");
            vod.bProtected = !!mResults.value("pass_protect").toInt();
            vod.bFavourit  = !!mResults.value("favorite").toInt();

            // store element ...
            vVodList.push_back(vod);
         }
         break;

      default:
         break;

      } // end switch ...

   } // end while ...

   // check for xml errors ...
   if(xmlSr.hasError())
   {
      QMessageBox::critical(NULL, tr("Error in %1").arg(__FUNCTION__),
                            tr("XML Error String: %1").arg(xmlSr.errorString()));

      iRV = -1;
   }

   // unlock parser ...
   mutex.unlock();

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: parseVideoInfo
|  Begin: 09.12.2010 / 16:27
|  Author: Jo2003
|  Description: parse video info
|
|  Parameters: ref. to response, ref. to video info struct
|
|  Returns: 0 --> ok
|        else --> any error
\----------------------------------------------------------------- */
int CKartinaXMLParser::parseVideoInfo(const QString &sResp, cparser::SVodVideo &vidInfo)
{
   int                    iRV = 0;
   QStringList            slNeeded;
   QMap<QString, QString> mResults;
   cparser::SVodFileInfo  fInfo;
   bool                   bEnd = false;

   // lock parser ...
   mutex.lock();

   // init struct ...
   vidInfo.sActors    = "";
   vidInfo.sCountry   = "";
   vidInfo.sDescr     = "";
   vidInfo.sDirector  = "";
   vidInfo.sImg       = "";
   vidInfo.sName      = "";
   vidInfo.sYear      = "";
   vidInfo.uiLength   = 0;
   vidInfo.uiVidId    = 0;
   vidInfo.bProtected = false;
   vidInfo.bFavourit  = false;
   vidInfo.vVodFiles.clear();

   xmlSr.clear();
   xmlSr.addData(sResp);

   while(!xmlSr.atEnd() && !xmlSr.hasError() && !bEnd)
   {
      switch (xmlSr.readNext())
      {
      // any xml element starts ...
      case QXmlStreamReader::StartElement:
         if (xmlSr.name() == "film")
         {
            mResults.clear();
            slNeeded.clear();

            slNeeded << "name" << "lenght" << "description" << "actors"
                     << "country" << "director" << "poster" << "year"
                     << "id" << "genre_str";

            oneLevelParser("vis", slNeeded, mResults);

            vidInfo.sActors   = mResults.value("actors");
            vidInfo.sCountry  = mResults.value("country");
            vidInfo.sDescr    = mResults.value("description");
            vidInfo.sDirector = mResults.value("director");
            vidInfo.sImg      = mResults.value("poster");
            vidInfo.sName     = mResults.value("name");
            vidInfo.sYear     = mResults.value("year");
            vidInfo.sGenres   = mResults.value("genre_str");
            vidInfo.uiLength  = mResults.value("lenght").toUInt();
            vidInfo.uiVidId   = mResults.value("id").toUInt();
         }
         else if (xmlSr.name() == "item")
         {
            mResults.clear();
            slNeeded.clear();

            slNeeded << "id" << "title" << "format" << "url"
                     << "size" << "length" << "codec" << "width"
                     << "height";

            // parse vod parts ...
            oneLevelParser("item", slNeeded, mResults);

            fInfo.iHeight = mResults.value("height").toInt();
            fInfo.iId     = mResults.value("id").toInt();
            fInfo.iLength = mResults.value("length").toInt();
            fInfo.iSize   = mResults.value("size").toInt();
            fInfo.iWidth  = mResults.value("width").toInt();
            fInfo.sCodec  = mResults.value("codec");
            fInfo.sFormat = mResults.value("format");
            fInfo.sTitle  = mResults.value("title");
            fInfo.sUrl    = mResults.value("url");

            vidInfo.vVodFiles.push_back(fInfo);
         }
         else if (xmlSr.name() == "genres")
         {
            // there is nothing we need from genres ...
            ignoreUntil("genres");
         }
         else if (xmlSr.name() == "pass_protect")
         {
            if (xmlSr.readNext() == QXmlStreamReader::Characters)
            {
               vidInfo.bProtected = !!xmlSr.text().toString().toInt();
            }
         }
         else if (xmlSr.name() == "favorite")
         {
            if (xmlSr.readNext() == QXmlStreamReader::Characters)
            {
               vidInfo.bFavourit = !!xmlSr.text().toString().toInt();
            }
         }
         break;

      case QXmlStreamReader::EndElement:
         // end of videos means end of needed info ...
         if (xmlSr.name() == "film")
         {
            bEnd = true;
         }
         break;

      default:
         break;
      }
   }

   // check for xml errors ...
   if(xmlSr.hasError())
   {
      QMessageBox::critical(NULL, tr("Error in %1").arg(__FUNCTION__),
                            tr("XML Error String: %1").arg(xmlSr.errorString()));

      iRV = -1;
   }

   // unlock parser ...
   mutex.unlock();

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: parseEpg
|  Begin: 29.07.2010 / 11:28:20
|  Author: Jo2003
|  Description: parse epg xml
|
|  Parameters: ref. to response, ref. to epgList
|
|  Returns: 0 --> ok
|        else --> any error
\----------------------------------------------------------------- */
int CKartinaXMLParser::parseEpg (const QString &sResp, QVector<cparser::SEpg> &epgList)
{
   QRegExp       rx("([^\n]*)[\n]*(.*)");
   QMap<QString, QString> mResults;
   QStringList            slNeeded;
   cparser::SEpg epg;
   int           iRV = 0;

   // clear epg list ...
   epgList.clear();

   // lock parser ...
   mutex.lock();

   xmlSr.clear();
   xmlSr.addData(sResp);

   slNeeded << "ut_start" << "progname" << "pdescr";

   while(!xmlSr.atEnd() && !xmlSr.hasError())
   {
      switch (xmlSr.readNext())
      {
      case QXmlStreamReader::StartElement:

         if (xmlSr.name() == "item")
         {
            epg.sDescr = "";

            oneLevelParser("item", slNeeded, mResults);

            epg.uiGmt = mResults.value("ut_start").toUInt();

            // program description will be after '\n' ...
            if (rx.indexIn(mResults.value("progname")) > -1)
            {
               // yes, program description there ...
               epg.sName  = rx.cap(1);
               epg.sDescr = rx.cap(2);
            }
            else
            {
               // program name only ...
               epg.sName = mResults.value("progname");
            }

            // is there a description ... ?
            if (mResults.value("pdescr") != "")
            {
               epg.sDescr = mResults.value("pdescr");
            }

            // store element ...
            epgList.push_back(epg);
         }
         break;

      default:
         break;

      } // end switch ...

   } // end while ...

   // check for xml errors ...
   if(xmlSr.hasError())
   {
      QMessageBox::critical(NULL, tr("Error in %1").arg(__FUNCTION__),
                            tr("XML Error String: %1").arg(xmlSr.errorString()));

      iRV = -1;
   }

   // unlock parser ...
   mutex.unlock();

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: parseSettings
|  Begin: 29.07.2010 / 11:28:20
|  Author: Jo2003
|  Description: parse settings xml response
|
|  Parameters: ref. to response, ref. to value vector,
|              ref. to act. val
|
|  Returns: 0 --> ok
|        else --> any error
\----------------------------------------------------------------- */
int CKartinaXMLParser::parseSettings (const QString &sResp, QVector<int> &vValues,
                                      int &iActVal, QString &sName)
{
   int iRV = 0;

   // clear epg list ...
   vValues.clear();

   // lock parser ...
   mutex.lock();

   xmlSr.clear();
   xmlSr.addData(sResp);

   while(!xmlSr.atEnd() && !xmlSr.hasError())
   {
      switch (xmlSr.readNext())
      {
      // we aren't interested in ...
      case QXmlStreamReader::StartDocument:
      case QXmlStreamReader::EndDocument:
      case QXmlStreamReader::EndElement:
         break;

      // any xml element starts ...
      case QXmlStreamReader::StartElement:
         if (xmlSr.name() == "item")
         {
            // read item ...
            if (xmlSr.readNext() == QXmlStreamReader::Characters)
            {
               vValues.push_back(xmlSr.text().toString().toInt());
            }
         }
         else if (xmlSr.name() == "value")
         {
            // read actual value ...
            if (xmlSr.readNext() == QXmlStreamReader::Characters)
            {
               iActVal = xmlSr.text().toString().toInt();
            }
         }
         else if (xmlSr.name() == "name")
         {
            // read actual value ...
            if (xmlSr.readNext() == QXmlStreamReader::Characters)
            {
               sName = xmlSr.text().toString();
            }
         }
         break;

      default:
         break;

      } // end switch ...

   } // end while ...

   // check for xml errors ...
   if(xmlSr.hasError())
   {
      QMessageBox::critical(NULL, tr("Error in %1").arg(__FUNCTION__),
                            tr("XML Error String: %1").arg(xmlSr.errorString()));

      iRV = -1;
   }

   // unlock parser ...
   mutex.unlock();

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: parseSetting
|  Begin: 11.05.2012
|  Author: Jo2003
|  Description: parse one setting in login response
|
|  Parameters: ref. to response, name of setting,
|              vector to store values, active value
|
|  Returns: 0 --> ok
|        else --> any error
\----------------------------------------------------------------- */
int CKartinaXMLParser::parseSetting(const QString& sResp, const QString &sName, QVector<int>& vValues, int& iActVal)
{
   int  iRV      = 0;
   bool bAtEnd   = false;
   bool bStarted = false;

   // clear epg list ...
   vValues.clear();

   // lock parser ...
   mutex.lock();

   xmlSr.clear();
   xmlSr.addData(sResp);

   while(!xmlSr.atEnd() && !xmlSr.hasError() && !bAtEnd)
   {
      switch (xmlSr.readNext())
      {
      // we aren't interested in ...
      case QXmlStreamReader::StartDocument:
      case QXmlStreamReader::EndDocument:
         break;

      // any xml element starts ...
      case QXmlStreamReader::StartElement:
         if (xmlSr.name() == sName)
         {
            bStarted = true;
         }

         if (bStarted)
         {
            if (xmlSr.name() == "item")
            {
               // read item ...
               if (xmlSr.readNext() == QXmlStreamReader::Characters)
               {
                  vValues.push_back(xmlSr.text().toString().toInt());
               }
            }
            else if (xmlSr.name() == "value")
            {
               // read actual value ...
               if (xmlSr.readNext() == QXmlStreamReader::Characters)
               {
                  iActVal = xmlSr.text().toString().toInt();
               }
            }
         }
         break;

      case QXmlStreamReader::EndElement:
         if (xmlSr.name() == sName)
         {
            bStarted = false;
            bAtEnd   = true;
         }
         break;

      default:
         break;

      } // end switch ...

   } // end while ...

   // check for xml errors ...
   if(xmlSr.hasError())
   {
      QMessageBox::critical(NULL, tr("Error in %1").arg(__FUNCTION__),
                            tr("XML Error String: %1").arg(xmlSr.errorString()));

      iRV = -1;
   }

   // unlock parser ...
   mutex.unlock();

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: parseUrl
|  Begin: 30.07.2010 / 11:28:20
|  Author: Jo2003
|  Description: parse url response
|
|  Parameters: ref. to response, ref. to url
|
|  Returns: 0 --> ok
|        else --> any error
\----------------------------------------------------------------- */
int CKartinaXMLParser::parseUrl(const QString &sResp, QString &sUrl)
{
   QRegExp rx("<url>([^<]*).*");

   int     iRV = 0;

   sUrl = "";

   // use reg. expressions instead of xml stream parser ...
   if (rx.indexIn(sResp) > -1)
   {
      sUrl = rx.cap(1);

      // ignore given mrl options ...
      if (sUrl.contains(QChar(' ')))
      {
         sUrl = sUrl.left(sUrl.indexOf(QChar(' ')));
      }
   }
   else
   {
      iRV = -1;
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: parseVodUrls
|  Begin: 11.05.2012
|  Author: Jo2003
|  Description: parse vod url response (may include ad_url
|
|  Parameters: ref. to response, ref. to stringlist
|
|  Returns: 0 --> ok
|        else --> any error
\----------------------------------------------------------------- */
int CKartinaXMLParser::parseVodUrls (const QString& sResp, QStringList& sUrls)
{
   int     iRV = 0;
   QString sUrl, sAdUrl;

   // clear string list ...
   sUrls.clear();

   // lock parser ...
   mutex.lock();

   xmlSr.clear();
   xmlSr.addData(sResp);

   while(!xmlSr.atEnd() && !xmlSr.hasError())
   {
      switch (xmlSr.readNext())
      {
      // we aren't interested in ...
      case QXmlStreamReader::StartDocument:
      case QXmlStreamReader::EndDocument:
      case QXmlStreamReader::EndElement:
         break;

      // any xml element starts ...
      case QXmlStreamReader::StartElement:
         if (xmlSr.name() == "url")
         {
            if (xmlSr.readNext() == QXmlStreamReader::Characters)
            {
               sUrl = xmlSr.text().toString();
               if (sUrl.contains(QChar(' ')))
               {
                  sUrl = sUrl.left(sUrl.indexOf(QChar(' ')));
               }
            }
         }
         else if (xmlSr.name() == "ad_url")
         {
            if (xmlSr.readNext() == QXmlStreamReader::Characters)
            {
               sAdUrl = xmlSr.text().toString();
               if (sAdUrl.contains(QChar(' ')))
               {
                  sAdUrl = sAdUrl.left(sAdUrl.indexOf(QChar(' ')));
               }
            }
         }
         break;

      default:
         break;

      } // end switch ...

   } // end while ...

   // fill string list with url(s) ...
   sUrls << sUrl;
   if (sAdUrl != "")
   {
      sUrls << sAdUrl;
   }

   // check for xml errors ...
   if(xmlSr.hasError())
   {
      QMessageBox::critical(NULL, tr("Error in %1").arg(__FUNCTION__),
                            tr("XML Error String: %1").arg(xmlSr.errorString()));

      iRV = -1;
   }

   // unlock parser ...
   mutex.unlock();

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: xmlElementToValue
|  Begin: 22.12.2010 / 11:45
|  Author: Jo2003
|  Description: a (very) simple but handy xml element parser
|
|  Parameters: ref. to string result, ref to name
|
|  Returns: value string
\----------------------------------------------------------------- */
QString CKartinaXMLParser::xmlElementToValue(const QString &sElement, const QString &sName)
{
   QString sValue;
   QString sRegEx = QString("<%1>([^<]+)</%1>").arg(sName);

   QRegExp rx(sRegEx);

   if (rx.indexIn(sElement) > -1)
   {
      sValue = rx.cap(1);
   }

   return sValue;
}

/* -----------------------------------------------------------------\
|  Method: oneLevelParser
|  Begin: 25.01.2011 / 9:50
|  Author: Jo2003
|  Description: parse one level xml
|
|  Parameters: ref. to stop element. list with needed values,
|              ref. to result map
|
|  Returns: 0
\----------------------------------------------------------------- */
int CKartinaXMLParser::oneLevelParser(const QString &sEndElement, const QStringList &slNeeded, QMap<QString, QString> &mResults)
{
   QString sUnknown, sKey, sVal;
   mResults.clear();
   bool bEndMain = false;

   while(!xmlSr.atEnd() && !xmlSr.hasError() && !bEndMain)
   {
      switch (xmlSr.readNext())
      {
      // start element ...
      case QXmlStreamReader::StartElement:

         // needed element ... ?
         if (slNeeded.contains(xmlSr.name().toString()))
         {
            // store key / value in map ...
            // make sure we add an empty string if there is no text
            // inside this element.
            sKey = xmlSr.name().toString();
            sVal = "";

            if (xmlSr.readNext() == QXmlStreamReader::Characters)
            {
               sVal = xmlSr.text().toString();
            }

            mResults.insert(sKey, sVal);
         }
         else if (xmlSr.name().toString() == sEndElement)
         {
            // maybe end element isn't searched ...
            // to get the end element we should NOT count it
            // as unknown ...
         }
         else
         {
            // starttag unknown element ...
            sUnknown = xmlSr.name().toString();

#ifndef QT_NO_DEBUG
            mInfo(tr("Found unused element %1 ...").arg(sUnknown));
#endif

            // search for endtag of unknown element ...
            ignoreUntil(sUnknown);
         }
         break;

      case QXmlStreamReader::EndElement:
         if (xmlSr.name().toString() == sEndElement)
         {
            bEndMain = true;
         }
         break;

      default:
         break;
      }
   }

   return 0;
}

/* -----------------------------------------------------------------\
|  Method: ignoreUntil
|  Begin: 30.05.2012
|  Author: Jo2003
|  Description: ignore XML tree 'til we found end element (or error)
|
|  Parameters: end element
|
|  Returns: 0 --> ok (ignored 'til end element)
|          -1 --> end element not found or error
\----------------------------------------------------------------- */
int CKartinaXMLParser::ignoreUntil(const QString &sEndElement)
{
   while(!xmlSr.atEnd() && !xmlSr.hasError())
   {
      if ((xmlSr.readNext() == QXmlStreamReader::EndElement)
         && (xmlSr.name().toString() == sEndElement))
      {
         // found end tag of searched element ...
         break;
      }
   }

   return (xmlSr.atEnd() || xmlSr.hasError()) ? -1 : 0;
}

/* -----------------------------------------------------------------\
|  Method: parseUpdInfo
|  Begin: 12.10.2011
|  Author: Jo2003
|  Description: parse update info xml
|
|  Parameters: response string, buffer for info
|
|  Returns: 0 --> ok
|          -1 --> any error
\----------------------------------------------------------------- */
int CKartinaXMLParser::parseUpdInfo(const QString &sResp, cparser::SUpdInfo &updInfo)
{
   int                    iRV = 0;
   QStringList            slNeeded;
   QMap<QString, QString> mResults;

   QString sSys = "n.a.";

#if defined Q_OS_WIN32
   sSys = "win";
#elif defined Q_OS_LINUX
   sSys = "nix";
#elif defined Q_OS_MAC
   sSys = "osx";
#endif

   // clear updInfo struct ...
   updInfo.iMajor   = 0;
   updInfo.iMinor   = 0;
   updInfo.sVersion = "";
   updInfo.sUrl     = "";

   // lock parser ...
   mutex.lock();

   xmlSr.clear();
   xmlSr.addData(sResp);

   while(!xmlSr.atEnd() && !xmlSr.hasError())
   {
      switch (xmlSr.readNext())
      {
      // any xml element starts ...
      case QXmlStreamReader::StartElement:
         if (xmlSr.name() == sSys)
         {
            mResults.clear();
            slNeeded.clear();

            slNeeded << "string_version" << "major" << "minor" << "link";

            oneLevelParser(sSys, slNeeded, mResults);

            updInfo.iMajor   = mResults.value("major").toInt();
            updInfo.iMinor   = mResults.value("minor").toInt();
            updInfo.sVersion = mResults.value("string_version");
            updInfo.sUrl     = mResults.value("link");
         }
         break;

      default:
         break;
      }
   }

   // check for xml errors ...
   if(xmlSr.hasError())
   {
      QMessageBox::critical(NULL, tr("Error in %1").arg(__FUNCTION__),
                            tr("XML Error String: %1").arg(xmlSr.errorString()));

      iRV = -1;
   }

   // unlock parser ...
   mutex.unlock();

   return iRV;
}

/*=============================================================================\
|                                    History:
| ---------------------------------------------------------------------------
| 05.Jan.2010 -
\=============================================================================*/

