/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/sunduk.tv/csundukclnt.h $
 *
 *  @file     csundukclnt.h
 *
 *  @author   Jo2003
 *
 *  @date     12.12.2013
 *
 *  $Id: csundukclnt.h 1509 2015-03-04 11:26:51Z Olenka.Joerg $
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#ifndef __20131212_CSUNDUKCLNT_H
   #define __20131212_CSUNDUKCLNT_H

#include "ckartinaclnt.h"
#include <QObject>
#include <QUrl>

//---------------------------------------------------------------------------
//! \class   CSundukClnt
//! \date    12.12.2013
//! \author  Jo2003
//! \brief   subclass kartina client to support sunduk special handling
//---------------------------------------------------------------------------
class CSundukClnt : public CKartinaClnt
{
   Q_OBJECT

public:
   CSundukClnt(QObject *parent);
   virtual ~CSundukClnt();
   virtual int queueRequest(CIptvDefs::EReq req, const QVariant& par_1 = QVariant(), const QVariant& par_2 = QVariant());

protected:
   virtual void GetCookie ();
   virtual void GetStreamURL (int iChanID, const QString &secCode, bool bTimerRec = false);
   virtual void GetArchivURL (const QString &prepared, const QString &secCode);
   virtual void GetVodUrl (int iVidId, const QString &secCode);
   virtual void GetVodUrl (const QUrl &dst);
   virtual void GetVodTypes();
   QString getStreamProtocol(CIptvDefs::EReq req);

public slots:
   void slotStrProto(QString p);

protected:
   QString sStrProto;
};

#endif // __20131212_CSUNDUKCLNT_H
