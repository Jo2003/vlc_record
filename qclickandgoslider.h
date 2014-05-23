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
   QClickAndGoSlider ( QWidget * parent = 0 );
   QClickAndGoSlider ( Qt::Orientation orientation, QWidget * parent = 0 );

   virtual ~QClickAndGoSlider ();

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
   virtual void setHandleRangeVal(int i);

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
   void setVideo (bool val);

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
   void init ();

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
   virtual void mouseMoveEvent(QMouseEvent * event);

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
   virtual void mousePressEvent (QMouseEvent * event);

private:
   int  _iHandleRange;
   bool _isVideo;

signals:
   // special signal so we know for sure how to handle...
   void sigClickNGo(int newPos);
};

#endif // __20120216_QCLICKANDGOSLIDER_H
