/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qstalkerparser.h
 *
 *  @author   Jo2003
 *
 *  @date     06.09.2015
 *
 *  $Id$
 *
 *///------------------------- (c) 2015 by Jo2003  --------------------------
#ifndef __20150906_QSTALKERPARSER_H
    #define __20150906_QSTALKERPARSER_H

#include "cstdjsonparser.h"

class QStalkerParser : public CStdJsonParser
{
Q_OBJECT

public:
   QStalkerParser(QObject* parent = 0);
   virtual int parseAuth(const QString &sResp, cparser::SAuth auth);
};

#endif // __20150906_QSTALKERPARSER_H
