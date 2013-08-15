/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     chtmlwriter.h
 *
 *  @author   Jo2003
 *
 *  @date     09.08.2013
 *
 *  $Id$
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#ifndef __20130809_CHTMLWRITER_H
   #define __20130809_CHTMLWRITER_H

#include <QObject>
#include <QString>

//---------------------------------------------------------------------------
//! \class   CHtmlWriter
//! \date    13.08.2013
//! \author  Jo2003
//! \brief   a class to generate html code
//! This class doesn't use the percent placeholder (%) in strings at all
//! since it leads to problems with percent encoded URLs!
//---------------------------------------------------------------------------
class CHtmlWriter : public QObject
{
   Q_OBJECT
public:
   explicit CHtmlWriter(QObject *parent = 0);
   ~CHtmlWriter();

   QString htmlPage (const QString& content, const QString& title = QString(), const QString& css = QString());
   QString tableCell (const QString &content, const QString& style = QString(), int colspan = 1, const QString& align = QString(), const QString& valign = QString());
   QString span (const QString &content, const QString& style = QString());
   QString paragraph(const QString &content, const QString& style = QString());
   QString htmlTag(const QString& tag, const QString &content, const QString& style = QString(), const QString& attr = QString());
   QString simpleTag(const QString& tag, const QString &content, const QString& style = QString());
   QString table(const QString &content, const QString& style = QString());
   QString tableRow(const QString &content, const QString& style = QString());
   QString link(const QString& href, const QString& content, const QString& title = QString(), const QString &style = QString());
   QString image(const QString& src, int width = 0, int height = 0, const QString& style = QString(), const QString& title = QString());
   QString tableHead(const QString &content, const QString& style = QString(), int colspan = 1, const QString& align = QString(), const QString& valign = QString());
   QString div(const QString &content, const QString& style = QString(), const QString& align = QString());
   QString oneCellPage (const QString& content, const QString& style = QString());
   QString createTooltip (const QString& name, const QString& prog, uint start, uint end, const QString& style = QString());
};

#endif // __20130809_CHTMLWRITER_H
