/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/qclickandgoslider.h $
 *
 *  @file     qclickandgoslider.h
 *
 *  @author   Jo2003, inspired by Ralf Neubersch
 *
 *  @date     16.02.2012
 *
 *  $Id: qclickandgoslider.h 1216 2013-11-11 15:06:57Z Olenka.Joerg $
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
      _iHandleRange = 80;
   }

   QClickAndGoSlider ( Qt::Orientation orientation, QWidget * parent = 0 )
      : QSlider(orientation, parent)
   {
      _iHandleRange = 80;
   }

   virtual ~QClickAndGoSlider ()
   {
      _iHandleRange = 80;
   }

   //---------------------------------------------------------------------------
   //
   //! \brief   modify the range around the slider handle so it doesn't catch
   //!          click events.
   //!          std: 80. Larger values will reduce the range, lower will raise
   //
   //! \author  Jo2003
   //! \date    11.11.2013
   //
   //! \param   --
   //
   //! \return  --
   //---------------------------------------------------------------------------
   virtual void setHandleRangeVal(int i)
   {
      _iHandleRange = i;
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
         /// the slider / _iHandleRange so we must not click at the 100%
         /// right position to get the "old" normal
         /// slider behavior.
         if (std::abs((int)(value() - pos)) > (range / _iHandleRange))
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

private:
   int _iHandleRange;

signals:
   // special signal so we know for sure how to handle...
   void sigClickNGo(int newPos);
};

#endif // __20120216_QCLICKANDGOSLIDER_H
