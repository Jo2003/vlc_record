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
#include <QToolTip>
#include <QTime>

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
      : QSlider(parent), _isVideo(false)
   {
      init();
   }

   QClickAndGoSlider ( Qt::Orientation orientation, QWidget * parent = 0 )
      : QSlider(orientation, parent), _isVideo(false)
   {
      init();
   }

   virtual ~QClickAndGoSlider ()
   {
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

   //---------------------------------------------------------------------------
   //
   //! \brief   set video flag
   //
   //! \author  Jo2003
   //! \date    19.05.2014
   //
   //! \param   val (bool) new value
   //
   //! \return  --
   //---------------------------------------------------------------------------
   void setVideo (bool val)
   {
      _isVideo = val;
   }

protected:

   //---------------------------------------------------------------------------
   //
   //! \brief   set handle range and mouse tracking
   //
   //! \author  Jo2003
   //! \date    19.05.2014
   //
   //! \param   --
   //
   //! \return  --
   //---------------------------------------------------------------------------
   void init ()
   {
      _iHandleRange = 80;
      setMouseTracking(true);
   }

   //---------------------------------------------------------------------------
   //
   //! \brief   show special tooltip with timing information
   //
   //! \author  Jo2003
   //! \date    19.05.2014
   //
   //! \param   event (QMouseEvent *) pointer to mous event
   //
   //! \return  --
   //---------------------------------------------------------------------------
   virtual void mouseMoveEvent(QMouseEvent * event)
   {
      // create additional tooltip when using
      // as video slider ...
      if (_isVideo)
      {
         int pos;
         int range = maximum() - minimum();
         int dif;
         QString tt;

         if (orientation() == Qt::Vertical)
         {
            pos = (range * (height() - event->y())) / height();
         }
         else
         {
            pos = (range * event->x()) / width();
         }

         // get time from slider handle and mouse position ...
         QTime tpos = QTime(0, 0).addSecs(pos);
         QTime npos = QTime(0, 0).addSecs(value() - minimum());

         // remove old tooltip (if exists) ...
         if (!QToolTip::text().isEmpty())
         {
            QToolTip::showText(mapToGlobal(QPoint(0, 0)), "", this);
         }

         // get difference between slider handle and mouse position ...
         dif = npos.secsTo(tpos);


         QTime dtime = QTime(0, 0).addSecs(abs(dif));

         // create tooltip string ...
         tt = QString("%1 %2 -> %3").arg((dif < 0) ? "-" : "+")
               .arg(dtime.toString((abs(dif) < 3600) ? "m:ss" : "H:mm:ss"))
               .arg(tpos.toString("H:mm:ss"));

         // display tooltip ...
         QToolTip::showText(mapToGlobal(event->pos()), tt, this);
      }

      QSlider::mouseMoveEvent(event);
   }

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
         if (abs(value() - pos) > (range / _iHandleRange))
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
   int  _iHandleRange;
   bool _isVideo;

signals:
   // special signal so we know for sure how to handle...
   void sigClickNGo(int newPos);
};

#endif // __20120216_QCLICKANDGOSLIDER_H
