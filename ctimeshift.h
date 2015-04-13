/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/ctimeshift.h $
 *
 *  @file     ctimeshift.h
 *
 *  @author   Jo2003
 *
 *  @date     07.08.2013
 *
 *  $Id: ctimeshift.h 1168 2013-08-08 13:21:21Z Olenka.Joerg $
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#ifndef __20130807_CTIMESHIFT_H
   #define __20130807_CTIMESHIFT_H

#include <QObject>
#include <QString>

// internal value for internal timeshift value usage ...
// most commonly unused ...
#define INTERN_TS  -2899

//---------------------------------------------------------------------------
//! \class   CTimeShift
//! \date    07.08.2013
//! \author  Jo2003
//! \brief   global class for timeshift chaos
//---------------------------------------------------------------------------
class CTimeShift : public QObject
{
   Q_OBJECT

public:
   explicit CTimeShift(QObject *parent = 0);
   ~CTimeShift();

   int  timeShift();
   void setTimeShift(int i);

   uint    fromGmt(uint uit, int iTs = INTERN_TS);
   uint    toGmt(uint uit, int iTs = INTERN_TS);
   QString fromGmtFormatted(uint uit, const QString& format, int iTs = INTERN_TS);
   QString toGmtFormatted(uint uit, const QString& format, int iTs = INTERN_TS);
   
private:
   int _ts;
   
};

#endif // __20130807_CTIMESHIFT_H
