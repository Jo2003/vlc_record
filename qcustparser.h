/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/sunduk.tv/qcustparser.h $
 *
 *  @file     qcustparser.h
 *
 *  @author   Jo2003
 *
 *  @date     26.03.2013
 *
 *  $Id: qcustparser.h 1053 2013-03-26 09:04:35Z Olenka.Joerg $
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#ifndef __20120910_QCUSTPARSER_H
   #define __20120910_QCUSTPARSER_H

#include <QObject>
#include <QString>
#include <QXmlStreamReader>
#include <QDir>
#include <QResource>

#include "cdirstuff.h"

//---------------------------------------------------------------------------
//! \class   QCustParser
//! \date    26.03.2013
//! \author  Jo2003
//! \brief   simple customization parser class
//---------------------------------------------------------------------------
class QCustParser : public QObject
{
   Q_OBJECT

public:
   QCustParser(QObject *parent = 0);
   virtual ~QCustParser();

   const QString& strVal(const QString &name);
   int parseCust();

private:
   QMap<QString, QString> mStrings;

protected:
   int loadCustResource();

signals:

public slots:

};

#endif // __20120910_QCUSTPARSER_H
