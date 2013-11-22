/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadUR$
 *
 *  @file     qcustparser.cpp
 *
 *  @author   Jo2003
 *
 *  @date     26.03.2013
 *
 *  $Id$
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#include "qcustparser.h"

extern CDirStuff *pFolders;

//---------------------------------------------------------------------------
//
//! \brief   constructs QCustParser object
//
//! \author  Jo2003
//! \date    10.09.2012
//
//! \param   parent pointer to parent object
//
//! \return  --
//---------------------------------------------------------------------------
QCustParser::QCustParser(QObject *parent) :
   QObject(parent)
{
   // Insert initial string values. Values will be replaced
   // if customization was found!
   mStrings.insert("APP_NAME"            ,  APP_NAME            );
   mStrings.insert("UPD_CHECK_URL"       ,  UPD_CHECK_URL       );
   mStrings.insert("API_SERVER"          ,  API_SERVER          );
   mStrings.insert("COMPANY_NAME"        ,  COMPANY_NAME        );
   mStrings.insert("COMPANY_LINK"        ,  COMPANY_LINK        );
   mStrings.insert("API_XML_PATH"        ,  API_XML_PATH        );
   mStrings.insert("API_JSON_PATH"       ,  API_JSON_PATH       );
   mStrings.insert("APPLICATION_SHORTCUT",  APPLICATION_SHORTCUT);
}

//---------------------------------------------------------------------------
//
//! \brief   destroys QCustParser object
//
//! \author  Jo2003
//! \date    26.03.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
QCustParser::~QCustParser()
{
   // nothing to do so far ...
}

//---------------------------------------------------------------------------
//
//! \brief   try to load customization resources
//
//! \author  Jo2003
//! \date    26.03.2013
//
//! \param   --
//
//! \return  0 -> resource loaded; -1 -> no customization found
//---------------------------------------------------------------------------
int QCustParser::loadCustResource()
{
   int  iRet      = -1;
   QDir resDir    = pFolders->getResDir();
   QStringList sl = resDir.entryList(QStringList() << "*.qcr");

   if (!sl.isEmpty())
   {
      iRet = QResource::registerResource(QString("%1/%2").arg(pFolders->getResDir()).arg(sl[0])) ? 0 : -1;
   }

   if (iRet < 0)
   {
      QDir::addSearchPath("branding", ":/unbranded");
   }
   else
   {
      QDir::addSearchPath("branding", ":/oem");
#ifdef __EXTRACT_BRANDING
      QFile::copy("branding:cust", "/tmp/cust.xml");
      QFile::copy("branding:main/icon", "/tmp/icon.png");
      QFile::copy("branding:main/live", "/tmp/live.png");
      QFile::copy("branding:video/logo", "/tmp/logo.png");
#endif // __EXTRACT_BRANDING
   }

   return iRet;
}

//---------------------------------------------------------------------------
//
//! \brief   parse customization resources (if there)
//
//! \author  Jo2003
//! \date    26.03.2013
//
//! \param   --
//
//! \return  1 -> no resource, fall back;
//!          0 -> resource successfully parsed;
//!         -1 -> error parsing resource,
//!               a mixed customization might be used
//---------------------------------------------------------------------------
int QCustParser::parseCust()
{
   int iRet = 0;

   if (!loadCustResource())
   {
      // we have customize resource ...
      QXmlStreamAttributes attr;
      QXmlStreamReader     xml;
      QFile                resFile("branding:cust");

      if (resFile.open(QIODevice::ReadOnly | QIODevice::Text))
      {
         xml.setDevice(&resFile);

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
               if (xml.name() == "string")
               {
                  attr = xml.attributes();
                  mStrings.insert(attr.value("name").toString(), attr.value("value").toString());
               }
               break;

            default:
               break;

            } // end switch ...
         } // end while ...
      }

      iRet = xml.hasError() ? -1 : 0;
   }
   else
   {
      // use fallback variant ...
      iRet = 1;
   }

   return iRet;
}

//---------------------------------------------------------------------------
//
//! \brief   get string value to KEY
//
//! \author  Jo2003
//! \date    26.03.2013
//
//! \param   name (const QString&) ref. to key string
//
//! \return  string value if found; else empty string
//---------------------------------------------------------------------------
const QString& QCustParser::strVal(const QString &name)
{
   return mStrings[name];
}
