/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     capixmlparser.cpp
 *
 *  @author   Jo2003
 *
 *  @date     18.04.2013
 *
 *  $Id$
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#include "capixmlparser.h"
#include "externals_inc.h"

//---------------------------------------------------------------------------
//
//! \brief   construct api xml parser
//
//! \author  Jo2003
//! \date    18.04.2013
//
//! \param   parent (QObject *) pointer to parent object
//
//! \return  --
//---------------------------------------------------------------------------
CApiXmlParser::CApiXmlParser(QObject * parent) : CApiParser(parent)
{
   // nothing to do here ...
}

//---------------------------------------------------------------------------
//
//! \brief   get string value from unique xml element
//
//! \author  Jo2003
//! \date    18.04.2013
//
//! \param   sElement (const QString &) ref. to XML string to parse
//! \param   sName (const QString &) ref. to element name
//
//! \return  element value as string (if found)
//---------------------------------------------------------------------------
QString CApiXmlParser::xmlElementToValue(const QString &sElement, const QString &sName)
{
   QString sValue;
   QString sRegEx = QString("<%1>([^<]+)</%1>").arg(sName);

   QRegExp rx(sRegEx);

   if (rx.indexIn(sElement) > -1)
   {
      sValue = rx.cap(1);
      sValue = sValue.simplified();
   }

   return sValue;
}

//---------------------------------------------------------------------------
//
//! \brief   parse one level of XML element
//
//! \author  Jo2003
//! \date    18.04.2013
//
//! \param   xml (QXmlStreamReader &) ref. to XML stream reader
//! \param   sEndElement (const QString &) parse until this element
//! \param   slNeeded (const QStringList &) elements to look for
//! \param   mResults QMap<QString, QString> &) ref. to results map
//
//! \return  0 --> ok; -1 --> any error
//---------------------------------------------------------------------------
int CApiXmlParser::oneLevelParser(QXmlStreamReader &xml, const QString &sEndElement,
                                  const QStringList &slNeeded, QMap<QString, QString> &mResults)
{
   QString sUnknown, sKey, sVal;
   bool    bEndMain = false;
   mResults.clear();

   while(!xml.atEnd() && !xml.hasError() && !bEndMain)
   {
      switch (xml.readNext())
      {
      // start element ...
      case QXmlStreamReader::StartElement:

         // needed element ... ?
         if (slNeeded.contains(xml.name().toString()))
         {
            // store key / value in map ...
            // make sure we add an empty string if there is no text
            // inside this element.
            sKey = xml.name().toString();
            sVal = "";

            if (xml.readNext() == QXmlStreamReader::Characters)
            {
               sVal = xml.text().toString();
            }

            mResults.insert(sKey, sVal);
         }
         else if (xml.name().toString() == sEndElement)
         {
            // maybe end element isn't searched ...
            // to get the end element we should NOT count it
            // as unknown ...
         }
         else
         {
            // starttag unknown element ...
            sUnknown = xml.name().toString();

#ifndef QT_NO_DEBUG
            mInfo(tr("Found unused element %1 ...").arg(sUnknown));
#endif

            // search for endtag of unknown element ...
            ignoreUntil(xml, sUnknown);
         }
         break;

      case QXmlStreamReader::EndElement:
         if (xml.name().toString() == sEndElement)
         {
            bEndMain = true;
         }
         break;

      default:
         break;
      }
   }

   return 0;
}

//---------------------------------------------------------------------------
//
//! \brief   parse one level of XML element
//
//! \author  Jo2003
//! \date    18.04.2013
//
//! \param   xml (QXmlStreamReader &) ref. to XML stream reader
//! \param   sEndElement (const QString &) read forward until this element
//
//! \return  0 --> ok; -1 --> any error
//---------------------------------------------------------------------------
int CApiXmlParser::ignoreUntil(QXmlStreamReader &xml, const QString &sEndElement)
{
   while(!xml.atEnd() && !xml.hasError())
   {
      if ((xml.readNext() == QXmlStreamReader::EndElement)
         && (xml.name().toString() == sEndElement))
      {
         // found end tag of searched element ...
         break;
      }
   }

   return (xml.atEnd() || xml.hasError()) ? -1 : 0;
}

//---------------------------------------------------------------------------
//
//! \brief   check response for errors
//
//! \author  Jo2003
//! \date    18.04.2013
//
//! \param   sResp (const QString&) ref. to XML response
//! \param   sMsg (QString&) store error message there
//! \param   eCode (int&) store error coe there
//
//! \return  0 --> ok; -1 --> pattern doesn't match
//---------------------------------------------------------------------------
int CApiXmlParser::parseError (const QString& sResp, QString& sMsg, int& eCode)
{
   int     iRet = 0;
   QRegExp rx("<message>(.*)</message>[ \t\n\r]*"
              "<code>(.*)</code>");

   // quick'n'dirty error parser ...
   if ((iRet = rx.indexIn(sResp)) > -1)
   {
      sMsg  = rx.cap(1);
      eCode = rx.cap(2).toInt();
   }

   return iRet;
}
