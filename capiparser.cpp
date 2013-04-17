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

// log file functions ...
extern CLogFile VlcLog;

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
   iOffset    = 0;

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
   /// Note:
   /// This function is a little tricky ...
   /// Try to find out the real difference between the
   /// time kartina.tv assumes for us and the real time
   /// running on this machine ...
   /// Round offset to full 30 minutes (min. timezone step)

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

//---------------------------------------------------------------------------
//
//! \brief   fix time in channel list as sent from server
//
//! \author  Jo2003
//! \date    15.04.2013
//
//! \param   uiSrvTime (uint &) ref. to time stamp
//
//! \return  0 --> unchanged; 1 --> fixed
//---------------------------------------------------------------------------
int CApiParser::fixTime (uint &uiTime)
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

//---------------------------------------------------------------------------
//
//! \brief   return time offset
//
//! \author  Jo2003
//! \date    17.04.2013
//
//! \param   --
//
//! \return  time offset in seconds
//---------------------------------------------------------------------------
int CApiParser::GetFixTime()
{
   return iOffset;
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

   if (!col.isValid() || (col == QColor("black")))
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
   // utf-8 hex code for cyrillic "all" ...
   const char cIgnore[] = {0xd0, 0x92, 0xd1, 0x81, 0xd0, 0xb5, 0x00};

   if (grpEntry.sProgramm == QString::fromUtf8(QByteArray(cIgnore)))
   {
      initChanEntry(grpEntry, false);
      return true;
   }

   return false;
}
