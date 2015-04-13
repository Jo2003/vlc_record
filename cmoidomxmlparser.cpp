/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/cmoidomxmlparser.cpp $
 *
 *  @file     cmoidomxmlparser.cpp
 *
 *  @author   Jo2003
 *
 *  @date     02.12.2013
 *
 *  $Id: cmoidomxmlparser.cpp 1252 2013-12-02 19:07:04Z Olenka.Joerg $
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#include "cmoidomxmlparser.h"

// log file functions ...
extern CLogFile VlcLog;

//---------------------------------------------------------------------------
//
//! \brief   constructs CMoiDomXmlParser object
//
//! \author  Jo2003
//! \date    02.12.2013
//
//! \param   parent (QObject*) pointer to parent object
//
//! \return  --
//---------------------------------------------------------------------------
CMoiDomXmlParser::CMoiDomXmlParser(QObject *parent) :
   CKartinaXMLParser(parent)
{
   // nothing to do so far ...
}

//---------------------------------------------------------------------------
//
//! \brief   destroys CMoiDomXmlParser object
//
//! \author  Jo2003
//! \date    02.12.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
CMoiDomXmlParser::~CMoiDomXmlParser()
{
   // and still nothing ...
}

//---------------------------------------------------------------------------
//
//! \brief   parse channels from xml response
//
//! \author  Jo2003
//! \date    02.12.2013
//
//! \param   xml (QXmlStreamReader &) ref. to xml reader
//! \param   chanList (QVector<cparser::SChan> &) ref. to channel list
//! \param   bFixTime (bool) should we fix time if(true)
//
//! \return  0
//---------------------------------------------------------------------------
int CMoiDomXmlParser::parseChannels(QXmlStreamReader &xml, QVector<cparser::SChan> &chanList,
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
         else if (xml.name() == "audiotracks")
         {
            chanEntry.uiDefAud = parseATracks(xml);
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

//---------------------------------------------------------------------------
//
//! \brief   get default audio stream index from xml response
//
//! \author  Jo2003
//! \date    02.12.2013
//
//! \param   xml (QXmlStreamReader &) ref. to xml reader
//
//! \return  default index
//---------------------------------------------------------------------------
uint CMoiDomXmlParser::parseATracks(QXmlStreamReader &xml)
{
   uint             uiRet = 0;
   QStringList      sl;
   QString          sDef;

   // while no end of audio track stuff ...
   while (!((xml.readNext() == QXmlStreamReader::EndElement)
      && (xml.name() == "audiotrack_default")))
   {
      if (xml.tokenType() == QXmlStreamReader::StartElement)
      {
         // audio track item  ...
         if (xml.name() == "item")
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               sl << xml.text().toString();
            }
         }
         else if (xml.name() == "audiotrack_default")
         {
            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               sDef = xml.text().toString();
            }
         }
      }
   }

   if (!sl.isEmpty() && (sl.count() > 1))
   {
      uiRet = (uint)sl.indexOf(sDef);

      if (uiRet == (uint)-1)
      {
         uiRet = 0;
      }
   }

   return uiRet;
}
