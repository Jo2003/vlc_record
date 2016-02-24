/*=============================================================================\
| $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/COtradaParser.cpp $
|
| Author: Jo2003
|
| last changed by: $Author: Olenka.Joerg $
|
| Begin: 19.03.2013
|
| $Id: COtradaParser.cpp 1299 2014-02-12 14:00:23Z Olenka.Joerg $
|
\=============================================================================*/
#include "cotradaparser.h"
#include <stdint.h>

#define DEF_OPT_ARCH (1 << 0)   /// 0000 0001
#define DEF_OPT_TV   (1 << 7)   /// 1000 0000
#define DEF_OPT_RAD  (1 << 6)   /// 0100 0000
#define DEF_OPT_VOD  (1 << 5)   /// 0010 0000

// log file functions ...
extern CLogFile VlcLog;

/* -----------------------------------------------------------------\
|  Method: COtradaParser / constructor
|  Begin: 19.01.2010 / 15:27:01
|  Author: Jo2003
|  Description: construct object and init values
|
|  Parameters: data to parse, pointer to parent object
|
|  Returns: --
\----------------------------------------------------------------- */
COtradaParser::COtradaParser(QObject * parent) : CApiXmlParser(parent)
{
   // nothing to do here ...
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
int COtradaParser::parseChannelList (const QString &sResp,
                                     QVector<cparser::SChan> &chanList,
                                     bool bFixTime)
{
   int              iRV = 0;
   QXmlStreamReader xml;

   // clear channel list ...
   chanList.clear();

   sImgTmplTv    = xmlElementToValue(sResp, "tv");
   sImgTmplRadio = xmlElementToValue(sResp, "radio");

   // fall back variant ...
   if (sImgTmplTv == "")
   {
      sImgTmplTv    = xmlElementToValue(sResp, "default");
   }

   xml.addData(sResp);

   while(!xml.atEnd() && !xml.hasError())
   {
      switch (xml.readNext())
      {
      // any xml element starts ...
      case QXmlStreamReader::StartElement:
         if ((xml.name() == "groups_tv") || (xml.name() == "groups_radio"))
         {
            // go into next level and parse groups ...
            parseGroups(xml, chanList, xml.name().toString(), bFixTime);
         }
         break;

      default:
         break;

      } // end switch ...

   } // end while ...

   // check for xml errors ...
   if(xml.hasError())
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("XML Error String: %1").arg(xml.errorString()));

      iRV = -1;
   }

   return iRV;
}

/* -----------------------------------------------------------------\
|  Method: parseGroups
|  Begin: 14.01.2011 / 16:50
|  Author: Jo2003
|  Description: parse group part of channel list
|
|  Parameters: ref. to xml parser, ref. to chanList, , end tag, fixTime flag
|
|  Returns: 0
\----------------------------------------------------------------- */
int COtradaParser::parseGroups (QXmlStreamReader &xml, QVector<cparser::SChan> &chanList,
                                const QString &endTag,
                                bool bFixTime)
{
   QString        sUnknown;
   cparser::SChan groupEntry;
   int            idx      = 0;
   bool           isRadio  = (endTag == "groups_radio") ? true : false;

   // while not end groups ...
   while (!((xml.readNext() == QXmlStreamReader::EndElement)
      && (xml.name() == endTag)))
   {
      if (xml.tokenType() == QXmlStreamReader::StartElement)
      {
         // item start --> initialize struct ...
         if (xml.name() == "item")
         {
            initChanEntry(groupEntry, false);
         }
         else if (xml.name() == "name") // this is the default ...
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               groupEntry.sName = xml.text().toString();
            }
         }
         else if (xml.name() == "user_title") // here we might overwrite the default name ...
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

               if (isRadio)
               {
                   groupEntry.iId |= RADIO_OFFSET;
               }
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
            // ignore group ... ?
            if (!ignoreGroup(groupEntry))
            {
               // make sure color isn't black ...
               checkColor(groupEntry.sProgramm, idx++);

               // store group entry ...
               chanList.push_back(groupEntry);

               // go into next level (channels)
               parseChannels(xml, chanList, isRadio, bFixTime);
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
|  Method: parseChannels
|  Begin: 14.01.2011 / 16:50
|  Author: Jo2003
|  Description: parse channels part of channel list
|
|  Parameters: ref. to xml parser, ref. to chanList, radio flag,
|              fixTime flag
|
|  Returns: 0
\----------------------------------------------------------------- */
int COtradaParser::parseChannels(QXmlStreamReader &xml, QVector<cparser::SChan> &chanList,
                                 bool isRadio, bool bFixTime)
{
   QString                sUnknown;
   cparser::SChan         chanEntry;
   QStringList            slNeeded;
   QMap<QString, QString> mResults;
   QStringList            slATracks;

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

               if (isRadio)
               {
                  // make radio cid unique  ...
                  chanEntry.iId |= RADIO_OFFSET;
               }
            }
         }
         else if (xml.name() == "is_video")
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               chanEntry.bIsVideo = (xml.text().toString().toInt() == 1) ? true : false;
            }
         }
         else if (xml.name() == "has_archive")
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               chanEntry.bHasArchive = (xml.text().toString().toInt() == 1) ? true : false;
            }
         }
         else if (xml.name() == "time_shift")
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               chanEntry.bHasTsInfo = true;
               chanEntry.iTs        = xml.text().toString().toInt();
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
               chanEntry.sIcon = chanEntry.bIsVideo ? sImgTmplTv : sImgTmplRadio;
               chanEntry.sIcon.replace("%ICON%", xml.text().toString());
            }
         }
         else if (xml.name() == "epg")
         {
            // known ...
         }
         else if (xml.name() == "current")
         {
            slNeeded.clear();
            mResults.clear();

            slNeeded << "title" << "info" << "begin" << "end";

            oneLevelParser(xml, "current", slNeeded, mResults);

            chanEntry.sProgramm = QString("%1\n%2").arg(mResults.value("title")).arg(mResults.value("info"));
            chanEntry.uiStart   = mResults.value("begin").toUInt();
            chanEntry.uiEnd     = mResults.value("end").toUInt();

            if (bFixTime)
            {
               fixTime(chanEntry.uiStart);
               fixTime(chanEntry.uiEnd);
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
         else if (xml.name() == "audiotracks")
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               slATracks = xml.text().toString().split(QChar(','), QString::SkipEmptyParts);
            }
         }
         else if (xml.name() == "audiotrack_default")
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               chanEntry.sLangCode = xml.text().toString();
               uint idx = (uint)slATracks.indexOf(chanEntry.sLangCode);
               chanEntry.uiDefAud = (idx == (uint)-1) ? 0 : idx;
            }
         }
         else if (xml.name() == "group_id")
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
                // string contains a comma separated list
                // of groups where the channel should appear ...
                QString sInGrp = xml.text().toString();
                QStringList slInGrp = sInGrp.split(QChar(','), QString::SkipEmptyParts);

                foreach (const QString& sGrp, slInGrp)
                {
                    chanEntry.vInGroups.append(sGrp.toInt());
                }
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
int COtradaParser::parseStreamParams (QXmlStreamReader &xml, QVector<cparser::STimeShift>& vTs)
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
int COtradaParser::parseSServersLogin(const QString &sResp, QVector<cparser::SSrv> &vSrv,
                                     QString &sActIp)
{
   cparser::SSrv    srv;
   int              iRV      = 0;
   QXmlStreamReader xml;
   bool             bStarted = false;
   bool             bAtEnd   = false;

   // clear epg list ...
   vSrv.clear();

   sActIp = xmlElementToValue(sResp, "media_server_id");

   xml.addData(sResp);

   while(!xml.atEnd() && !xml.hasError() && !bAtEnd)
   {
      switch (xml.readNext())
      {
      // we aren't interested in ...
      case QXmlStreamReader::StartDocument:
      case QXmlStreamReader::EndDocument:
         break;

      // any xml element ends ...
      case QXmlStreamReader::EndElement:
         if (xml.name() == "media_servers")
         {
            bStarted = false;
            bAtEnd   = true;
         }

         if ((xml.name() == "item") && bStarted)
         {
            vSrv.push_back(srv);
         }
         break;

      // any xml element starts ...
      case QXmlStreamReader::StartElement:
         if (xml.name() == "media_servers")
         {
            bStarted = true;
         }

         if (bStarted)
         {
            if (xml.name() == "item")
            {
               srv.sName = "";
               srv.sIp   = "";
            }
            else if (xml.name() == "id")
            {
               // read srv ip address ...
               if (xml.readNext() == QXmlStreamReader::Characters)
               {
                  srv.sIp = xml.text().toString();
               }
            }
            else if (xml.name() == "title")
            {
               // read srv name ...
               if (xml.readNext() == QXmlStreamReader::Characters)
               {
                  srv.sName = xml.text().toString();
               }
            }
         }
         break;

      default:
         break;

      } // end switch ...

   } // end while ...

   // check for xml errors ...
   if(xml.hasError())
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("XML Error String: %1").arg(xml.errorString()));

      iRV = -1;
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
int COtradaParser::parseCookie (const QString &sResp, QString &sCookie, cparser::SAccountInfo &sInf)
{
   int                    iRV = 0;
   QXmlStreamReader       xml;
   QString                sSid, sSidName = "sid";
   QStringList            slNeeded;
   QMap<QString, QString> mResults;
   uint8_t                opts;
   sCookie = "";

   // so far we have no info about this -> fake!
   sInf.bHasArchive = true;
   sInf.bHasVOD     = false;

   xml.addData(sResp);

   while(!xml.atEnd() && !xml.hasError())
   {
      switch (xml.readNext())
      {
      // any xml element starts ...
      case QXmlStreamReader::StartElement:
         if (xml.name() == "sid")
         {
            // read sid ...
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               sSid = xml.text().toString();
            }
         }
         else if (xml.name() == "sid_name")
         {
            // read sid_name ...
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               sSidName = xml.text().toString();
            }
         }
         else if (xml.name() == "subscriptions")
         {
            mResults.clear();
            slNeeded.clear();

            slNeeded << "title" << "end_date" << "option";

            // read next start tag "item" ...
            xml.readNext();

            // get expires value ...
            oneLevelParser(xml, "item", slNeeded, mResults);

            // format into string ...
            sInf.sExpires  = mResults.value("end_date");

            /// Fix-Me! this might not work ... I haven't the possibility to check this right now!
            sInf.dtExpires = QDateTime::fromString(sInf.sExpires);

            opts = mResults.value("option").toLong(0, 2);

            sInf.bHasVOD     = !!(opts & DEF_OPT_VOD);
            sInf.bHasArchive = !!(opts & DEF_OPT_ARCH);
#ifdef __TRACE
            mInfo(tr("Options: %1 (TV: %2, RADIO: %3, VOD: %4, ARCHIVE: %5)")
                  .arg(QString::number((uint)opts, 2))
                  .arg(!!(opts & DEF_OPT_TV))
                  .arg(!!(opts & DEF_OPT_RAD))
                  .arg(!!(opts & DEF_OPT_VOD))
                  .arg(!!(opts & DEF_OPT_ARCH)));
#endif // __TRACE
         }
         else if (xml.name() == "services")
         {
            mResults.clear();
            slNeeded.clear();
            slNeeded << "vod" << "archive";

            // get values ...
            oneLevelParser(xml, "services", slNeeded, mResults);

            sInf.bHasVOD     = mResults.value("vod").toInt() ? true : false;
            sInf.bHasArchive = mResults.value("archive").toInt() ? true : false;
         }
         else if (xml.name() == "servertime")
         {
            // read server time ...
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               // check offset ...
               checkTimeOffSet (xml.text().toString().toUInt());
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
   if(xml.hasError())
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("XML Error String: %1").arg(xml.errorString()));

      iRV = -1;
   }

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
int COtradaParser::parseGenres (const QString& sResp, QVector<cparser::SGenre>& vGenres)
{
   int              iRV = 0;
   QXmlStreamReader xml;
   cparser::SGenre  sGenre;
   vGenres.clear ();

   xml.addData(sResp);

   while(!xml.atEnd() && !xml.hasError())
   {
      switch (xml.readNext())
      {
      // any xml element starts ...
      case QXmlStreamReader::StartElement:
         if (xml.name() == "id")
         {
            // read id ...
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               sGenre.uiGid = xml.text().toString().toUInt();
            }
         }
         else if (xml.name() == "title")
         {
            // read name ...
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               sGenre.sGName = xml.text().toString();

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
   if(xml.hasError())
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("XML Error String: %1").arg(xml.errorString()));

      iRV = -1;
   }

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
int COtradaParser::parseEpgCurrent (const QString& sResp, QCurrentMap &currentEpg)
{
   int                           iRV =  0;
   QXmlStreamReader              xml;
   int                           cid = -1;
   QStringList                   slNeeded;
   QMap<QString, QString>        mResults;
   cparser::SEpgCurrent          entry;
   QVector<cparser::SEpgCurrent> chanEntries;

   currentEpg.clear();

   slNeeded << "title" << "info" << "begin" << "end";

   xml.addData(sResp);

   while(!xml.atEnd() && !xml.hasError())
   {
      switch (xml.readNext())
      {
      // any xml element starts ...
      case QXmlStreamReader::StartElement:
         if (xml.name() == "id")
         {
            // read cid ...
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               cid = xml.text().toString().toInt();
            }
         }
         else if((xml.name() == "current") && (cid != -1))
         {
            // read epg entry ...
            oneLevelParser(xml, "current", slNeeded, mResults);

            entry.sShow   = QString("%1\n%2").arg(mResults.value("title")).arg(mResults.value("info"));
            entry.uiStart = mResults.value("begin").toUInt();
            entry.uiEnd   = mResults.value("end").toUInt();

            chanEntries.append(entry);
         }
         else if((xml.name() == "next") && (cid != -1))
         {
            // read epg entry ...
            oneLevelParser(xml, "next", slNeeded, mResults);

            entry.sShow   = QString("%1\n%2").arg(mResults.value("title")).arg(mResults.value("info"));
            entry.uiStart = mResults.value("begin").toUInt();
            entry.uiEnd   = mResults.value("end").toUInt();

            chanEntries.append(entry);
         }
         break;

      case QXmlStreamReader::EndElement:
         if ((xml.name() == "item") && (cid != -1))
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
   if(xml.hasError())
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("XML Error String: %1").arg(xml.errorString()));

      iRV = -1;
   }

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
int COtradaParser::parseVodManager (const QString &sResp, QVector<cparser::SVodRate> &vRates)
{
   int               iRV = 0;
   QXmlStreamReader  xml;
   cparser::SVodRate sRate;
   vRates.clear ();

   xml.addData(sResp);

   while(!xml.atEnd() && !xml.hasError())
   {
      switch (xml.readNext())
      {
      // any xml element starts ...
      case QXmlStreamReader::StartElement:
         if (xml.name() == "item")
         {
            sRate.iRateID = -1;
            sRate.sAccess = "";
            sRate.sGenre  = "";
         }
         else if (xml.name() == "id_rate")
         {
            // read id ...
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               sRate.iRateID = xml.text().toString().toUInt();
            }
         }
         else if (xml.name() == "rate_name")
         {
            // read genre name  ...
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               sRate.sGenre = xml.text().toString();
            }
         }
         else if (xml.name() == "action")
         {
            // read access  ...
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               sRate.sAccess = xml.text().toString();
            }
         }
         break;

      // any xml element ends ...
      case QXmlStreamReader::EndElement:
         if (xml.name() == "item")
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
   if(xml.hasError())
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("XML Error String: %1").arg(xml.errorString()));

      iRV = -1;
   }

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
int COtradaParser::parseVodList(const QString &sResp, QVector<cparser::SVodVideo> &vVodList, cparser::SGenreInfo &gInfo)
{
   int                    iRV = 0;
   QXmlStreamReader       xml;
   QStringList            slNeeded;
   QMap<QString, QString> mResults;
   cparser::SVodVideo     vod;

   // clear vod list ...
   vVodList.clear();

   xml.addData(sResp);

   while(!xml.atEnd() && !xml.hasError())
   {
      switch (xml.readNext())
      {
      // any xml element starts ...
      case QXmlStreamReader::StartElement:
         if (xml.name() == "options")
         {
            mResults.clear();
            slNeeded.clear();

            // we need following data ...
            slNeeded << "count" << "page";

            oneLevelParser(xml, "page", slNeeded, mResults);

            gInfo.sType  = "";
            gInfo.iCount = mResults.value("count").toInt();
            gInfo.iPage  = mResults.value("page").toInt();
            gInfo.iTotal = mResults.value("count").toInt();

            mInfo(tr("Got Type: %1, Count: %2, Page: %3, Total: %4")
                  .arg(gInfo.sType)
                  .arg(gInfo.iCount)
                  .arg(gInfo.iPage)
                  .arg(gInfo.iTotal));
         }
         else if (xml.name() == "item")
         {
            mResults.clear();
            slNeeded.clear();

            // we need following data ...
            slNeeded << "id" << "title" << "protected" << "pic" << "is_favorite";

            oneLevelParser(xml, "item", slNeeded, mResults);

            vod.uiVidId    =   mResults.value("id").toUInt();
            vod.sName      =   mResults.value("title");
            vod.sDescr     =   "";
            vod.sYear      =   "";
            vod.sCountry   =   "";
            vod.sImg       =   mResults.value("pic");
            vod.bProtected = !!mResults.value("protected").toInt();
            vod.bFavourit  = !!mResults.value("is_favorite").toInt();

            // store element ...
            vVodList.push_back(vod);
         }
         break;

      default:
         break;

      } // end switch ...

   } // end while ...

   // check for xml errors ...
   if(xml.hasError())
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("XML Error String: %1").arg(xml.errorString()));

      iRV = -1;
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
int COtradaParser::parseVideoInfo(const QString &sResp, cparser::SVodVideo &vidInfo)
{
   int                    iRV = 0;
   QXmlStreamReader       xml;
   QStringList            slNeeded;
   QMap<QString, QString> mResults;
   cparser::SVodFileInfo  fInfo;
   bool                   bEnd = false;

   xml.addData(sResp);

   while(!xml.atEnd() && !xml.hasError() && !bEnd)
   {
      switch (xml.readNext())
      {
      // any xml element starts ...
      case QXmlStreamReader::StartElement:
         if (xml.name() == "item")
         {
            mResults.clear();
            slNeeded.clear();

            slNeeded << "title" << "time" << "description" << "acters"
                     << "country" << "director" << "pic" << "year"
                     << "id" << "genre" << "protected" << "is_favorite";

            oneLevelParser(xml, "vis", slNeeded, mResults);

            vidInfo.sActors     = mResults.value("acters");
            vidInfo.sCountry    = mResults.value("country");
            vidInfo.sDescr      = mResults.value("description");
            vidInfo.sDirector   = mResults.value("director");
            vidInfo.sImg        = mResults.value("pic");
            vidInfo.sName       = mResults.value("title");
            vidInfo.sYear       = mResults.value("year");
            vidInfo.sGenres     = mResults.value("genre");
            vidInfo.uiLength    = mResults.value("time").toUInt() / 60;
            vidInfo.uiLengthSec = mResults.value("time").toUInt();
            vidInfo.uiVidId     = mResults.value("id").toUInt();
            vidInfo.bProtected  = !!mResults.value("protected").toInt();
            vidInfo.bFavourit   = !!mResults.value("is_favorite").toInt();

            vidInfo.sActors.replace("\r", "");
            vidInfo.sActors.replace("\n", ", ");
            vidInfo.sActors = vidInfo.sActors.simplified();
         }
         else if (xml.name() == "videos")
         {
            slNeeded.clear();

            slNeeded << "id" << "title" << "format" << "url"
                     << "size" << "length" << "codec" << "width"
                     << "height";

            while ((xml.readNext() == QXmlStreamReader::StartElement)
                && (xml.name() == "item")
                && !xml.atEnd() && !xml.hasError())
            {
               mResults.clear();

               // parse vod parts ...
               oneLevelParser(xml, "item", slNeeded, mResults);

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
         }
         else if (xml.name() == "favorite")
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               vidInfo.bFavourit = !!xml.text().toString().toInt();
            }
         }
         break;

      case QXmlStreamReader::EndElement:
         // end of videos means end of needed info ...
         if (xml.name() == "item")
         {
            bEnd = true;
         }
         break;

      default:
         break;
      }
   }

   /// hack for missing videos ...
   if (vidInfo.vVodFiles.isEmpty())
   {
       fInfo.iHeight = 0;
       fInfo.iId     = vidInfo.uiVidId;
       fInfo.iLength = vidInfo.uiLengthSec;
       fInfo.iSize   = 0;
       fInfo.iWidth  = 0;
       fInfo.sCodec  = tr("h264");
       fInfo.sFormat = tr("VOD");
       fInfo.sTitle  = tr("Video");
       fInfo.sUrl    = "";

       vidInfo.vVodFiles.push_back(fInfo);
   }

   // check for xml errors ...
   if(xml.hasError())
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("XML Error String: %1").arg(xml.errorString()));

      iRV = -1;
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
int COtradaParser::parseEpg (const QString &sResp, QVector<cparser::SEpg> &epgList)
{
   QXmlStreamReader       xml;
   QRegExp                rx("([^\n]*)[\n]*(.*)");
   QMap<QString, QString> mResults;
   QStringList            slNeeded;
   cparser::SEpg          epg;
   bool                   bStarted = false;
   int                    iRV = 0;

   // clear epg list ...
   epgList.clear();

   xml.addData(sResp);

   slNeeded << "title" << "info" << "begin" << "end";

   while(!xml.atEnd() && !xml.hasError())
   {
      switch (xml.readNext())
      {
      case QXmlStreamReader::StartElement:

         if (xml.name() == "epg")
         {
            bStarted = true;
         }
         else if (xml.name() == "item")
         {
            if (bStarted)
            {
               epg.sDescr = "";

               oneLevelParser(xml, "item", slNeeded, mResults);

               epg.uiGmt = mResults.value("begin").toUInt();
               epg.uiEnd = mResults.value("end").toUInt();

               // program description will be after '\n' ...
               if (rx.indexIn(mResults.value("title")) > -1)
               {
                  // yes, program description there ...
                  epg.sName  = rx.cap(1);
                  epg.sDescr = rx.cap(2);
               }
               else
               {
                  // program name only ...
                  epg.sName = mResults.value("title");
               }

               // is there a description ... ?
               if (mResults.value("info") != "")
               {
                  epg.sDescr = mResults.value("info");
               }

               // store element ...
               epgList.push_back(epg);
            }
         }
         break;

      default:
         break;

      } // end switch ...

   } // end while ...

   // check for xml errors ...
   if(xml.hasError())
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("XML Error String: %1").arg(xml.errorString()));

      iRV = -1;
   }

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
int COtradaParser::parseSetting(const QString& sResp, const QString &sName, QVector<int>& vValues, int& iActVal)
{
   int              iRV      = 0;
   QXmlStreamReader xml;
   bool             bAtEnd   = false;
   bool             bStarted = false;

   // adaption for rodnoe ...
   if (sName == "timeshift")
   {
      for (int i = 0; i <= 24; i++)
      {
         vValues.append(i);
      }

      iActVal = xmlElementToValue(sResp, "time_shift").toInt();

      return 0;
   }

   // clear epg list ...
   vValues.clear();

   xml.addData(sResp);

   while(!xml.atEnd() && !xml.hasError() && !bAtEnd)
   {
      switch (xml.readNext())
      {
      // we aren't interested in ...
      case QXmlStreamReader::StartDocument:
      case QXmlStreamReader::EndDocument:
         break;

      // any xml element starts ...
      case QXmlStreamReader::StartElement:
         if (xml.name() == sName)
         {
            bStarted = true;
         }

         if (bStarted)
         {
            if (xml.name() == "item")
            {
               // read item ...
               if (xml.readNext() == QXmlStreamReader::Characters)
               {
                  vValues.push_back(xml.text().toString().toInt());
               }
            }
            else if (xml.name() == "value")
            {
               // read actual value ...
               if (xml.readNext() == QXmlStreamReader::Characters)
               {
                  iActVal = xml.text().toString().toInt();
               }
            }
         }
         break;

      case QXmlStreamReader::EndElement:
         if (xml.name() == sName)
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
   if(xml.hasError())
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("XML Error String: %1").arg(xml.errorString()));

      iRV = -1;
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
int COtradaParser::parseUrl(const QString &sResp, QString &sUrl)
{
   QRegExp rx("<url>([^<]*).*");

   int     iRV = 0;

   sUrl = "";

   // use reg. expressions instead of xml stream parser ...
   if (rx.indexIn(sResp) > -1)
   {
      sUrl = rx.cap(1);
      sUrl.replace("&amp;", "&");

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
int COtradaParser::parseVodUrls (const QString& sResp, QStringList& sUrls)
{
   int              iRV = 0;
   QXmlStreamReader xml;
   QString          sUrl, sAdUrl;

   // clear string list ...
   sUrls.clear();

   xml.addData(sResp);

   while(!xml.atEnd() && !xml.hasError())
   {
      switch (xml.readNext())
      {
      // we aren't interested in ...
      case QXmlStreamReader::StartDocument:
      case QXmlStreamReader::EndDocument:
      case QXmlStreamReader::EndElement:
         break;

      // any xml element starts ...
      case QXmlStreamReader::StartElement:
         if (xml.name() == "url")
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               sUrl = xml.text().toString();
               if (sUrl.contains(QChar(' ')))
               {
                  sUrl = sUrl.left(sUrl.indexOf(QChar(' ')));
               }
            }
         }
         else if (xml.name() == "ad_url")
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               sAdUrl = xml.text().toString();
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
   if(xml.hasError())
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("XML Error String: %1").arg(xml.errorString()));

      iRV = -1;
   }

   return iRV;
}

//---------------------------------------------------------------------------
//
//! \brief   parse audio stream response
//
//! \author  Jo2003
//! \date    29.07.2013
//
//! \param   sResp (const QString&) ref. to response string
//! \param   sl (QStringList&) save audio codes here
//
//! \return  --
//---------------------------------------------------------------------------
int COtradaParser::parseAStreams(const QString &sResp, QStringList &sl)
{
   int              iRV = 0;
   QXmlStreamReader xml;

   sl.clear();

   xml.addData(sResp);

   while(!xml.atEnd() && !xml.hasError())
   {
      switch (xml.readNext())
      {
      // we aren't interested in ...
      case QXmlStreamReader::StartDocument:
      case QXmlStreamReader::EndDocument:
      case QXmlStreamReader::EndElement:
         break;

      // any xml element starts ...
      case QXmlStreamReader::StartElement:
         if (xml.name() == "title")
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               sl << xml.text().toString();
            }
         }
         break;

      default:
         break;

      } // end switch ...

   } // end while ...

   // check for xml errors ...
   if(xml.hasError())
   {
      emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                    tr("XML Error String: %1").arg(xml.errorString()));

      iRV = -1;
   }

   return iRV;
}

/*=============================================================================\
|                                    History:
| ---------------------------------------------------------------------------
| 05.Jan.2010 -
\=============================================================================*/

