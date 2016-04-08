/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     defines_stalker.h
 *
 *  @author   Jo2003
 *
 *  @date     06.09.2015
 *
 *  $Id$
 *
 *///------------------------- (c) 2015 by Jo2003  --------------------------
#ifndef __20150906_DEFINES_STALKER_H
   #define __20150906_DEFINES_STALKER_H

#include <QtGlobal>

#define APP_NAME      "Lenta.TV"
#define UPD_CHECK_URL "http://rt.coujo.de/lenta_tv_ver.xml"
#define BIN_NAME      "lenta_tv"
#define API_SERVER    "v2.api.portal.lentatv.tv"
#define COMPANY_NAME  "Lenta.TV"
#define COMPANY_LINK  "<a href='http://www.lentatv.tv'>" COMPANY_NAME "</a>"
#define VERSION_APPENDIX

#define API_JSON_PATH  "/stalker_portal/api/"

// define classes of api client ...
#define ApiClient             QStalkerClient
#define ApiParser             QStalkerParser

#endif // __20150906_DEFINES_STALKER_H
