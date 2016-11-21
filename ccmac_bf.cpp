/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     ccmac_bf.cpp
 *
 *  @author   Jo2003
 *
 *  @date     21.11.2016
 *
 *  $Id$
 *
 *///------------------------- (c) 2016 by Jo2003  --------------------------
#include "ccmac_bf.h"


//------------------------------------------------------------------------------
//! @brief      Constructs the object.
//!
//! @param[in]  k     initial key as hex string
//! @param[in]  k1    k1 key as hrx string
//! @param[in]  k2    k2 key as hex string
//------------------------------------------------------------------------------
CCMAC_Bf::CCMAC_Bf(const QString &k, const QString &k1, const QString &k2)
    : mKey(QByteArray::fromHex(k.toUtf8())),
      mK1(QByteArray::fromHex(k1.toUtf8())),
      mK2(QByteArray::fromHex(k2.toUtf8())),
      mBs(8)
{
    pBubbles = new QBlowfish(mKey);
}

//------------------------------------------------------------------------------
//! @brief      Destroys the object.
//------------------------------------------------------------------------------
CCMAC_Bf::~CCMAC_Bf()
{
    if (pBubbles)
    {
        delete pBubbles;
        pBubbles = NULL;
    }
}

//------------------------------------------------------------------------------
//! @brief      get cmac-bf sign to message
//!
//! @param[in]  message  message to sign
//!
//! @return     sign for message as string
//------------------------------------------------------------------------------
QString CCMAC_Bf::sign(const QString &message)
{
    QByteArray  msg  = message.toUtf8();
    QByteArray  prev = QByteArray(mBs, '\0');
    QByteArray  tok;
    QByteArray  key;
    int         i;
    int         pad;
    int         loop = msg.size() / mBs;

    if ((pad = (msg.size() % mBs)) == 0)
    {
        loop --;
    }

    for (i = 0; i < loop; i ++)
    {
        tok   = msg.mid(i * mBs, mBs);
        prev  = pBubbles->encrypted(xor64(tok, prev));
    }

    if (pad == 0)
    {
        key = mK1;
        tok = msg.mid(loop * mBs, mBs);
    }
    else
    {
        key = mK2;
        tok = msg.mid(loop * mBs, pad);
        tok.append(0x80);

        for (i = (pad + 1); i < mBs; i++)
        {
            tok.append((char)0);
        }
    }

    return QString(pBubbles->encrypted(xor64(xor64(tok, prev), key)).toHex());
}

//------------------------------------------------------------------------------
//! @brief      xor for 64 bit using byte arrays
//!
//! @param[in]  val1  The value 1
//! @param[in]  val2  The value 2
//!
//! @return     xored value
//------------------------------------------------------------------------------
QByteArray CCMAC_Bf::xor64(const QByteArray& val1, const QByteArray& val2)
{
    QByteArray ba;

    for (int i = 0; i < mBs; i++)
    {
        ba.append(val1.at(i) ^ val2.at(i));
    }

    return ba;
}
