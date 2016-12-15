/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     ivi_kartina_credits.h
 *
 *  @author   Jo2003
 *
 *  @date     25.11.2016
 *
 *  $Id$
 *
 *///------------------------- (c) 2016 by Jo2003  --------------------------
#ifndef __20161125_IVI_KARTINA_CREDITS_H
    #define __20161125_IVI_KARTINA_CREDITS_H

#include <QtGlobal>

/**
  Please note!
  All values in this file are dummy values! If you want to
  use IVI video service, you have to contact them to get real
  credentials!
 */

#warning Using defined values below will lead to not working IVI stuff!

// operating system ...
#ifdef Q_OS_LINUX
    #define IVI_APP_VERSION 1234
#elif defined Q_OS_WIN32
    #define IVI_APP_VERSION 2345
#elif defined Q_OS_MAC
    #define IVI_APP_VERSION 3456
#else
    #define IVI_APP_VERSION 4567
#endif // op sys

#define IVI_KEY               "be270e1b2ca4f211fa573f3b17b20f3f"    ///< replace with correct data!
#define IVI_K1                "81d99183076c13ad"                    ///< replace with correct data!
#define IVI_K2                "03b323060ed82741"                    ///< replace with correct data!

#endif // __20161125_IVI_KARTINA_CREDITS_H
