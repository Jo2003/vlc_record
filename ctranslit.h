/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 08.02.2010 / 10:50:35
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#ifndef __020810__CTRANSLIT_H
   #define __020810__CTRANSLIT_H

#include <QtCore>
#include <QString>
#include <QTextStream>
#include <QChar>
#include <QMap>

// max. length of phrase ...
#define MAX_LAT2CYR_LEN 4

/********************************************************************\
|  Class: CTranslit
|  Date:  08.02.2010 / 11:00:28
|  Author: Jo2003
|  Description: class to make translit between latin and cyrillic
|
\********************************************************************/
class CTranslit
{
public:
    CTranslit();
    ~CTranslit ();

    QString CyrToLat (const QString &str, bool fileName = true);
    QString LatToCyr (const QString &str, bool fileName = true);

private:
    QMap<QString, QString> mCyr2Lat;
    QMap<QString, QString> mLat2Cyr;
};

#endif /* __020810__CTRANSLIT_H */
/************************* History ***************************\
| $Log$
\*************************************************************/
