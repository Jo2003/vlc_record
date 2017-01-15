/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/api_inc.h $
 *
 *  @file     api_inc.h
 *
 *  @author   Jo2003
 *
 *  @date     15.04.2013
 *
 *  $Id: api_inc.h 1303 2014-03-27 15:51:56Z Olenka.Joerg $
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#ifndef __20130416_API_INC_H
   #define __20130416_API_INC_H

/// Make sure to include the correct api headers ...
#ifdef _TASTE_OTRADA_TV
   #include "cotradaclient.h"
   #include "cotradaparser.h"
#elif defined _TASTE_IPTV_RECORD
   #include "crodnoeclient.h"
   #include "crodnoeparser.h"
#elif defined _TASTE_NOVOE_TV
   #include "cnovoeclient.h"
   #include "cnovoeparser.h"
#elif defined _TASTE_MOIDOM_TV
   #include "cmoidomxmlparser.h"
   #include "ckartinaclnt.h"
#elif defined _TASTE_SUNDUK_TV
   #include "csundukclnt.h"
   #include "cstdjsonparser.h"
#elif defined _TASTE_TELEPROM
   #include "ctelepromclient.h"
   #include "cstdjsonparser.h"
#elif defined _TASTE_TV_CLUB
   #include "ctvclubclient.h"
   #include "ctvclubparser.h"
#else
   #include "ckartinaclnt.h"
   #ifdef _USE_QJSON
      #include "cstdjsonparser.h"
   #else
      #include "ckartinaxmlparser.h"
   #endif
#endif

#endif // __20130416_API_INC_H
