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
   int           iRV;

   // clear channel list ...
   chanList.clear();

   // check for errors ...
   iRV = checkResponse(sResp, __FUNCTION__, __LINE__);

   if (!iRV)
   {
      xmlSr.clear();
      xmlSr.addData(sCleanResp);

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
   }

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
   int           iRV;

   // clear epg list ...
   vSrv.clear();

   // check for errors ...
   iRV = checkResponse(sResp, __FUNCTION__, __LINE__);

   if (!iRV)
   {
      xmlSr.clear();
      xmlSr.addData(sCleanResp);

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
   }

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
int CKartinaXMLParser::parseCookie (const QString &sResp, QString &sCookie, QString &sExp)
{
   QString                sSid, sSidName;
   QStringList            slNeeded;
   QMap<QString, QString> mResults;
   sCookie = "";

   slNeeded << "login" << "packet_name" << "packet_expire";

   // check for errors ...
   int iRV = checkResponse(sResp, __FUNCTION__, __LINE__);

   if (!iRV)
   {
      xmlSr.clear();
      xmlSr.addData(sCleanResp);

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
               // get expires value ...
               oneLevelParser("account", slNeeded, mResults);

               // format into string ...
               sExp = QDateTime::fromTime_t(mResults.value("packet_expire").toUInt())
                      .toString(DEF_TIME_FORMAT);
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
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: parseGenres
|  Begin: 09.12.2010 / 13:27
|  Author: Jo2003
|  Description: parse genre html (when api supports xml, we must
|               change this function
|
|  Parameters: ref. to response, ref. to ganres vector
|
|  Returns: 0 --> ok
|        else --> any error
\----------------------------------------------------------------- */
int CKartinaXMLParser::parseGenres (const QString& sResp, QVector<cparser::SGenre>& vGenres)
{
   cparser::SGenre sGenre;
   vGenres.clear ();

   // check for errors ...
   int iRV = checkResponse(sResp, __FUNCTION__, __LINE__);

   if (!iRV)
   {
      xmlSr.clear();
      xmlSr.addData(sCleanResp);

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
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: parseVodList
|  Begin: 09.12.2010 / 13:27
|  Author: Jo2003
|  Description: parse vod list
|
|  Parameters: ref. to response, ref. to vod list vector
|
|  Returns: 0 --> ok
|        else --> any error
\----------------------------------------------------------------- */
int CKartinaXMLParser::parseVodList(const QString &sResp, QVector<cparser::SVodVideo> &vVodList)
{
   QStringList            slNeeded;
   QMap<QString, QString> mResults;
   cparser::SVodVideo     vod;

   // clear vod list ...
   vVodList.clear();

   // check for errors ...
   int iRV = checkResponse(sResp, __FUNCTION__, __LINE__);

   if (!iRV)
   {
      // we need following data ...
      slNeeded << "id" << "name" << "description" << "year" << "country" << "poster";

      xmlSr.clear();
      xmlSr.addData(sCleanResp);

      while(!xmlSr.atEnd() && !xmlSr.hasError())
      {
         switch (xmlSr.readNext())
         {
         // any xml element starts ...
         case QXmlStreamReader::StartElement:

            if (xmlSr.name() == "item")
            {
               oneLevelParser("item", slNeeded, mResults);

               vod.uiVidId  = mResults.value("id").toUInt();
               vod.sName    = mResults.value("name");
               vod.sDescr   = mResults.value("description");
               vod.sYear    = mResults.value("year");
               vod.sCountry = mResults.value("country");
               vod.sImg     = mResults.value("poster");

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
   }

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
   QString sUnknown;

   // check for errors ...
   int iRV = checkResponse(sResp, __FUNCTION__, __LINE__);

   if (!iRV)
   {
      xmlSr.clear();
      xmlSr.addData(sCleanResp);

      while(!xmlSr.atEnd() && !xmlSr.hasError())
      {
         switch (xmlSr.readNext())
         {
         // any xml element starts ...
         case QXmlStreamReader::StartElement:
            if (xmlSr.name() == "film")
            {
               // init struct ...
               vidInfo.sActors   = "";
               vidInfo.sCountry  = "";
               vidInfo.sDescr    = "";
               vidInfo.sDirector = "";
               vidInfo.sImg      = "";
               vidInfo.sName     = "";
               vidInfo.sYear     = "";
               vidInfo.uiLength  = 0;
               vidInfo.uiVidId   = 0;
               vidInfo.vVodFiles.clear();
            }
            else if (xmlSr.name() == "name")
            {
               // video name ...
               if (xmlSr.readNext() == QXmlStreamReader::Characters)
               {
                  vidInfo.sName = xmlSr.text().toString();
               }
            }
            else if (xmlSr.name() == "lenght")
            {
               // video length ...
               if (xmlSr.readNext() == QXmlStreamReader::Characters)
               {
                  vidInfo.uiLength = xmlSr.text().toString().toUInt();
               }
            }
            else if (xmlSr.name() == "description")
            {
               // video description ...
               if (xmlSr.readNext() == QXmlStreamReader::Characters)
               {
                  vidInfo.sDescr = xmlSr.text().toString();
               }
            }
            else if (xmlSr.name() == "actors")
            {
               // video actors ...
               if (xmlSr.readNext() == QXmlStreamReader::Characters)
               {
                  vidInfo.sActors = xmlSr.text().toString();
               }
            }
            else if (xmlSr.name() == "country")
            {
               // country ...
               if (xmlSr.readNext() == QXmlStreamReader::Characters)
               {
                  vidInfo.sCountry = xmlSr.text().toString();
               }
            }
            else if (xmlSr.name() == "director")
            {
               // director ...
               if (xmlSr.readNext() == QXmlStreamReader::Characters)
               {
                  vidInfo.sDirector = xmlSr.text().toString();
               }
            }
            else if (xmlSr.name() == "poster")
            {
               // image ...
               if (xmlSr.readNext() == QXmlStreamReader::Characters)
               {
                  vidInfo.sImg = xmlSr.text().toString();
               }
            }
            else if (xmlSr.name() == "year")
            {
               // year ...
               if (xmlSr.readNext() == QXmlStreamReader::Characters)
               {
                  vidInfo.sYear = xmlSr.text().toString();
               }
            }
            else if (xmlSr.name() == "id")
            {
               // year ...
               if (xmlSr.readNext() == QXmlStreamReader::Characters)
               {
                  vidInfo.uiVidId = xmlSr.text().toString().toUInt();
               }
            }
            else if (xmlSr.name() == "videos")
            {
               // parse vod parts ...
               parseVodParts (xmlSr, vidInfo.vVodFiles);
            }
            else if (xmlSr.name() == "response")
            {
               // not needed ...
            }
            else
            {
               // any unknown element shouldn't break our parser ...
               sUnknown = xmlSr.name().toString();

#ifndef QT_NO_DEBUG
               mInfo(tr("Found unused element %1 ...").arg(sUnknown));
#endif
               while (!((xmlSr.readNext() == QXmlStreamReader::EndElement)
                  && (xmlSr.name().toString() == sUnknown)))
               {
#ifndef QT_NO_DEBUG
                  mInfo(tr("Found unused child %1: %2 ...")
                       .arg(xmlSr.name().toString()).arg(xmlSr.text().toString()));
#endif
               }
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
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: parseVodParts
|  Begin: 24.01.2011 / 20:50
|  Author: Jo2003
|  Description: parse vod video parts
|
|  Parameters: ref. to xml parser, ref. to vod parts vector
|
|  Returns: 0
\----------------------------------------------------------------- */
int CKartinaXMLParser::parseVodParts (QXmlStreamReader &xml, QVector<uint>& vVodParts)
{
   QString sUnknown;

   vVodParts.clear();

   while (!((xml.readNext() == QXmlStreamReader::EndElement)
      && (xml.name() == "videos")))
   {
      if (xml.tokenType() == QXmlStreamReader::StartElement)
      {
         if (xml.name() == "item")
         {
            // not needed ...
         }
         else if (xml.name() == "id")
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               vVodParts.push_back(xml.text().toString().toUInt());
            }
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
   int           iRV;

   // clear epg list ...
   epgList.clear();

   // check for errors ...
   iRV = checkResponse(sResp, __FUNCTION__, __LINE__);

   if (!iRV)
   {
      slNeeded << "ut_start" << "progname" << "pdescr";

      xmlSr.clear();
      xmlSr.addData(sCleanResp);

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
   }

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
   int     iRV;

   // clear epg list ...
   vValues.clear();

   // check for errors ...
   iRV = checkResponse(sResp, __FUNCTION__, __LINE__);

   if (!iRV)
   {
      xmlSr.clear();
      xmlSr.addData(sCleanResp);

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
   }

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
   QRegExp rx("<url>([^ ]*).*");

   int     iRV = 0;

   sUrl = "";

   // error check ...
   iRV = checkResponse(sResp, __FUNCTION__, __LINE__);

   if (!iRV)
   {
      // use reg. expressions instead of xml stream parser ...
      if (rx.indexIn(sCleanResp) > -1)
      {
         sUrl = rx.cap(1);
      }
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: checkResponse
|  Begin: 28.07.2010 / 18:42:54
|  Author: Jo2003
|  Description: format kartina error string
|
|  Parameters: --
|
|  Returns: error string
\----------------------------------------------------------------- */
int CKartinaXMLParser::checkResponse (const QString &sResp, const QString &sFunction, int iLine)
{
   int iRV = 0;

   // clean response ... (delete content which may come
   // after / before the xml code ...
   QString sEndTag = "</response>";
   int iStartPos   = sResp.indexOf("<?xml");
   int iEndPos     = sResp.indexOf(sEndTag) + sEndTag.length();

   // store clean string in private variable ...
   sCleanResp      = sResp.mid(iStartPos, iEndPos - iStartPos);

   QRegExp rx("<message>(.*)</message>[ \t\n\r]*"
              "<code>(.*)</code>");

   sErr = "";

   // quick'n'dirty error check ...
   if (sCleanResp.contains("<error>"))
   {
      if (rx.indexIn(sCleanResp) > -1)
      {
         iRV  = rx.cap(2).toInt();
         sErr = tr("Error #%1 in %2():%3: %4")
                .arg(iRV)
                .arg(sFunction)
                .arg(iLine)
                .arg(rx.cap(1));

         QMessageBox::critical(NULL, tr("Error"), sErr);
         mErr(QString("\n --> %1").arg(sErr));
      }
   }

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
   QString sUnknown, sKey;
   mResults.clear();

   while (!((xmlSr.readNext() == QXmlStreamReader::EndElement)
      && (xmlSr.name().toString() == sEndElement)))
   {
      if (xmlSr.tokenType() == QXmlStreamReader::StartElement)
      {
         if (slNeeded.contains(xmlSr.name().toString()))
         {
            sKey = xmlSr.name().toString();

            if (xmlSr.readNext() == QXmlStreamReader::Characters)
            {
               mResults.insert(sKey, xmlSr.text().toString());
            }
         }
         else
         {
            // any unknown element shouldn't break our parser ...
            sUnknown = xmlSr.name().toString();

#ifndef QT_NO_DEBUG
            mInfo(tr("Found unused element %1 ...").arg(sUnknown));
#endif
            while (!((xmlSr.readNext() == QXmlStreamReader::EndElement)
               && (xmlSr.name().toString() == sUnknown)))
            {
#ifndef QT_NO_DEBUG
               mInfo(tr("Found unused child %1: %2 ...")
                    .arg(xmlSr.name().toString()).arg(xmlSr.text().toString()));
#endif
            }
         }
      }
   }

   return 0;
}

/*=============================================================================\
|                                    History:
| ---------------------------------------------------------------------------
| 05.Jan.2010 -
\=============================================================================*/

