/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qfusioncontrol.cpp
 *
 *  @author   Jo2003
 *
 *  @date     23.11.2012
 *
 *  $Id$
 *
 *///------------------------- (c) 2012 by Jo2003  --------------------------
#include "qfusioncontrol.h"

/////////////////////////////////////////////////////////////////////////////
/// signal docu
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//! \fn      sigVolChanged [signal]
//! \brief   change volume
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   vol new volume value
//
//! \return  --
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! \fn      sigPlay [signal]
//! \brief   do play
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! \fn      sigStop [signal]
//! \brief   do stop
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! \fn      sigRec [signal]
//! \brief   do record
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! \fn      sigFwd [signal]
//! \brief   jump forward
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! \fn      sigBwd [signal]
//! \brief   jump backward
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! \fn      sigPosClickNGo [signal]
//! \brief   jump to position
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   pos(int) position
//
//! \return  --
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! \fn      sigPossliderReleased [signal]
//! \brief   slider handle was released
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! \fn      sigPosSliderValueChanged [signal]
//! \brief   slider position chnaged
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   pos(int) new position
//
//! \return  --
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
//! \brief   constructs QFusionControl object
//
//! \author  Jo2003
//! \date    23.11.2012
//
//! \param   parent pointer to parent object
//
//! \return  --
//---------------------------------------------------------------------------
QFusionControl::QFusionControl(QObject *parent) :
   QObject(parent)
{
}

//---------------------------------------------------------------------------
//
//! \brief   destroys QFusionControl object
//
//! \author  Jo2003
//! \date    23.11.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
QFusionControl::~QFusionControl()
{
   // so far nothing to do ...
}

//---------------------------------------------------------------------------
//
//! \brief   release connections, delete stored object pointer
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::disconnectCtrls()
{
   disconnectBtn();
   disconnectVol();
   disconnectJmp();
   disconnectCng();
   disconnectLab();
}

/////////////////////////////////////////////////////////////////////////////
/// Button region ...
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//
//! \brief   add button to control
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   pBtn pointer to pushbutton
//! \param   rol rol for button (start, stop, rec, ...)
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::addButton (QPushButton *pBtn, eBtnRol rol)
{
   switch (rol)
   {
   case BTN_PLAY:
      // connect signal --> slots ...
      connect (pBtn, SIGNAL(clicked()), this, SLOT(slotPlay()));

      // add to vector ...
      _playBtnVector.append(pBtn);
      break;

   case BTN_REC:
      // connect signal --> slots ...
      connect (pBtn, SIGNAL(clicked()), this, SLOT(slotRec()));

      // add to vector ...
      _recBtnVector.append(pBtn);
      break;

   case BTN_STOP:
      // connect signal --> slots ...
      connect (pBtn, SIGNAL(clicked()), this, SLOT(slotStop()));

      // add to vector ...
      _stopBtnVector.append(pBtn);
      break;

   case BTN_FWD:
      // connect signal --> slots ...
      connect (pBtn, SIGNAL(clicked()), this, SLOT(slotFwd()));

      // add to vector ...
      _fwdBtnVector.append(pBtn);
      break;

   case BTN_BWD:
      // connect signal --> slots ...
      connect (pBtn, SIGNAL(clicked()), this, SLOT(slotBwd()));

      // add to vector ...
      _bwdBtnVector.append(pBtn);
      break;

   default:
      break;
   }
}

//---------------------------------------------------------------------------
//
//! \brief   set icon to button
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   icon new icon to set
//! \param   rol rol of button (start, stop, rec, ...)
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::btnSetIcon(const QIcon &icon, eBtnRol rol)
{
   QVector<QPushButton *> *pVector = NULL;
   switch (rol)
   {
   case BTN_PLAY:
      pVector = &_playBtnVector;
      break;

   case BTN_REC:
      pVector = &_recBtnVector;
      break;

   case BTN_STOP:
      pVector = &_stopBtnVector;
      break;

   case BTN_FWD:
      pVector = &_fwdBtnVector;
      break;

   case BTN_BWD:
      pVector = &_bwdBtnVector;
      break;

   default:
      break;
   }

   if (pVector)
   {
      for (int i = 0; i < pVector->size(); i++)
      {
         pVector->at(i)->setIcon(icon);
      }
   }
}

//---------------------------------------------------------------------------
//
//! \brief   one of the play buttons was pressed, emit play signal
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::slotPlay ()
{
   emit sigPlay();
}

//---------------------------------------------------------------------------
//
//! \brief   one of the rec buttons was pressed, emit record signal
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::slotRec ()
{
   emit sigRec();
}

//---------------------------------------------------------------------------
//
//! \brief   one of the stop buttons was pressed, emit stop signal
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::slotStop ()
{
   emit sigStop();
}

//---------------------------------------------------------------------------
//
//! \brief   one of the jump fwd buttons was pressed, emit forward signal
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::slotFwd ()
{
   emit sigFwd();
}

//---------------------------------------------------------------------------
//
//! \brief   one of the jump bwd buttons was pressed, emit backward signal
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::slotBwd ()
{
   emit sigBwd();
}

//---------------------------------------------------------------------------
//
//! \brief   enable / disable buttons with given rol
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   enable enable / disable flag
//! \param   rol button rol
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::enableBtn (bool enable, eBtnRol rol)
{
   QVector<QPushButton *> *pVector = NULL;
   switch (rol)
   {
   case BTN_PLAY:
      pVector = &_playBtnVector;
      break;

   case BTN_REC:
      pVector = &_recBtnVector;
      break;

   case BTN_STOP:
      pVector = &_stopBtnVector;
      break;

   case BTN_FWD:
      pVector = &_fwdBtnVector;
      break;

   case BTN_BWD:
      pVector = &_bwdBtnVector;
      break;

   default:
      break;
   }

   if (pVector)
   {
      for (int i = 0; i < pVector->size(); i++)
      {
         pVector->at(i)->setEnabled(enable);
      }
   }
}

//---------------------------------------------------------------------------
//
//! \brief   release connection to buttons and remove stored button pointer
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::disconnectBtn()
{
   disconnect(this, SLOT(slotPlay()));
   _playBtnVector.clear();

   disconnect(this, SLOT(slotRec()));
   _recBtnVector.clear();

   disconnect(this, SLOT(slotStop()));
   _stopBtnVector.clear();

   disconnect(this, SLOT(slotFwd()));
   _fwdBtnVector.clear();

   disconnect(this, SLOT(slotBwd()));
   _bwdBtnVector.clear();
}

/////////////////////////////////////////////////////////////////////////////
/// volume slider region ...
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//
//! \brief   put volume slider under control, make connections
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   pSli poibter to volume slider
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::addVolSlider (QSlider *pSli)
{
   connect (pSli, SIGNAL(sliderMoved(int)), this, SLOT(slotVolSliderMoved(int)));
   _volSliderVector.append(pSli);
}

//---------------------------------------------------------------------------
//
//! \brief   volume was changed, emit volchange signal
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   vol new volume value
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::slotVolSliderMoved(int vol)
{
   emit sigVolSliderMoved(vol);

   // make sure all slider have same value now ...
   setVolSliderPosition(vol);
}

//---------------------------------------------------------------------------
//
//! \brief   set volume slider position to all sliders
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   vol new volume value
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::setVolSliderPosition(int vol)
{
   for (int i = 0; i < _volSliderVector.count(); i++)
   {
      _volSliderVector.at(i)->setSliderPosition(vol);
   }
}

//---------------------------------------------------------------------------
//
//! \brief   set volume value to all sliders
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   vol new volume value
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::setVolume(int vol)
{
   for (int i = 0; i < _volSliderVector.count(); i++)
   {
      _volSliderVector.at(i)->setValue(vol);
   }
}

//---------------------------------------------------------------------------
//
//! \brief   get volume value
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   --
//
//! \return  volume value
//---------------------------------------------------------------------------
int QFusionControl::getVolume()
{
   if (_volSliderVector.size())
   {
      // slider are in sync ...
      return _volSliderVector.at(0)->value();
   }
   return -1;
}

//---------------------------------------------------------------------------
//
//! \brief   get minimum of first volume slider (they all are in sync)
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   --
//
//! \return  minimum value
//---------------------------------------------------------------------------
int  QFusionControl::volMinimum ()
{
   if (_volSliderVector.size())
   {
      // values are synchronized ...
      return _volSliderVector.at(0)->minimum();
   }
   return -1;
}

//---------------------------------------------------------------------------
//
//! \brief   get maximum of first volume slider (they all are in sync)
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   --
//
//! \return  maximum value
//---------------------------------------------------------------------------
int  QFusionControl::volMaximum ()
{
   if (_volSliderVector.size())
   {
      // values are synchronized ...
      return _volSliderVector.at(0)->maximum();
   }
   return -1;
}

//---------------------------------------------------------------------------
//
//! \brief   release connection to vol slider, delete pointer to sliders
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::disconnectVol()
{
   disconnect(this, SLOT(slotVolChanged(int)));
   _volSliderVector.clear();
}

/////////////////////////////////////////////////////////////////////////////
/// jump box region ...
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//
//! \brief   add jump combobox, make connections
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   pBox pointer to combobox
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::addJumpBox (QComboBox *pBox)
{
   connect (pBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotJumpValChanged(int)));
   _jumpCbxVector.append(pBox);
}

//---------------------------------------------------------------------------
//
//! \brief   jump value was changed, synchronize sliders
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   idx new index value
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::slotJumpValChanged(int idx)
{
   for (int i = 0; i < _jumpCbxVector.size(); i ++)
   {
      _jumpCbxVector.at(i)->setCurrentIndex(idx);
   }
}

//---------------------------------------------------------------------------
//
//! \brief   get curent value from 1st combobox (note: all are in sync)
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   --
//
//! \return  current jump value
//---------------------------------------------------------------------------
int QFusionControl::getJumpValue()
{
   bool ok  = false;
   int  iRV = -1;

   if (_jumpCbxVector.size())
   {
      // all jump boxes should be synchronized ...
      iRV = _jumpCbxVector.at(0)->currentText().toInt(&ok);
   }

   return (ok ? iRV : -1);
}

//---------------------------------------------------------------------------
//
//! \brief   enable / disable jump cbx
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   enable enable / disable flag
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::enableJumpBox(bool enable)
{
   for (int i = 0; i < _jumpCbxVector.size(); i ++)
   {
      _jumpCbxVector.at(i)->setEnabled(enable);
   }
}

//---------------------------------------------------------------------------
//
//! \brief   release jump box connections, delete object pointer
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::disconnectJmp()
{
   disconnect(this, SLOT(slotJumpValChanged(int)));
   _jumpCbxVector.clear();
}

/////////////////////////////////////////////////////////////////////////////
/// Click'n'Go slider region ...
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//
//! \brief   add position slider, make connections
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   pSli pointer to QClickAndGoSlider
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::addCngSlider (QClickAndGoSlider *pSli)
{
   connect (pSli, SIGNAL(sigClickNGo(int)), this, SLOT(slotClickNGo(int)));
   connect (pSli, SIGNAL(sliderReleased()), this, SLOT(slotPosSliderReleased()));
   connect (pSli, SIGNAL(valueChanged(int)), this, SLOT(slotPosSliderValueChanged(int)));
   _cngSliderVector.append(pSli);
}

//---------------------------------------------------------------------------
//
//! \brief   click'n' on one slider, emit signal
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   pos new position
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::slotClickNGo(int pos)
{
   emit sigPosClickNGo(pos);
   setPosValue(pos);
}

//---------------------------------------------------------------------------
//
//! \brief   click'n'go slider value changed, emit signal, make sync
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   pos new position
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::slotPosSliderValueChanged(int pos)
{
   emit sigPosSliderValueChanged(pos);
   setPosValue(pos);
}

//---------------------------------------------------------------------------
//
//! \brief   slider handle of one slider was released, emit signal
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::slotPosSliderReleased ()
{
   emit sigPosSliderReleased();
}

//---------------------------------------------------------------------------
//
//! \brief   check if one slider handle is down
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   --
//
//! \return  true -> one is down, false -> no one down
//---------------------------------------------------------------------------
bool QFusionControl::isPosSliderDown ()
{
   bool bIsDown = false;
   for (int i = 0; ((i < _cngSliderVector.size()) && !bIsDown); i++)
   {
      bIsDown = _cngSliderVector.at(i)->isSliderDown();
   }

   return bIsDown;
}

//---------------------------------------------------------------------------
//
//! \brief   set new position on position slider
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   pos new position
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::setPosValue (int pos)
{
   for (int i = 0; i < _cngSliderVector.size(); i++)
   {
      _cngSliderVector.at(i)->setValue(pos);
   }
}

//---------------------------------------------------------------------------
//
//! \brief   get minimum of first position slider (they all are in sync)
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   --
//
//! \return  minimum value
//---------------------------------------------------------------------------
int  QFusionControl::posMinimum ()
{
   if (_cngSliderVector.size())
   {
      // values are synchronized ...
      return _cngSliderVector.at(0)->minimum();
   }
   return -1;
}

//---------------------------------------------------------------------------
//
//! \brief   get maximum of first position slider (they all are in sync)
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   --
//
//! \return  maximum value
//---------------------------------------------------------------------------
int  QFusionControl::posMaximum ()
{
   if (_cngSliderVector.size())
   {
      // values are synchronized ...
      return _cngSliderVector.at(0)->maximum();
   }
   return -1;
}

//---------------------------------------------------------------------------
//
//! \brief   emable / disable position slider
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   enable enable / disable flag
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::enablePosSlider(bool enable)
{
   for (int i = 0; i < _cngSliderVector.size(); i++)
   {
      _cngSliderVector.at(i)->setEnabled(enable);
   }
}

//---------------------------------------------------------------------------
//
//! \brief   set range for position slider
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   min min value
//! \param   nax max value
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::setPosRange (int min, int max)
{
   for (int i = 0; i < _cngSliderVector.size(); i++)
   {
      _cngSliderVector.at(i)->setRange(min, max);
   }
}

//---------------------------------------------------------------------------
//
//! \brief   get position of first position slider (they're in sync)
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   --
//
//! \return  current position
//---------------------------------------------------------------------------
int  QFusionControl::posValue ()
{
   if (_cngSliderVector.size())
   {
      // values are synchronized ...
      return _cngSliderVector.at(0)->value();
   }
   return -1;
}

//---------------------------------------------------------------------------
//
//! \brief   release connection to CNG slider, remove object pointers
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::disconnectCng()
{
   disconnect(this, SLOT(slotClickNGo()));
   disconnect(this, SLOT(slotPosSliderReleased()));
   disconnect(this, SLOT(slotPosSliderValueChanged(int)));
   _cngSliderVector.clear();
}

/////////////////////////////////////////////////////////////////////////////
/// time label region ...
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//
//! \brief   put time label under control
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   pLab pointer to QLabel
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::addTimeLab (QTimeLabel *pLab)
{
   _timeLabVector.append(pLab);
}

//---------------------------------------------------------------------------
//
//! \brief   set time string to all time labels
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   time new time value
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::setTime(quint64 time)
{
   for (int i = 0; _timeLabVector.size(); i++)
   {
      _timeLabVector.at(i)->setTime(time);
   }
}

//---------------------------------------------------------------------------
//
//! \brief   remove label pointer
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::disconnectLab()
{
   _timeLabVector.clear();
}

