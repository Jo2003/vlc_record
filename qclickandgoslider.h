/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qclickandgoslider.h
 *
 *  @author   Jo2003, inspired by Ralf Neubersch
 *
 *  @date     16.02.2012
 *
 *  $Id$
 *
 *///------------------------- (c) 2012 by Jo2003  --------------------------
#ifndef __20120216_QCLICKANDGOSLIDER_H
   #define __20120216_QCLICKANDGOSLIDER_H

#include <QSlider>
#include <QMouseEvent>

//---------------------------------------------------------------------------
//! \class   QClickAndGoSlider
//! \date    16.02.2012
//! \author  Ralf Neubersch / Jo2003
//! \brief   a custom slider with support of click'n'go
//---------------------------------------------------------------------------
class QClickAndGoSlider : public QSlider
{
   Q_OBJECT

public:
   QClickAndGoSlider ( QWidget * parent = 0 )
      : QSlider(parent)
   {
      // nothing to do ...
   }

   QClickAndGoSlider ( Qt::Orientation orientation, QWidget * parent = 0 )
      : QSlider(orientation, parent)
   {
      // nothing to do ...
   }

   virtual ~QClickAndGoSlider ()
   {
      // nothing to do ...
   }

protected:

   //---------------------------------------------------------------------------
   //
   //! \brief   override QWidget::mousePressEvent()
   //
   //! \author  Ralf Neubersch / Jo2003
   //! \date    16.02.2012
   //
   //! \param   event pointer to mouse event
   //
   //! \return  --
   //---------------------------------------------------------------------------
   virtual void mousePressEvent (QMouseEvent * event)
   {
      // left click ...
      if (event->button() == Qt::LeftButton)
      {
         int pos;
         int range = maximum() - minimum();

         if (orientation() == Qt::Vertical)
         {
            pos = minimum() + (range * (height() - event->y())) / height();
         }
         else
         {
            pos = minimum() + (range * event->x()) / width();
         }

         // make sure pos is in between minimum and maximum ...
         pos = (pos < minimum()) ? minimum() : ((pos > maximum()) ? maximum() : pos);

         // check if position is different from slider ...
         /// Note: We must use a practical threshold value here.
         /// So we use the range which should be handled by
         /// the slider / 80 so we must not click at the 100%
         /// right position to get the "old" normal
         /// slider behavior.
         if (abs(value() - pos) > (range / 80))
         {
            setValue(pos);
            emit sigClickNGo(pos);
            event->accept();

            // no further handling needed ...
            return;
         }
      }

      // not handled --> delegate ...
      event->ignore();
      QSlider::mousePressEvent(event);
   }

signals:
   // special signal so we know for sure how to handle...
   void sigClickNGo(int newPos);
};

#endif // __20120216_QCLICKANDGOSLIDER_H
