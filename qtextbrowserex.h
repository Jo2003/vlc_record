/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @descr    compatibility layer between Qt4 and Qt5
 *
 *  @file     qtextbrowserex.h
 *
 *  @author   Jo2003
 *
 *  @date     19.06.2015
 *
 *  $Id$
 *
 *///------------------------- (c) 2015 by Jo2003  --------------------------
#ifndef __20150619_QTEXTBROWSEREX_H
   #define __20150619_QTEXTBROWSEREX_H

#include <QTextBrowser>
#include "qurlex.h"

//---------------------------------------------------------------------------
//! \class   QTextBrwoserEx
//! \date    19.06.2015
//! \author  Jo2003
//! \brief   text browser which supports QUrlEx
//---------------------------------------------------------------------------
class QTextBrowserEx : public QTextBrowser
{
    Q_OBJECT

public:

    //---------------------------------------------------------------------------
    //
    //! \brief   create QTextBrowserEx object
    //
    //! \author  Jo2003
    //! \date    19.06.2015
    //
    //! \param   [in] parent (QWidget*) pointer to parent widget
    //---------------------------------------------------------------------------
    QTextBrowserEx(QWidget* parent = 0) : QTextBrowser(parent)
    {
        connect (this, SIGNAL(anchorClicked(QUrl)), this, SLOT(translateClick(QUrl)));
    }

    // destructor
    virtual ~QTextBrowserEx(){}

private slots:

    //---------------------------------------------------------------------------
    //
    //! \brief   catch anchorClicked link
    //
    //! \author  Jo2003
    //! \date    19.06.2015
    //
    //! \param   [in] link (const QUrl&) clicked link
    //---------------------------------------------------------------------------
    void translateClick(const QUrl& link)
    {
        QUrlEx linkEx(link);
        emit anchorClickedEx(linkEx);
    }

signals:
    void anchorClickedEx(const QUrlEx & link);
};

#endif // __20150619_QTEXTBROWSEREX_H
