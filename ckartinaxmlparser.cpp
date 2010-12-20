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
|  Method: parseChannelList
|  Begin: 29.07.2010 / 11:28:20
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
   cparser::SChan chan;
   QString        sErr;
   bool           bInChannels = false;
   int            iRV;

   // clear chanList ...
   chanList.clear();

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
            if (sr.name() == "channels")
            {
               bInChannels = false;
            }
            else if (sr.name() == "item")
            {
               if (bInChannels)
               {
                  // channel item ends ...
                  chanList.push_back(chan);
               }
            }
            break;

         // any xml element starts ...
         case QXmlStreamReader::StartElement:
            if (sr.name() == "channels")
            {
               // we're in channels part now ...
               bInChannels  = true;

               // this also means group stuff is completed ...
               chan.bIsGroup = true;
               chanList.push_back(chan);
            }
            else if (sr.name() == "item")
            {
               // new item starts --> init chan struct ...
               chan.bHasArchive  = false;
               chan.bIsGroup     = false;
               chan.bIsProtected = false;
               chan.bIsVideo     = false;
               chan.iId          = -1;
               chan.sIcon        = "";
               chan.sName        = "";
               chan.sProgramm    = "";
               chan.uiEnd        = 0;
               chan.uiStart      = 0;
            }
            else if (sr.name() == "id")
            {
               // read id ...
               if (sr.readNext() == QXmlStreamReader::Characters)
               {
                  chan.iId = sr.text().toString().toInt();
               }
            }
            else if (sr.name() == "name")
            {
               // read name ...
               if (sr.readNext() == QXmlStreamReader::Characters)
               {
                  chan.sName = sr.text().toString();
               }
            }
            else if (sr.name() == "color")
            {
               // read color ...
               if (sr.readNext() == QXmlStreamReader::Characters)
               {
                  chan.sProgramm = sr.text().toString();
               }
            }
            else if (sr.name() == "is_video")
            {
               // is video ...
               if (sr.readNext() == QXmlStreamReader::Characters)
               {
                  chan.bIsVideo = (sr.text().toString().toInt()) ? true : false;
               }
            }
            else if (sr.name() == "protected")
            {
               // is protected ...
               if (sr.readNext() == QXmlStreamReader::Characters)
               {
                  chan.bIsProtected = (sr.text().toString().toInt()) ? true : false;
               }
            }
            else if (sr.name() == "have_archive")
            {
               // has archive ...
               if (sr.readNext() == QXmlStreamReader::Characters)
               {
                  chan.bHasArchive = (sr.text().toString().toInt()) ? true : false;
               }
            }
            else if (sr.name() == "icon")
            {
               // read icon path ...
               if (sr.readNext() == QXmlStreamReader::Characters)
               {
                  chan.sIcon = sr.text().toString();
               }
            }
            else if (sr.name() == "epg_progname")
            {
               // read show name ...
               if (sr.readNext() == QXmlStreamReader::Characters)
               {
                  chan.sProgramm = sr.text().toString();
               }
            }
            else if (sr.name() == "epg_start")
            {
               // read show start ...
               if (sr.readNext() == QXmlStreamReader::Characters)
               {
                  chan.uiStart = sr.text().toString().toUInt();

                  if (bFixTime)
                  {
                     fixTime(chan.uiStart);
                  }
               }
            }
            else if (sr.name() == "epg_end")
            {
               // read show end ...
               if (sr.readNext() == QXmlStreamReader::Characters)
               {
                  chan.uiEnd = sr.text().toString().toUInt();

                  if (bFixTime)
                  {
                     fixTime(chan.uiEnd);
                  }
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
      vod.uiVidId  = dlIds.item(i).nodeValue().toUInt();
      vod.sName    = dlNames.item(i).nodeValue();
      vod.sDescr   = dlDescs.item(i).nodeValue();
      vod.sYear    = dlYear.item(i).nodeValue();
      vod.sCountry = dlCountry.item(i).nodeValue();
      vod.sImg     = dlImg.item(i).nodeValue();

      vVodList.push_back(vod);
   }

   return vVodList.count() ? 0 : -1;
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

/*=============================================================================\
|                                    History:
| ---------------------------------------------------------------------------
| 05.Jan.2010 -
\=============================================================================*/

