/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/capixmlparser.h $
 *
 *  @file     capixmlparser.h
 *
 *  @author   Jo2003
 *
 *  @date     18.04.2013
 *
 *  $Id: capixmlparser.h 1148 2013-07-24 14:27:14Z Olenka.Joerg $
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#ifndef __20130418_CAPIXMLPARSER_H
   #define __20130418_CAPIXMLPARSER_H

#include <QXmlStreamReader>
#include <QRegExp>
#include <QDateTime>
#include <QStringList>

#include "clogfile.h"
#include "defdef.h"
#include "cparser.h"
#include "capiparser.h"

//---------------------------------------------------------------------------
//! \class   CApiXmlParser
//! \date    18.04.2013
//! \author  Jo2003
//! \brief   parent class for all IPTV api XML parser
//---------------------------------------------------------------------------
class CApiXmlParser : public CApiParser
{
   Q_OBJECT

public:
   CApiXmlParser(QObject * parent = 0);
   virtual int parseError (const QString& sResp, QString& sMsg, int& eCode);
   virtual QString xmlElementToValue (const QString &sElement, const QString &sName);

protected:
   virtual int oneLevelParser (QXmlStreamReader &xml, const QString &sEndElement, const QStringList& slNeeded, QMap<QString, QString>& mResults);
   virtual int ignoreUntil(QXmlStreamReader &xml, const QString &sEndElement);
};

#endif // __20130418_CAPIXMLPARSER_H
