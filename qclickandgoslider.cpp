/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qclickandgoslider.cpp
 *
 *  @author   Jo2003, inspired by Ralf Neubersch
 *
 *  @date     23.05.2014
 *
 *  $Id$
 *
 *///------------------------- (c) 2012 by Jo2003  --------------------------
#include "qclickandgoslider.h"

#include "templates.h"
#include "externals_inc.h"

//---------------------------------------------------------------------------
//
//! \brief   construct click'n'go slider
//
//! \author  Jo2003
//! \date    23.05.2014
//
//! \param   parent (QWidget *) pointer to parent widget
//
//! \return  --
//---------------------------------------------------------------------------
QClickAndGoSlider::QClickAndGoSlider ( QWidget * parent )
   : QSlider(parent), _isVideo(false)
{
   init();
}

//---------------------------------------------------------------------------
//
//! \brief   construct click'n'go slider
//
//! \author  Jo2003
//! \date    23.05.2014
//
//! \param   orientation (Qt::Orientation) horizontal or vertical
//! \param   parent (QWidget *) pointer to parent widget
//
//! \return  --
//---------------------------------------------------------------------------
QClickAndGoSlider::QClickAndGoSlider ( Qt::Orientation orientation, QWidget * parent )
   : QSlider(orientation, parent), _isVideo(false)
{
   init();
}

//---------------------------------------------------------------------------
//
//! \brief   destroy click'n'go slider
//
//! \author  Jo2003
//! \date    23.05.2014
//
//---------------------------------------------------------------------------
QClickAndGoSlider::~QClickAndGoSlider ()
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
void QClickAndGoSlider::setHandleRangeVal(int i)
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
void QClickAndGoSlider::setVideo (bool val)
{
   _isVideo = val;
}

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
void QClickAndGoSlider::init ()
{
   _iHandleRange    = 80;
   _clickedPosition = 0;
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
void QClickAndGoSlider::mouseMoveEvent(QMouseEvent * event)
{
   int pos;
   int range = maximum() - minimum();
   int dif;
   QString tt;

   // create additional tooltip when using
   // as video slider ...
   if (_isVideo && (event->buttons() == Qt::NoButton))
   {
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
      if (QToolTip::isVisible())
      {
         QToolTip::hideText();
      }

      // get difference between slider handle and mouse position ...
      dif = npos.secsTo(tpos);


      QTime dtime = QTime(0, 0).addSecs(abs(dif));

      // create tooltip string ...
      tt = pHtml->htmlTag("p", QString("%1 %2 -> %3").arg((dif < 0) ? "-" : "+")
                              .arg(dtime.toString((abs(dif) < 3600) ? "m:ss" : "H:mm:ss"))
                              .arg(tpos.toString("H:mm:ss")), "font-size: 14px; white-space: pre;");

      // display tooltip ...
      QToolTip::showText(mapToGlobal(event->pos()), tt, this);
   }
   else if(_isVideo && (event->buttons() == Qt::LeftButton))
   {
      // get time from slider handle and mouse position ...
      dif         = value() - _clickedPosition;
      QTime tdif  = QTime(0, 0).addSecs(value() - minimum());
      QTime dtime = QTime(0, 0).addSecs(abs(dif));

      // create tooltip string ...
      tt = pHtml->htmlTag("p", QString("%1 %2 -> %3").arg((dif < 0) ? "-" : "+")
                              .arg(dtime.toString((abs(dif) < 3600) ? "m:ss" : "H:mm:ss"))
                              .arg(tdif.toString("H:mm:ss")), "font-size: 14px; white-space: pre;");

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
void QClickAndGoSlider::mousePressEvent (QMouseEvent * event)
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
      else
      {
         // on slider handle ... store position ...
         _clickedPosition = value();
      }
   }

   // not handled --> delegate ...
   event->ignore();
   QSlider::mousePressEvent(event);
}
