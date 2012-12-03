/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qcomboboxex.h
 *
 *  @author   Jo2003
 *
 *  @date     03.12.2012
 *
 *  $Id$
 *
 *///------------------------- (c) 2012 by Jo2003  --------------------------
#ifndef __20121203_QCOMBOBOXEX_H
   #define __20121203_QCOMBOBOXEX_H

#include <QComboBox>

//---------------------------------------------------------------------------
//! \class   QComboBoxEx
//! \date    03.12.2012
//! \author  Jo2003
//! \brief   an extended QComboBox class which tells about popup state
//---------------------------------------------------------------------------
class QComboBoxEx : public QComboBox
{
   Q_OBJECT

public:

   //---------------------------------------------------------------------------
   //
   //! \brief   construct QComboBoxEx object
   //
   //! \author  Jo2003
   //! \date    03.12.2012
   //
   //! \param   parent pointer to parent widget
   //
   //! \return  --
   //---------------------------------------------------------------------------
   explicit QComboBoxEx (QWidget *parent = 0) : QComboBox(parent)
   {
      // nothing to do so far ...
   }

   //---------------------------------------------------------------------------
   //
   //! \brief   called when popup should be shown, emit showPopup signal
   //
   //! \author  Jo2003
   //! \date    03.12.2012
   //
   //! \param   --
   //
   //! \return  --
   //---------------------------------------------------------------------------
   virtual void showPopup()
   {
      QComboBox::showPopup();
      emit sigShownPopup();
   }

   //---------------------------------------------------------------------------
   //
   //! \brief   called when popup should be hidden, emit hidePopup signal
   //
   //! \author  Jo2003
   //! \date    03.12.2012
   //
   //! \param   --
   //
   //! \return  --
   //---------------------------------------------------------------------------
   virtual void hidePopup()
   {
      QComboBox::hidePopup();
      emit sigHidePopup();
   }

signals:

   //---------------------------------------------------------------------------
   //
   //! \brief   tells about that popup is shown
   //
   //! \author  Jo2003
   //! \date    03.12.2012
   //
   //! \param   --
   //
   //! \return  --
   //---------------------------------------------------------------------------
   void sigShownPopup();

   //---------------------------------------------------------------------------
   //
   //! \brief   tells about that popup is hidden
   //
   //! \author  Jo2003
   //! \date    03.12.2012
   //
   //! \param   --
   //
   //! \return  --
   //---------------------------------------------------------------------------
   void sigHidePopup();
};

#endif // __20121203_QCOMBOBOXEX_H
