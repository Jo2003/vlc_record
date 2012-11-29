/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qmovehandle.h
 *
 *  @author   Jo2003
 *
 *  @date     29.11.2012
 *
 *  $Id$
 *
 *///------------------------- (c) 2012 by Jo2003  --------------------------
#ifndef __20121129_QMOVEHANDLE_H
   #define __20121129_QMOVEHANDLE_H

#include <QLabel>
#include <QEvent>

//---------------------------------------------------------------------------
//! \class   QMoveHandle
//! \date    29.11.2012
//! \author  Jo2003
//! \brief   a special label which sends signals on mouse enter / leave
//---------------------------------------------------------------------------
class QMoveHandle : public QLabel
{
   Q_OBJECT

public:

   //---------------------------------------------------------------------------
   //
   //! \brief   constructs QMoveHandl eobject
   //
   //! \author  Jo2003
   //! \date    29.11.2012
   //
   //! \param   parent pointer to parent widget
   //! \param   f window flags
   //
   //! \return  --
   //---------------------------------------------------------------------------
   explicit QMoveHandle(QWidget *parent = 0, Qt::WindowFlags f = 0) : QLabel(parent, f)
   {
      // nothing to do ...
   }

protected:

   //---------------------------------------------------------------------------
   //
   //! \brief   mouse enters widget -> emit signal
   //
   //! \author  Jo2003
   //! \date    29.11.2012
   //
   //! \param   e pointer to enter event
   //
   //! \return  --
   //---------------------------------------------------------------------------
   virtual void enterEvent(QEvent *e)
   {
      emit mouseEnters();
      QLabel::enterEvent(e);
   }

   //---------------------------------------------------------------------------
   //
   //! \brief   mouse leaves widget -> emit signal
   //
   //! \author  Jo2003
   //! \date    29.11.2012
   //
   //! \param   e pointer to leave event
   //
   //! \return  --
   //---------------------------------------------------------------------------
   virtual void leaveEvent(QEvent *e)
   {
      emit mouseLeabes();
      QLabel::leaveEvent(e);
   }

signals:
   //---------------------------------------------------------------------------
   //
   //! \brief   mouse enter widget signal
   //
   //! \author  Jo2003
   //! \date    29.11.2012
   //
   //! \param   --
   //
   //! \return  --
   //---------------------------------------------------------------------------
   void mouseEnters();

   //---------------------------------------------------------------------------
   //
   //! \brief   mouse leave widget signal
   //
   //! \author  Jo2003
   //! \date    29.11.2012
   //
   //! \param   --
   //
   //! \return  --
   //---------------------------------------------------------------------------
   void mouseLeabes();
};



#endif // __20121129_QMOVEHANDLE_H
