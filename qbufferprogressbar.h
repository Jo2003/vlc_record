/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qbufferprogressbar.h
 *
 *  @author   Jo2003
 *
 *  @date     19.04.2013
 *
 *  $Id$
 *
 *///------------------------- (c) 2012 by Jo2003  --------------------------
#ifndef __20130419_QBUFFERPROGRESSBAR_H
   #define __20130419_QBUFFERPROGRESSBAR_H

#include <QProgressBar>
#include <QColor>
#include <QRect>
#include <QPainter>
#include <QPen>
#include <QBrush>

//---------------------------------------------------------------------------
//! \class   QBufferProgressBar
//! \date    19.04.2013
//! \author  Jo2003
//! \brief   a custom progress bar changing its color from red to green
//!          minimum must be 0, maximum 100!!
//---------------------------------------------------------------------------
class QBufferProgressBar : public QProgressBar
{
   Q_OBJECT

public:

   //---------------------------------------------------------------------------
   //
   //! \brief   construct widget, init values
   //
   //! \author  Jo2003
   //! \date    19.04.2013
   //
   //! \param   parent (QWidget *) optional pointer to parent widget
   //
   //! \return  --
   //---------------------------------------------------------------------------
   QBufferProgressBar(QWidget * parent = 0) : QProgressBar(parent)
   {

   }

   //---------------------------------------------------------------------------
   //
   //! \brief   compute color from given position (red ... yellow ... green)
   //
   //! \author  Jo2003
   //! \date    19.04.2013
   //
   //! \param   iPos (int) position value 0 ... 100
   //
   //! \return  QRgb color
   //---------------------------------------------------------------------------
   QRgb getPosColor(int iPos)
   {
      int r = 0, g = 0;

      if ((iPos >= 0) && (iPos <= 50))
      {
         r = 255;
         g = iPos * 255 / 50;
      }
      else if ((iPos > 50) && (iPos <= 100))
      {
         r = 255 - ((iPos - 50) * 255 / 50);
         g = 255;
      }

      return qRgb(r, g, 0);
   }

   //---------------------------------------------------------------------------
   //
   //! \brief   repaint progress bar
   //
   //! \author  Jo2003
   //! \date    19.04.2013
   //
   //! \param   pe (QPaintEvent *) pointer to paint event (unused)
   //
   //! \return  --
   //---------------------------------------------------------------------------
   virtual void paintEvent(QPaintEvent *pe)
   {
      Q_UNUSED(pe);

      int x = 0, y = 0, h = 0, w = 0;

      QRect    progRect(0, 0, width(), height());
      QPainter painter(this);

      painter.setRenderHint(QPainter::Antialiasing);

      // prepare pen for border line ...
      QPen pen(Qt::SolidLine);
      pen.setColor(Qt::white);
      pen.setWidth(2);
      painter.setPen(pen);

      if (orientation() == Qt::Horizontal)
      {
         // height should be 14px ...
         progRect.setHeight(14);
         progRect.setY((height() - 14) / 2);

         // compute size of inner rect (take care of border) ...
         h =  progRect.height() - pen.width();
         w = (progRect.width()  - pen.width()) * value() / 100;
         x =  pen.width() / 2;
         y =  progRect.y() + (pen.width() / 2);
      }
      else if (orientation() == Qt::Vertical)
      {
         // width should be 12px ...
         progRect.setWidth(12);
         progRect.setX(((width() - 12) / 2));

         // compute size of inner rect (take care of border) ...
         h = -((progRect.height() - pen.width()) * value() / 100);
         w =  progRect.width()  - pen.width();
         x =  progRect.x() + pen.width() / 2;
         y =  progRect.height() - (pen.width() / 2);
      }

      // make sure background is white ...
      painter.fillRect(progRect, QColor("#333"));

      // draw outline ...
      painter.drawRoundedRect(progRect, 3, 3);

      painter.fillRect(QRect(x, y, w, h), QColor(getPosColor(value())));
   }
};

#endif // __20130419_QBUFFERPROGRESSBAR_H
