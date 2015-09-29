/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qstalkerparser.cpp
 *
 *  @author   Jo2003
 *
 *  @date     06.09.2015
 *
 *  $Id$
 *
 *///------------------------- (c) 2015 by Jo2003  --------------------------
#include "qstalkerparser.h"

// log file functions ...
extern CLogFile VlcLog;

QStalkerParser::QStalkerParser(QObject *parent)
    : CStdJsonParser(parent)
{
}

int QStalkerParser::parseAuth(const QString &sResp, cparser::SAuth& auth)
{
    int  iRV = 0;
    bool bOk = false;
    QVariantMap contentMap;

    contentMap = QtJson::parse(sResp, bOk).toMap();

    if (bOk)
    {
        auth.toktype      = contentMap.value("token_type").toString();
        auth.token        = contentMap.value("access_token").toString();
        auth.expires      = contentMap.value("expires_in").toInt();
        auth.refreshToken = contentMap.value("refresh_token").toString();
        auth.userId       = contentMap.value("user_id").toInt();
    }
    else
    {
       emit sigError((int)Msg::Error, tr("Error in %1").arg(__FUNCTION__),
                     tr("QtJson parser error in %1 %2():%3")
                     .arg(__FILE__).arg(__FUNCTION__).arg(__LINE__));

       iRV = -1;
    }

    return iRV;
}

