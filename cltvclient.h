/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     cltvclient.h
 *
 *  @author   Jo2003
 *
 *  @date     16.07.2013
 *
 *  $Id$
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#ifndef __20130716_CLTVCLIENT_H
   #define __20130716_CLTVCLIENT_H
#include "ckartinaclnt.h"

//---------------------------------------------------------------------------
//! \class   CLtvClient
//! \date    16.07.2013
//! \author  Jo2003
//! \brief   kartina clients small child ...
//---------------------------------------------------------------------------
class CLtvClient : public CKartinaClnt
{
   Q_OBJECT

public:
   explicit CLtvClient(QObject *parent = 0);
   ~CLtvClient();

protected:
   virtual void slotStringResponse (int reqId, QString strResp);
};

#endif // __20130716_CLTVCLIENT_H
