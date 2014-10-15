/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     cmoidomxmlparser.h
 *
 *  @author   Jo2003
 *
 *  @date     02.12.2013
 *
 *  $Id$
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#ifndef __20131202_CMOIDOMXMLPARSER_H
   #define __20131202_CMOIDOMXMLPARSER_H

#include <QObject>
#include "ckartinaxmlparser.h"

//---------------------------------------------------------------------------
//! \class   CMoiDomXmlParser
//! \date    02.12.2013
//! \author  Jo2003
//! \brief   slightly changed xml parser for Moi-Dom.TV
//---------------------------------------------------------------------------
class CMoiDomXmlParser : public CKartinaXMLParser
{
   Q_OBJECT
public:
   CMoiDomXmlParser(QObject *parent = 0);
   virtual ~CMoiDomXmlParser();

protected:
   virtual int parseChannels(QXmlStreamReader &xml, QVector<cparser::SChan> &chanList, bool bFixTime);
   virtual uint parseATracks(QXmlStreamReader &xml);
};

#endif // __20131202_CMOIDOMXMLPARSER_H
