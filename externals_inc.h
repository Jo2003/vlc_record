/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     external_inc.h
 *
 *  @author   Jo2003
 *
 *  @date     06.08.2014
 *
 *  $Id$
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#ifndef __20140806_EXTERNALS_INC_H
   #define __20140806_EXTERNALS_INC_H

#include <QTranslator>
#include "api_inc.h"
#include "qdatetimesyncro.h"
#include "qcustparser.h"
#include "qfusioncontrol.h"
#include "clogfile.h"
#include "cdirstuff.h"
#include "cshowinfo.h"
#include "cvlcrecdb.h"
#include "chtmlwriter.h"
#include "qchannelmap.h"
#include "qstatemessage.h"
#include "qwatchstats.h"

// global syncronized time ...
extern QDateTimeSyncro tmSync;

// global customization class ...
extern QCustParser *pCustomization;

// fusion control ...
extern QFusionControl missionControl;

// for logging ...
extern CLogFile VlcLog;

// for folders ...
extern CDirStuff *pFolders;

// global showinfo class ...
extern CShowInfo showInfo;

// global rec db ...
extern CVlcRecDB *pDb;

// global client api classes ...
extern ApiClient *pApiClient;
extern ApiParser *pApiParser;

// global translaters ...
extern QTranslator *pAppTransl;
extern QTranslator *pQtTransl;

// global html writer ...
extern CHtmlWriter *pHtml;

// gloabl channel map ...
extern QChannelMap *pChanMap;

// global state message engine ...
extern QStateMessage *pStateMsg;

// global watch statistics
extern QWatchStats* pWatchStats;

#endif // __20140806_EXTERNALS_INC_H
