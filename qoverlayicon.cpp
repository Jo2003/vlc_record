/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qoverlayicon.cpp
 *
 *  @author   Jo2003
 *
 *  @date     21.08.2013
 *
 *  $Id$
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#include "qoverlayicon.h"
#include <QPainter>
#include <QFont>
#include <QFontMetrics>

//---------------------------------------------------------------------------
//
//! \brief   constructs QOverlayIcon object
//
//! \author  Jo2003
//! \date    21.08.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
QOverlayIcon::QOverlayIcon() :
   QIcon()
{
   // nothing to do so far ...
}

//---------------------------------------------------------------------------
//
//! \brief   constructs QOverlayIcon object
//
//! \author  Jo2003
//! \date    21.08.2013
//
//! \param   pixmap (const QPixmap &) icon source
//
//! \return  --
//---------------------------------------------------------------------------
QOverlayIcon::QOverlayIcon(const QPixmap & pixmap) : QIcon(pixmap)
{
   takeBackup();
}

//---------------------------------------------------------------------------
//
//! \brief   constructs QOverlayIcon object
//
//! \author  Jo2003
//! \date    21.08.2013
//
//! \param   other (const QIcon &) icon source
//
//! \return  --
//---------------------------------------------------------------------------
QOverlayIcon::QOverlayIcon(const QIcon & other) : QIcon(other)
{
   takeBackup();
}

//---------------------------------------------------------------------------
//
//! \brief   constructs QOverlayIcon object
//
//! \author  Jo2003
//! \date    21.08.2013
//
//! \param   fileName (const QString &) icon source
//
//! \return  --
//---------------------------------------------------------------------------
QOverlayIcon::QOverlayIcon(const QString & fileName) : QIcon(fileName)
{
   takeBackup();
}

//---------------------------------------------------------------------------
//
//! \brief   constructs QOverlayIcon object
//
//! \author  Jo2003
//! \date    21.08.2013
//
//! \param   engine (QIconEngine *) icon source
//
//! \return  --
//---------------------------------------------------------------------------
QOverlayIcon::QOverlayIcon(QIconEngine * engine) : QIcon(engine)
{
   takeBackup();
}

//---------------------------------------------------------------------------
//
//! \brief   destroys QOverlayIcon object
//
//! \author  Jo2003
//! \date    21.08.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
QOverlayIcon::~QOverlayIcon()
{
   // nothing to do so far ...
}

//---------------------------------------------------------------------------
//
//! \brief   store icon pixmap in max. size internally
//
//! \author  Jo2003
//! \date    21.08.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QOverlayIcon::takeBackup()
{
   QList<QSize> lsz = availableSizes();
   QSize        sz;

   // get the largest version ...
   for (int i = 0; i < lsz.count(); i++)
   {
      if (lsz.at(i).width() > sz.width())
      {
         sz = lsz.at(i);
      }
   }

   // store in max. size ...
   _bu = pixmap(sz);
}

//---------------------------------------------------------------------------
//
//! \brief   place text as overlay on icon taking care on text size
//
//! \author  Jo2003
//! \date    21.08.2013
//
//! \param   str (const QString &) text to place
//! \param   color (const QColor &) color for text
//
//! \return  --
//---------------------------------------------------------------------------
void QOverlayIcon::placeText(const QString &str, const QColor &color)
{
   QSize    sz = _bu.size();
   QPixmap  px = _bu;
   QPainter pt(&px);
   QFont    ft = pt.font();

   ft.setBold(true);

   // resize font so it matches current icon size ...
   if (QFontMetrics(ft).height() > (sz.height() / 2))
   {
      do
      {
         ft.setPointSize(ft.pointSize() - 1);

         // make sure we don't loop here ...
         if (ft.pointSize() <= 0)
         {
            break;
         }

      } while (QFontMetrics(ft).height() > (sz.height() / 2));
   }
   else if (QFontMetrics(ft).height() < (sz.height() / 2))
   {
      do
      {
         ft.setPointSize(ft.pointSize() + 1);

      } while (QFontMetrics(ft).height() < (sz.height() / 2));
   }

   // set font and pen (color) to painter ...
   pt.setFont(ft);
   pt.setPen(color);

   // place text at lower right corner ...
   pt.drawText(px.rect(), Qt::AlignRight | Qt::AlignBottom, str);

   // use new created pixmap as icon ...
   addPixmap(px);
}

//---------------------------------------------------------------------------
//
//! \brief   place an int as overlay on icon taking care on text size
//
//! \author  Jo2003
//! \date    21.08.2013
//
//! \param   i (int) number to place
//! \param   color (const QColor &) color for text
//
//! \return  --
//---------------------------------------------------------------------------
void QOverlayIcon::placeInt(int i, const QColor &color)
{
   placeText(QString::number(i), color);
}
