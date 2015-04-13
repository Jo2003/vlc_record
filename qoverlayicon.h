/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/qoverlayicon.h $
 *
 *  @file     qoverlayicon.h
 *
 *  @author   Jo2003
 *
 *  @date     21.08.2013
 *
 *  $Id: qoverlayicon.h 1184 2013-08-21 12:00:45Z Olenka.Joerg $
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#ifndef __20130821_QOVERLAYICON_H
   #define __20130821_QOVERLAYICON_H

#include <QIcon>
#include <QPixmap>

//---------------------------------------------------------------------------
//! \class   QOverlayIcon
//! \date    20.08.2013
//! \author  Jo2003
//! \brief   an icon where we can add an overlay string
//---------------------------------------------------------------------------
class QOverlayIcon : public QIcon
{
public:
   explicit QOverlayIcon ();
   QOverlayIcon(const QPixmap & pixmap);
   QOverlayIcon(const QIcon & other);
   QOverlayIcon(const QString & fileName);
   QOverlayIcon(QIconEngine * engine);
   QOverlayIcon(QIconEngineV2 * engine);

   ~QOverlayIcon();

   void placeText(const QString& str, const QColor& color = QColor("black"));
   void placeInt(int i, const QColor& color = QColor("black"));

protected:
   void takeBackup ();

private:
   QPixmap _bu;
};

#endif // __20130821_QOVERLAYICON_H
