/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     civistats.h
 *
 *  @author   Jo2003
 *
 *  @date     03.01.2017
 *
 *  $Id$
 *
 *///------------------------- (c) 2017 by Jo2003  --------------------------
#ifndef __20170103_CIVISTATS_H
    #define __20170103_CIVISTATS_H

#include <QObject>
#include <QTimer>

namespace ivistats
{
    ///
    /// \brief The SPixelAudit struct
    ///
    struct SPixelAudit
    {

    };

    ///
    /// \brief The SIviStats struct
    ///
    struct SIviStats
    {
        SIviStats() : mPost(false){}
        QString mUrl;       ///< url for request
        QString mContent;   ///< optional content data
        bool    mPost;      ///< post if tree; else get
    };
}

///
/// \brief The CIviStats class
///
class CIviStats : public QObject
{
    Q_OBJECT

public:
    CIviStats(QObject* parent = 0);
    virtual ~CIviStats();

signals:
    void sigStats(ivistats::SIviStats stats);
};

#endif // __20170103_CIVISTATS_H
