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
   iTimeShift = 0;
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
   int         i;
   QXmlQuery   xQuery;
   QStringList slGroups;
   QStringList slChannels;
   QStringList slResults, slRates;
   QStringList::const_iterator citGrp, citChn;
   cparser::SChan      chanEntry;
   cparser::STimeShift sTs;

   // clear chanList ...
   chanList.clear();

   // set XML source ...
   xQuery.setFocus(sResp);

   // get groups ...
   xQuery.setQuery(XP_GROUP_IDS);
   xQuery.evaluateTo(&slGroups);

   // add group by group ...
   for (citGrp = slGroups.constBegin(); citGrp != slGroups.constEnd(); citGrp ++)
   {
      // init chan struct ...
      initChanEntry(chanEntry, false);

      // fill group values ...

      // set id ...
      chanEntry.iId = (*citGrp).toInt();

      // get group name ...
      slResults.clear();
      xQuery.setQuery(QString(XP_GROUP_NAME).arg(*citGrp));
      if (xQuery.evaluateTo(&slResults) && (slResults.count() > 0))
      {
         chanEntry.sName = slResults[0];
      }

      // get group color ...
      slResults.clear();
      xQuery.setQuery(QString(XP_GROUP_COLOR).arg(*citGrp));
      if (xQuery.evaluateTo(&slResults) && (slResults.count() > 0))
      {
         chanEntry.sProgramm = slResults[0];
      }

      // group stuff completed -> add to vector ...
      chanList.push_back(chanEntry);

      // get all channels belonging to this group ...
      slChannels.clear();
      xQuery.setQuery(QString(XP_CHAN_IDS).arg(*citGrp));
      xQuery.evaluateTo(&slChannels);

      for (citChn = slChannels.constBegin(); citChn != slChannels.constEnd(); citChn ++)
      {
         // init chan struct ...
         initChanEntry(chanEntry);

         // fill channel values ...

         // set id ...
         chanEntry.iId = (*citChn).toInt();

         // set channel name ...
         slResults.clear();
         xQuery.setQuery(QString(XP_CHAN_NAME).arg(*citGrp).arg(*citChn));
         if (xQuery.evaluateTo(&slResults) && (slResults.count() > 0))
         {
            chanEntry.sName = slResults[0];
         }

         // is video ...
         slResults.clear();
         xQuery.setQuery(QString(XP_CHAN_VIDFLAG).arg(*citGrp).arg(*citChn));
         if (xQuery.evaluateTo(&slResults) && (slResults.count() > 0))
         {
            chanEntry.bIsVideo = slResults[0].toInt() ? true : false;
         }

         // protected flag ...
         slResults.clear();
         xQuery.setQuery(QString(XP_CHAN_PROTFLAG).arg(*citGrp).arg(*citChn));
         if (xQuery.evaluateTo(&slResults) && (slResults.count() > 0))
         {
            chanEntry.bIsProtected = slResults[0].toInt() ? true : false;
         }

         // has archive ...
         slResults.clear();
         xQuery.setQuery(QString(XP_CHAN_ARCHFLAG).arg(*citGrp).arg(*citChn));
         if (xQuery.evaluateTo(&slResults) && (slResults.count() > 0))
         {
            chanEntry.bHasArchive = slResults[0].toInt() ? true : false;
         }

         // icon ...
         slResults.clear();
         xQuery.setQuery(QString(XP_CHAN_ICON).arg(*citGrp).arg(*citChn));
         if (xQuery.evaluateTo(&slResults) && (slResults.count() > 0))
         {
            chanEntry.sIcon = slResults[0];
         }

         // program name ...
         slResults.clear();
         xQuery.setQuery(QString(XP_CHAN_SHOW).arg(*citGrp).arg(*citChn));
         if (xQuery.evaluateTo(&slResults) && (slResults.count() > 0))
         {
            chanEntry.sProgramm = slResults[0];
         }

         // gmt start ...
         slResults.clear();
         xQuery.setQuery(QString(XP_CHAN_START).arg(*citGrp).arg(*citChn));
         if (xQuery.evaluateTo(&slResults) && (slResults.count() > 0))
         {
            chanEntry.uiStart = slResults[0].toUInt();

            if (bFixTime)
            {
               fixTime(chanEntry.uiStart);
            }
         }

         // gmt end ...
         slResults.clear();
         xQuery.setQuery(QString(XP_CHAN_END).arg(*citGrp).arg(*citChn));
         if (xQuery.evaluateTo(&slResults) && (slResults.count() > 0))
         {
            chanEntry.uiEnd = slResults[0].toUInt();

            if (bFixTime)
            {
               fixTime(chanEntry.uiEnd);
            }
         }

         // stream params ...
         slResults.clear();
         slRates.clear();
         xQuery.setQuery(QString(XP_STREAM_RATE).arg(*citGrp).arg(*citChn));
         xQuery.evaluateTo(&slRates);
         xQuery.setQuery(QString(XP_STREAM_TS).arg(*citGrp).arg(*citChn));
         xQuery.evaluateTo(&slResults);

         if (slRates.count() && slResults.count() && (slRates.count() == slResults.count()))
         {
            for (i = 0; i < slResults.count(); i++)
            {
               sTs.iBitRate   = slRates[i].toInt();
               sTs.iTimeShift = slResults[i].toInt();

               // add bitrate ...
               chanEntry.vTs.push_back(sTs);
            }
         }

         // add channel entry to vector ...
         chanList.push_back(chanEntry);
      }
   }

   // no entry means error ...
   return chanList.count() ? 0 : -1;

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
   QString       sErr;
   int           iRV;

   // clear epg list ...
   vSrv.clear();

   // check for errors ...
   iRV = kartinaError(sResp, sErr);

   if (!iRV)
   {
      QXmlStreamReader sr (sResp);

      while(!sr.atEnd() && !sr.hasError())
      {
         switch (sr.readNext())
         {
         // we aren't interested in ...
         case QXmlStreamReader::StartDocument:
         case QXmlStreamReader::EndDocument:
            break;

         // any xml element ends ...
         case QXmlStreamReader::EndElement:
            if (sr.name() == "item")
            {
               vSrv.push_back(srv);
            }
            break;

         // any xml element starts ...
         case QXmlStreamReader::StartElement:
            if (sr.name() == "item")
            {
               srv.sName = "";
               srv.sIp   = "";
            }
            else if (sr.name() == "ip")
            {
               // read srv ip address ...
               if (sr.readNext() == QXmlStreamReader::Characters)
               {
                  srv.sIp = sr.text().toString();
               }
            }
            else if (sr.name() == "descr")
            {
               // read srv name ...
               if (sr.readNext() == QXmlStreamReader::Characters)
               {
                  srv.sName = sr.text().toString();
               }
            }
            else if (sr.name() == "value")
            {
               // read actual srv ip ...
               if (sr.readNext() == QXmlStreamReader::Characters)
               {
                  sActIp = sr.text().toString();
               }
            }
            break;

         default:
            break;

         } // end switch ...

      } // end while ...

      // check for xml errors ...
      if(sr.hasError())
      {
         QMessageBox::critical(NULL, tr("Error in %1").arg(__FUNCTION__),
                               tr("XML Error String: %1").arg(sr.errorString()));

         iRV = -1;
      }
   }
   else
   {
      QMessageBox::critical(NULL, tr("Error"), sErr);
      mErr(sErr);
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
   QString sErr;

   sCookie = "";

   // error check ...
   iRV = kartinaError(sResp, sErr);

   if (!iRV)
   {
      // use reg. expressions instead of xml stream parser ...
      if (rx.indexIn(sResp) > -1)
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
   else
   {
      // display error ...
      QMessageBox::critical(NULL, tr("Error"), sErr);
      mErr(sErr);
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
   int     iRV = 0, iPos = 0;
   QRegExp rx("<div[ \t]+class=\"filter\"[ \t]+rel=\"([0-9]+)\">([^<]+)</div>");
   cparser::SGenre sGenre;

   // clear vector ...
   vGenres.clear();

   // use reg. expressions instead of xml stream parser ...
   while ((iPos = rx.indexIn(sResp, iPos)) > -1)
   {
      sGenre.uiGid  = rx.cap(1).toUInt();
      sGenre.sGName = rx.cap(2);

      // add genre to vector ...
      vGenres.push_back(sGenre);

      // update position ...
      iPos += rx.matchedLength();
   }

   if (vGenres.count() == 0)
   {
      iRV = -1;
      mInfo("No Genres found ...");
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
   cparser::SVodVideo vod;
   QDomDocument  doc;

   // clear vod list ...
   vVodList.clear();

   doc.setContent(sResp);

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

   return vVodList.count() ? 0 : -1;
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
   query.setFocus(sResp);

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
   QString       sErr;
   int           iRV;

   // clear epg list ...
   epgList.clear();

   // check for errors ...
   iRV = kartinaError(sResp, sErr);

   if (!iRV)
   {
      QXmlStreamReader sr (sResp);

      while(!sr.atEnd() && !sr.hasError())
      {
         switch (sr.readNext())
         {
         // we aren't interested in ...
         case QXmlStreamReader::StartDocument:
         case QXmlStreamReader::EndDocument:
            break;

         // any xml element ends ...
         case QXmlStreamReader::EndElement:
            if (sr.name() == "item")
            {
               epgList.push_back(epg);
            }
            break;

         // any xml element starts ...
         case QXmlStreamReader::StartElement:
            if (sr.name() == "item")
            {
               epg.sDescr = "";
               epg.sName  = "";
               epg.uiGmt  = 0;
            }
            else if (sr.name() == "ut_start")
            {
               // read start time ...
               if (sr.readNext() == QXmlStreamReader::Characters)
               {
                  epg.uiGmt = sr.text().toString().toUInt();
               }
            }
            else if (sr.name() == "progname")
            {
               // read program name ...
               if (sr.readNext() == QXmlStreamReader::Characters)
               {
                  // program description will be after '\n' ...
                  if (rx.indexIn(sr.text().toString()) > -1)
                  {
                     // yes, program description there ...
                     epg.sName  = rx.cap(1);
                     epg.sDescr = rx.cap(2);
                  }
                  else
                  {
                     // program name only ...
                     epg.sName = sr.text().toString();
                  }
               }
            }
            else if (sr.name() == "pdescr")
            {
               // read program description (not used at the moment) ...
               if (sr.readNext() == QXmlStreamReader::Characters)
               {
                  epg.sDescr = sr.text().toString();
               }
            }
            break;

         default:
            break;

         } // end switch ...

      } // end while ...

      // check for xml errors ...
      if(sr.hasError())
      {
         QMessageBox::critical(NULL, tr("Error in %1").arg(__FUNCTION__),
                               tr("XML Error String: %1").arg(sr.errorString()));

         iRV = -1;
      }
   }
   else
   {
      QMessageBox::critical(NULL, tr("Error"), sErr);
      mErr(sErr);
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
   QString sErr;
   int     iRV;

   // clear epg list ...
   vValues.clear();

   // check for errors ...
   iRV = kartinaError(sResp, sErr);

   if (!iRV)
   {
      QXmlStreamReader sr (sResp);

      while(!sr.atEnd() && !sr.hasError())
      {
         switch (sr.readNext())
         {
         // we aren't interested in ...
         case QXmlStreamReader::StartDocument:
         case QXmlStreamReader::EndDocument:
         case QXmlStreamReader::EndElement:
            break;

         // any xml element starts ...
         case QXmlStreamReader::StartElement:
            if (sr.name() == "item")
            {
               // read item ...
               if (sr.readNext() == QXmlStreamReader::Characters)
               {
                  vValues.push_back(sr.text().toString().toInt());
               }
            }
            else if (sr.name() == "value")
            {
               // read actual value ...
               if (sr.readNext() == QXmlStreamReader::Characters)
               {
                  iActVal = sr.text().toString().toInt();
               }
            }
            else if (sr.name() == "name")
            {
               // read actual value ...
               if (sr.readNext() == QXmlStreamReader::Characters)
               {
                  sName = sr.text().toString();
               }
            }
            break;

         default:
            break;

         } // end switch ...

         // store timeshift value if needed ...
         if (sName == "timeshift")
         {
            iTimeShift = iActVal;
         }

      } // end while ...

      // check for xml errors ...
      if(sr.hasError())
      {
         QMessageBox::critical(NULL, tr("Error in %1").arg(__FUNCTION__),
                               tr("XML Error String: %1").arg(sr.errorString()));

         iRV = -1;
      }
   }
   else
   {
      QMessageBox::critical(NULL, tr("Error"), sErr);
      mErr(sErr);
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
   QString sErr;

   sUrl = "";

   // error check ...
   iRV = kartinaError(sResp, sErr);

   if (!iRV)
   {
      // use reg. expressions instead of xml stream parser ...
      if (rx.indexIn(sResp) > -1)
      {
         sUrl = rx.cap(1);
      }
   }
   else
   {
      QMessageBox::critical(NULL, tr("Error"), sErr);
      mErr(sErr);
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: kartinaError
|  Begin: 28.07.2010 / 18:42:54
|  Author: Jo2003
|  Description: format kartina error string
|
|  Parameters: --
|
|  Returns: error string
\----------------------------------------------------------------- */
int CKartinaXMLParser::kartinaError(const QString& sResp, QString &sErr)
{
   int iRV = 0;
   QRegExp rx("<message>(.*)</message>[ \t\n\r]*"
              "<code>(.*)</code>");

   sErr = "";

   // quick'n'dirty error check ...
   if (sResp.contains("<error>"))
   {
      if (rx.indexIn(sResp) > -1)
      {
         sErr = rx.cap(1);
         iRV  = rx.cap(2).toInt();
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

