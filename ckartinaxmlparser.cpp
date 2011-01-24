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

            while (!((xml.readNext() == QXmlStreamReader::EndElement)
               && (xml.name().toString() == sUnknown)))
            {
               mInfo(tr("Found not supported element %1 ...").arg(xml.name().toString()));
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

            while (!((xml.readNext() == QXmlStreamReader::EndElement)
               && (xml.name().toString() == sUnknown)))
            {
               mInfo(tr("Found unknown element %1 ...").arg(xml.name().toString()));
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

            while (!((xml.readNext() == QXmlStreamReader::EndElement)
               && (xml.name().toString() == sUnknown)))
            {
               mInfo(tr("Found unknown element %1 ...").arg(xml.name().toString()));
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
|  Parameters: XML in, buffer for cookie
|
|  Returns: 0 ==> ok
|        else ==> any error
\----------------------------------------------------------------- */
int CKartinaXMLParser::parseCookie (const QString &sResp, QString &sCookie)
{
   QRegExp rx("<sid>(.*)</sid>.*"
              "<sid_name>(.*)</sid_name>.*"
              "<servertime>(.*)</servertime>");

   int     iRV = 0;

   sCookie = "";

   // error check ...
   iRV = checkResponse(sResp, __FUNCTION__, __LINE__);

   if (!iRV)
   {
      // use reg. expressions instead of xml stream parser ...
      if (rx.indexIn(sCleanResp) > -1)
      {
         sCookie = QString("%1=%2").arg(rx.cap(2)).arg(rx.cap(1));

         // check offset ...
         checkTimeOffSet (rx.cap(3).toUInt());
      }
      else
      {
         mInfo("RegEx doesn't match ...");
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
   int             iRV = 0;
   QXmlQuery       query;
   QStringList     slGenre;
   QStringList     slGenId;
   cparser::SGenre sGenre;

   iRV = checkResponse(sResp, __FUNCTION__, __LINE__);

   if (!iRV)
   {
      query.setFocus(sCleanResp);
      query.setQuery("/response/genres/item/id/string()");
      query.evaluateTo(&slGenId);

      query.setQuery("/response/genres/item/name/string()");
      query.evaluateTo(&slGenre);

      if (slGenId.count() && slGenre.count() && (slGenId.count() == slGenre.count()))
      {
         for (int i = 0; i < slGenId.count(); i++)
         {
            sGenre.sGName = slGenre[i];
            sGenre.uiGid  = slGenId[i].toUInt();

            // add genre to vector ...
            vGenres.push_back(sGenre);
         }
      }

      if (vGenres.count() == 0)
      {
         iRV = -1;
         mInfo("No Genres found ...");
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
   int                iRV;
   cparser::SVodVideo vod;
   QDomDocument       doc;

   // clear vod list ...
   vVodList.clear();

   iRV = checkResponse(sResp, __FUNCTION__, __LINE__);

   if (!iRV)
   {
      doc.setContent(sCleanResp);

      QDomNodeList dlIds     = doc.elementsByTagName("id");
      QDomNodeList dlNames   = doc.elementsByTagName("name");
      QDomNodeList dlDescs   = doc.elementsByTagName("description");
      QDomNodeList dlYear    = doc.elementsByTagName("year");
      QDomNodeList dlCountry = doc.elementsByTagName("country");
      QDomNodeList dlImg     = doc.elementsByTagName("poster");

      int iNumb = dlIds.count();

      for (int i = 0; i < iNumb; i++)
      {
         vod.uiVidId  = dlIds.item(i).firstChild().nodeValue().toUInt();
         vod.sName    = dlNames.item(i).firstChild().nodeValue();
         vod.sDescr   = dlDescs.item(i).firstChild().nodeValue();
         vod.sYear    = dlYear.item(i).firstChild().nodeValue();
         vod.sCountry = dlCountry.item(i).firstChild().nodeValue();
         vod.sImg     = dlImg.item(i).firstChild().nodeValue();

         vVodList.push_back(vod);
      }

      iRV = vVodList.count() ? 0 : -1;
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
   int         iRV = 0;
   QStringList result;
   QStringList::const_iterator cit;
   QXmlQuery   query;

   iRV = checkResponse(sResp, __FUNCTION__, __LINE__);

   if (!iRV)
   {
      query.setFocus(sCleanResp);

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

      // name ...
      result.clear();
      query.setQuery("/response/film/name/string()");
      iRV |= (query.evaluateTo(&result)) ? 0 : -1;
      vidInfo.sName = result[0];

      // length ...
      result.clear();
      query.setQuery("/response/film/lenght/string()"); // nice typo ...
      iRV |= (query.evaluateTo(&result)) ? 0 : -1;
      vidInfo.uiLength = result[0].toUInt();

      // description ...
      result.clear();
      query.setQuery("/response/film/description/string()");
      iRV |= (query.evaluateTo(&result)) ? 0 : -1;
      vidInfo.sDescr = result[0];
      vidInfo.sDescr.replace("", "");

      // actors ...
      result.clear();
      query.setQuery("/response/film/actors/string()");
      iRV |= (query.evaluateTo(&result)) ? 0 : -1;
      vidInfo.sActors = result[0];

      // country ...
      result.clear();
      query.setQuery("/response/film/country/string()");
      iRV |= (query.evaluateTo(&result)) ? 0 : -1;
      vidInfo.sCountry = result[0];

      // director ...
      result.clear();
      query.setQuery("/response/film/director/string()");
      iRV |= (query.evaluateTo(&result)) ? 0 : -1;
      vidInfo.sDirector = result[0];

      // image ...
      result.clear();
      query.setQuery("/response/film/poster/string()");
      iRV |= (query.evaluateTo(&result)) ? 0 : -1;
      vidInfo.sImg = result[0];

      // year ...
      result.clear();
      query.setQuery("/response/film/year/string()");
      iRV |= (query.evaluateTo(&result)) ? 0 : -1;
      vidInfo.sYear = result[0];

      // id ...
      result.clear();
      query.setQuery("/response/film/id/string()");
      iRV |= (query.evaluateTo(&result)) ? 0 : -1;
      vidInfo.uiVidId = result[0].toUInt();

      // video id's
      result.clear();
      query.setQuery("/response/film/videos/item/id/string()");
      iRV |= (query.evaluateTo(&result)) ? 0 : -1;

      for (cit = result.constBegin(); cit != result.constEnd(); cit ++)
      {
         vidInfo.vVodFiles.push_back((*cit).toUInt());
      }
   }

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
   cparser::SEpg epg;
   int           iRV;

   // clear epg list ...
   epgList.clear();

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
               epgList.push_back(epg);
            }
            break;

         // any xml element starts ...
         case QXmlStreamReader::StartElement:
            if (xmlSr.name() == "item")
            {
               epg.sDescr = "";
               epg.sName  = "";
               epg.uiGmt  = 0;
            }
            else if (xmlSr.name() == "ut_start")
            {
               // read start time ...
               if (xmlSr.readNext() == QXmlStreamReader::Characters)
               {
                  epg.uiGmt = xmlSr.text().toString().toUInt();
               }
            }
            else if (xmlSr.name() == "progname")
            {
               // read program name ...
               if (xmlSr.readNext() == QXmlStreamReader::Characters)
               {
                  // program description will be after '\n' ...
                  if (rx.indexIn(xmlSr.text().toString()) > -1)
                  {
                     // yes, program description there ...
                     epg.sName  = rx.cap(1);
                     epg.sDescr = rx.cap(2);
                  }
                  else
                  {
                     // program name only ...
                     epg.sName = xmlSr.text().toString();
                  }
               }
            }
            else if (xmlSr.name() == "pdescr")
            {
               // read program description (not used at the moment) ...
               if (xmlSr.readNext() == QXmlStreamReader::Characters)
               {
                  epg.sDescr = xmlSr.text().toString();
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

/*=============================================================================\
|                                    History:
| ---------------------------------------------------------------------------
| 05.Jan.2010 -
\=============================================================================*/

