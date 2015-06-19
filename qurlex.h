/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @descr    compatibility layer between Qt4 and Qt5
 *
 *  @file     qurlex.h
 *
 *  @author   Jo2003
 *
 *  @date     19.06.2015
 *
 *  $Id$
 *
 *///------------------------- (c) 2015 by Jo2003  --------------------------
#ifndef __20150619_QURLEX_H
    #define __20150619_QURLEX_H
#include <QUrl>

#ifdef __QT_5
    #include <QUrlQuery>
#endif // __QT_5

//---------------------------------------------------------------------------
//! \class   QUrlEx
//! \date    19.06.2015
//! \author  Jo2003
//! \brief   compatibility layer between Qt4 and Qt5 for QUrl / QUrlQuery
//---------------------------------------------------------------------------
class QUrlEx : public QUrl
{
public:
#ifdef __QT_5
    QUrlEx() : QUrl(), mQuery(){}
    QUrlEx(const QUrl& other) : QUrl(other), mQuery(other.query()){}
    QUrlEx(const QString & url, ParsingMode parsingMode = TolerantMode) : QUrl(url, parsingMode), mQuery(url){}

    virtual void addQueryItem(const QString & key, const QString & value)
    {
        mQuery.addQueryItem(key, value);
        setQuery(mQuery);
    }

    virtual QString queryItemValue (const QString& key) const
    {
        return mQuery.queryItemValue(key);
    }

    virtual void clear()
    {
        QUrl::clear();
        mQuery.clear();
    }

#else
    QUrlEx() : QUrl(){}
    QUrlEx(const QString & url) : QUrl(url){}
    QUrlEx(const QUrl & other) :  QUrl(other){}
    QUrlEx(const QString & url, QUrl::ParsingMode parsingMode) : QUrl(url, parsingMode){}

    virtual QString query() const
    {
        return QString(encodedQuery());
    }

#endif // __QT_5

    virtual ~QUrlEx(){}

#ifdef __QT_5

private:
    QUrlQuery mQuery;

#endif // __QT_5
};

#endif // __20150619_QURLEX_H
