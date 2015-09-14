/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     capiparser.cpp
 *
 *  @author   Jo2003
 *
 *  @date     17.04.2013
 *
 *  $Id$
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#include "capiparser.h"
#include <QColor>
#include "small_helpers.h"
#include "qdatetimesyncro.h"
#include "externals_inc.h"

//---------------------------------------------------------------------------
//
//! \brief   construct api parser
//
//! \author  Jo2003
//! \date    17.04.2013
//
//! \param   parent pointer to parent object
//
//! \return  --
//---------------------------------------------------------------------------
CApiParser::CApiParser(QObject * parent) : QObject(parent)
{
   // define some nice colors ...
   slAltColors << "#ef0000" << "#00ef00" << "#0000ef"
               << "#efef00" << "#ef00ef" << "#00efef"
               << "#ef9933" << "#99ef33" << "#9933ef"
               << "#ef3399" << "#33ef99" << "#3399ef"
               << "#cc4444" << "#44cc44" << "#4444cc"
               << "#cccc44" << "#cc44cc" << "#44cccc"
               << "#cccccc" << "#999999";
}

//---------------------------------------------------------------------------
//
//! \brief   destroy api parser
//
//! \author  Jo2003
//! \date    17.04.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
CApiParser::~CApiParser()
{
   // nothing to do so far ...
}

//---------------------------------------------------------------------------
//
//! \brief   check time offset between server and local time
//
//! \author  Jo2003
//! \date    15.04.2013
//
//! \param   uiSrvTime (const uint &) ref. to server time
//
//! \return  --
//---------------------------------------------------------------------------
void CApiParser::checkTimeOffSet (const uint &uiSrvTime)
{
   // get difference between kartina.tv and our time ...
   int iOffSec    = (int)(QDateTime::currentDateTime().toTime_t() - uiSrvTime);

   if (abs(iOffSec) > 10)
   {
      tmSync.setOffset(iOffSec);
      mInfo(tr("Set time offset to %1 seconds!").arg(iOffSec));
   }
}

//---------------------------------------------------------------------------
//
//! \brief   init channel entry
//
//! \author  Jo2003
//! \date    15.04.2013
//
//! \param   entry (cparser::SChan &) ref. to channel entry
//! \param   bIsChan (bool) flag distinguish channel and group
//
//! \return  --
//---------------------------------------------------------------------------
void CApiParser::initChanEntry(cparser::SChan &entry, bool bIsChan)
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
   entry.iTs          = 0;
   entry.vTs.clear();
}

//---------------------------------------------------------------------------
//
//! \brief   parse update info xml
//
//! \author  Jo2003
//! \date    12.10.2011
//
//! \param   sResp (const QString &) ref. to response string
//! \param   updInfo (cparser::SUpdInfo &) ref. to update info struct
//
//! \return  0 --> ok; -1 --> any error
//---------------------------------------------------------------------------
int CApiParser::parseUpdInfo(const QString &sResp, cparser::SUpdInfo &updInfo)
{
   int                    iRV = 0;
   QString                sSys = "n.a.";
   bool                   bStarted = false, bDone = false;
   QXmlStreamReader       xml;

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
   updInfo.iBuild   = 0;
   updInfo.sVersion = "";
   updInfo.sUrl     = "";

   xml.addData(sResp);

   while(!xml.atEnd() && !xml.hasError() && !bDone)
   {
      switch (xml.readNext())
      {
      // any xml element starts ...
      case QXmlStreamReader::StartElement:
         if (xml.name() == sSys)
         {
            bStarted = true;
         }
         else if ((xml.name() == "major") && bStarted)
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               updInfo.iMajor = xml.text().toString().toInt();
            }
         }
         else if ((xml.name() == "minor") && bStarted)
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               updInfo.iMinor = xml.text().toString().toInt();
            }
         }
         else if ((xml.name() == "build") && bStarted)
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               updInfo.iBuild = xml.text().toString().toInt();
            }
         }
         else if ((xml.name() == "string_version") && bStarted)
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               updInfo.sVersion = xml.text().toString();
            }
         }
         else if ((xml.name() == "link") && bStarted)
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               updInfo.sUrl = xml.text().toString();
            }
         }
         break;

      case QXmlStreamReader::EndElement:
         if (xml.name() == sSys)
         {
            bStarted = false;
            bDone    = true;
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

//---------------------------------------------------------------------------
//
//! \brief   check for black color (code) and correct if needed
//
//! \author  Jo2003
//! \date    17.04.2013
//
//! \param   ccode (QString &) ref. to color code
//! \param   idx (int) group index
//
//! \return  --
//---------------------------------------------------------------------------
void CApiParser::checkColor(QString& ccode, int idx)
{
   QColor col(ccode);

   if (!col.isValid() || (col == QColor("black")) || (col == QColor("#ef4444")))
   {
      // set new group color ...
      ccode = slAltColors.at(idx % slAltColors.count());
   }
}

//---------------------------------------------------------------------------
//
//! \brief   check if we should ignore the whole group
//
//! \author  Jo2003
//! \date    17.04.2013
//
//! \param   grpEntry (cparser::SChan&) ref. to group entry
//
//! \return  true --> ignore; false --> don't ignore
//---------------------------------------------------------------------------
bool CApiParser::ignoreGroup(cparser::SChan& grpEntry)
{
   // utf-8 hex code for cyrillic "Bce" ...
   const unsigned char cIgnore[] = {0xd0, 0x92, 0xd1, 0x81, 0xd0, 0xb5, 0x00};

   if (grpEntry.sName == QString::fromUtf8(QByteArray((const char *)cIgnore)))
   {
      initChanEntry(grpEntry, false);
      return true;
   }

   return false;
}

//---------------------------------------------------------------------------
//
//! \brief   in case we had no timeshift info in channel list, add them
//
//! \author  Jo2003
//! \date    09.10.2013
//
//! \param   [in/out] chanLlist (QVector<cparser::SChan> &) ref. to channel list
//! \param   [in] bitRate (int) current bitrate
//
//! \return  0
//---------------------------------------------------------------------------
int CApiParser::handleTsStuff (QVector<cparser::SChan> &chanList, int bitRate)
{
   bool bTs;

   for (int i = 0; i < chanList.count(); i ++)
   {
      if (!chanList[i].bIsGroup)
      {
         // preset!
         chanList[i].iTs = 0;
         bTs             = false;

         foreach (cparser::STimeShift ts, chanList[i].vTs)
         {
            if (   (ts.iBitRate   == bitRate)
                && (ts.iTimeShift == tmSync.timeShift()))
            {
               chanList[i].iTs = tmSync.timeShift() * 3600;
               bTs             = true;
               break;
            }
         }

#ifdef __TRACE
         if (!bTs)
         {
            mInfo(tr("%1(%4) doesn't support timeshift %2 with bitrate %3 ...")
                  .arg(chanList[i].sName).arg(tmSync.timeShift()).arg(bitRate).arg(chanList[i].iId));
         }
#else
         (void)bTs;
#endif // __TRACE
      }
   }

   return 0;
}

//---------------------------------------------------------------------------
//
//! \brief   empty prototype for VOD language parser
//!          (so far only used for Chitram.TV)
//
//! \author  Jo2003
//! \date    03.02.2014
//
//! \param   sResp (const QString&) html response
//! \param   lMap (QVodLangMap&) result map for available languages
//
//! \return  0 -> ok; -1 -> error
//---------------------------------------------------------------------------
int CApiParser::parseVodLang(const QString &sResp, QVodLangMap &lMap)
{
   Q_UNUSED(sResp)
   Q_UNUSED(lMap)

    return 0;
}

//---------------------------------------------------------------------------
//
//! \brief   parse auto stream server response
//
//! \author  Jo2003
//! \date    14.09.2015
//
//! \param   sResp [in] (const QString&) html response
//! \param   ip [out] (QString&) auto stream server ip
//
//! \return  0 -> ok; -1 -> error
//---------------------------------------------------------------------------
int CApiParser::parseAutoStreamServer(const QString &sResp, QString &ip)
{
    int iRet = -1;
    QRegExp rx("<ip>([^<]+)</ip>");

    if (rx.indexIn(sResp) > -1)
    {
        iRet = 0;
        ip = rx.cap(1);
    }

    return iRet;
}
