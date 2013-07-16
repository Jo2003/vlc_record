/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     api_inc.h
 *
 *  @author   Jo2003
 *
 *  @date     15.04.2013
 *
 *  $Id$
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#ifndef __20130416_API_INC_H
   #define __20130416_API_INC_H

/// Make sure to include the correct api headers ...
#ifdef _TASTE_IPTV_RECORD
   #include "crodnoeclient.h"
   #include "crodnoeparser.h"
#elif _TASTE_NOVOE_TV
   #include "cnovoeclient.h"
   #include "cnovoeparser.h"
#elif _TASTE_LTVISION
   #include "cltvclient.h"
   #include "ckartinaxmlparser.h"
#else
   #include "ckartinaclnt.h"
   #ifdef _USE_QJSON
      #include "cstdjsonparser.h"
   #else
      #include "ckartinaxmlparser.h"
   #endif
#endif

#endif // __20130416_API_INC_H
