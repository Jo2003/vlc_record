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

// operating system ...
#ifdef Q_OS_LINUX
    #define IVI_APP_VERSION 5508
#elif defined Q_OS_WIN32
    #define IVI_APP_VERSION 5510
#elif defined Q_OS_MAC
    #define IVI_APP_VERSION 5509
#else
    #define IVI_APP_VERSION 1234
#endif // op sys

#define IVI_KEY               "bde9e570140e26daee8b4cdce3d5bb85"    ///< replace with correct data!
#define IVI_K1                "e75d776a96bc563c"                    ///< replace with correct data!
#define IVI_K2                "cebaeed52d78ac63"                    ///< replace with correct data!

#endif // __20161125_IVI_KARTINA_CREDITS_H
