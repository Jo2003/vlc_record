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
    cparser::SChan chanEntry;
    QStringList    used;
    QString        tagName;
    bool           readNext = true;

    // we are parsing this tree only in a flat way.
    // so we know what we're looking for ...
    used << "item" << "stream_params" << "audiotrack_default" << "name"
         << "id"   << "is_video"      << "have_archive"       << "protected"
         << "icon" << "epg_progname"  << "epg_start"          << "epg_end"
         << "hide";

    while (readNext)
    {
        switch (xml.readNext())
        {
        case QXmlStreamReader::StartElement:
            tagName = xml.name().toString();

            if (!used.contains(tagName))
            {
#ifndef QT_NO_DEBUG
                mInfo(tr("Found unused element %1 ...").arg(tagName));
#endif
                ignoreUntil(xml, tagName);
            }
            else if (tagName == "item")
            {
                initChanEntry(chanEntry);
            }
            else if (tagName == "stream_params")
            {
                // go into next level ... parse stream params ...
                parseStreamParams(xml, chanEntry.vTs);
            }
            break;

        case QXmlStreamReader::EndElement:
            if (xml.name() == "channels")
            {
                // done!
                readNext = false;
            }
            else if (xml.name() == "item")
            {
                // item completed ...
                chanList.push_back(chanEntry);
            }
            break;

        case QXmlStreamReader::Characters:
            if (tagName == "name")
            {
                chanEntry.sName = xml.text().toString();
            }
            else if (tagName == "id")
            {
                chanEntry.iId = xml.text().toString().toInt();
            }
            else if (tagName == "is_video")
            {
                chanEntry.bIsVideo = (xml.text().toString().toInt() == 1) ? true : false;
            }
            else if (tagName == "have_archive")
            {
                chanEntry.bHasArchive = (xml.text().toString().toInt() == 1) ? true : false;
            }
            else if (tagName == "protected")
            {
                chanEntry.bIsProtected = (xml.text().toString().toInt() == 1) ? true : false;
            }
            else if (tagName == "icon")
            {
                chanEntry.sIcon = xml.text().toString();
            }
            else if (tagName == "epg_progname")
            {
                chanEntry.sProgramm = xml.text().toString();
            }
            else if (tagName == "epg_start")
            {
                chanEntry.uiStart = xml.text().toString().toUInt();

                if (bFixTime)
                {
                    fixTime(chanEntry.uiStart);
                }
            }
            else if (tagName == "epg_end")
            {
                chanEntry.uiEnd = xml.text().toString().toUInt();

                if (bFixTime)
                {
                    fixTime(chanEntry.uiEnd);
                }
            }
            else if (tagName == "hide")
            {
                chanEntry.bIsHidden = (xml.text().toString().toInt() == 1) ? true : false;
            }
            else if (tagName == "audiotrack_default")
            {
                chanEntry.sLangCode = xml.text().toString();
            }
            break;

        default:
            readNext = false;
            break;
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
   QString          sName;
   bool             readNext = true;

   while (readNext)
   {
       switch(xml.readNext())
       {
       case QXmlStreamReader::StartElement:
           sName = xml.name().toString();
           break;

       case QXmlStreamReader::EndElement:
           sName = "";
           if (xml.name() == "audiotrack_default")
           {
               readNext = false;
           }
           break;

       case QXmlStreamReader::Characters:
           if (sName == "item")
           {
               sl << xml.text().toString();
           }
           else if (sName == "audiotrack_default")
           {
               sDef = xml.text().toString();
           }
           sName = "";
           break;

       default:
           readNext = false;
           break;
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
