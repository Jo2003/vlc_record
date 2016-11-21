/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     ccmac_bf.h
 *
 *  @author   Jo2003
 *
 *  @date     21.11.2016
 *
 *  $Id$
 *
 *///------------------------- (c) 2016 by Jo2003  --------------------------
#ifndef __20161121_CCMAC_BF_H
    #define __20161121_CCMAC_BF_H

#include <qblowfish.h>
#include <QString>

//------------------------------------------------------------------------------
//! @brief      class to do CMAC Blowfish signing
//------------------------------------------------------------------------------
class CCMAC_Bf
{
public:
    CCMAC_Bf(const QString& k, const QString& k1, const QString& k2);
    ~CCMAC_Bf();
    QString sign(const QString& message);

protected:
    QByteArray xor64(const QByteArray &val1, const QByteArray &val2);

    QBlowfish       *pBubbles;
    const QByteArray mKey;
    const QByteArray mK1;
    const QByteArray mK2;
    const int        mBs;
};

#endif // __20161121_CCMAC_BF_H
