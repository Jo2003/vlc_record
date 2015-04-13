/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/sunduk.tv/qfusioncontrol.cpp $
 *
 *  @file     qfusioncontrol.cpp
 *
 *  @author   Jo2003
 *
 *  @date     23.11.2012
 *
 *  $Id: qfusioncontrol.cpp 1224 2013-11-26 11:01:15Z Olenka.Joerg $
 *
 *///------------------------- (c) 2012 by Jo2003  --------------------------
#include "qfusioncontrol.h"
#include "clogfile.h"
#include "small_helpers.h"

extern CLogFile VlcLog;

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
//! \fn      sigFullScreen [signal]
//! \brief   fullscreen button clicked
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! \fn      sigScrShot [signal]
//! \brief   screenshot button clicked
//
//! \author  Jo2003
//! \date    13.11.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! \fn      sigEnterWndwd [signal]
//! \brief   enter windowed mode button clicked
//
//! \author  Jo2003
//! \date    26.11.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! \fn      sigSaveVideoFormat [signal]
//! \brief   save video format button clicked
//
//! \author  Jo2003
//! \date    03.12.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! \fn      sigAspectCurrentIndexChanged [signal]
//! \brief   curent index of aspect cbx changed
//
//! \author  Jo2003
//! \date    03.12.2012
//
//! \param   pos(int) new index
//
//! \return  --
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! \fn      sigCropCurrentIndexChanged [signal]
//! \brief   curent index of crop cbx changed
//
//! \author  Jo2003
//! \date    03.12.2012
//
//! \param   pos(int) new index
//
//! \return  --
//---------------------------------------------------------------------------

/////////////////////////////////////////////////////////////////////////////
/// functions docu
/////////////////////////////////////////////////////////////////////////////

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
   QObject(parent), _iPopup(0)
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
   disconnectCng();
   disconnectLab();
   disconnectCbx();
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
//! \param   role role for button (start, stop, rec, ...)
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::addButton (QPushButton *pBtn, eBtnRole role)
{
   switch (role)
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

   case BTN_FS:
      // connect signal --> slots ...
      connect (pBtn, SIGNAL(clicked()), this, SLOT(slotFs()));

      // add to vector ...
      _fsBtnVector.append(pBtn);
      break;

   case BTN_FRMT:
      // connect signal --> slots ...
      connect (pBtn, SIGNAL(clicked()), this, SLOT(slotSaveVideoFormat()));

      // add to vector ...
      _frmtBtnVector.append(pBtn);
      break;

   case BTN_SCRSHOT:
      // connect signal --> slots ...
      connect (pBtn, SIGNAL(clicked()), this, SLOT(slotScrShot()));

      // add to vector ...
      _scrShtBtnVector.append(pBtn);
      break;

   case BTN_WNDWD:
      // connect signal --> slots ...
      connect (pBtn, SIGNAL(clicked()), this, SLOT(slotEnterWndwd()));

      // add to vector ...
      _wndModBtnVector.append(pBtn);
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
//! \param   role role of button (start, stop, rec, ...)
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::btnSetIcon(const QIcon &icon, eBtnRole role)
{
   QVector<QPushButton *> *pVector = NULL;
   switch (role)
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

   case BTN_FS:
      pVector = &_fsBtnVector;
      break;

   case BTN_FRMT:
      pVector = &_frmtBtnVector;
      break;

   case BTN_SCRSHOT:
      pVector = &_scrShtBtnVector;
      break;

   case BTN_WNDWD:
      pVector = &_wndModBtnVector;
      break;

   default:
      break;
   }

   if (pVector)
   {
      for (int i = 0; i < pVector->count(); i++)
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
//! \brief   one fullscreen buttons was pressed, emit fullscreen signal
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::slotFs ()
{
   emit sigFullScreen();
}

//---------------------------------------------------------------------------
//
//! \brief   one screenshot button was pressed, emit scrrenshot signal
//
//! \author  Jo2003
//! \date    13.11.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::slotScrShot()
{
   emit sigScrShot();
}

//---------------------------------------------------------------------------
//
//! \brief   one enter windowed mode button was pressed, emit signal
//
//! \author  Jo2003
//! \date    26.11.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::slotEnterWndwd()
{
   emit sigEnterWndwd();
}

//---------------------------------------------------------------------------
//
//! \brief   save video format button was pressed, emit signal
//
//! \author  Jo2003
//! \date    03.12.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::slotSaveVideoFormat()
{
   emit sigSaveVideoFormat();
}

//---------------------------------------------------------------------------
//
//! \brief   enable / disable buttons with given role
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \param   enable enable / disable flag
//! \param   role button role
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::enableBtn (bool enable, eBtnRole role)
{
   QVector<QPushButton *> *pVector = NULL;
   switch (role)
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

   case BTN_FS:
      pVector = &_fsBtnVector;
      break;

   case BTN_FRMT:
      pVector = &_frmtBtnVector;
      break;

   case BTN_SCRSHOT:
      pVector = &_scrShtBtnVector;
      break;

   case BTN_WNDWD:
      pVector = &_wndModBtnVector;
      break;

   default:
      break;
   }

   if (pVector)
   {
      for (int i = 0; i < pVector->count(); i++)
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

   disconnect(this, SLOT(slotFs()));
   _fsBtnVector.clear();

   disconnect(this, SLOT(slotScrShot()));
   _scrShtBtnVector.clear();

   disconnect(this, SLOT(slotEnterWndwd()));
   _wndModBtnVector.clear();

   disconnect(this, SLOT(slotSaveVideoFormat()));
   _frmtBtnVector.clear();
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
//! \param   pSli pointer to volume slider
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::addVolSlider (QClickAndGoSlider *pSli)
{
   // make sure we can use it as a normal slider as well ...
   pSli->setHandleRangeVal(20);

   connect (pSli, SIGNAL(sliderMoved(int)), this, SLOT(slotVolSliderMoved(int)));
   connect (pSli, SIGNAL(sigClickNGo(int)), this, SLOT(slotVolSliderMoved(int)));
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
   // sliders are in sync ...
   return _volSliderVector.isEmpty() ? -1 : _volSliderVector.at(0)->value();
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
   // values are synchronized ...
   return _volSliderVector.isEmpty() ? -1 : _volSliderVector.at(0)->minimum();
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
   // values are synchronized ...
   return _volSliderVector.isEmpty() ? -1 : _volSliderVector.at(0)->maximum();
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
   disconnect(this, SLOT(slotVolSliderMoved(int)));
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
void QFusionControl::addJumpBox (QComboBoxEx *pBox)
{
   connect (pBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotJumpValChanged(int)));
   connect (pBox, SIGNAL(sigShownPopup()), this, SLOT(slotAddPopup()));
   connect (pBox, SIGNAL(sigHidePopup()), this, SLOT(slotRemPopup()));
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
   for (int i = 0; i < _jumpCbxVector.count(); i ++)
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

   if (!_jumpCbxVector.isEmpty())
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
   for (int i = 0; i < _jumpCbxVector.count(); i ++)
   {
      _jumpCbxVector.at(i)->setEnabled(enable);
   }
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

   for (int i = 0; ((i < _cngSliderVector.count()) && !bIsDown); i++)
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
   for (int i = 0; i < _cngSliderVector.count(); i++)
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
   // values are synchronized ...
   return _cngSliderVector.isEmpty() ? -1 : _cngSliderVector.at(0)->minimum();
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
   // values are synchronized ...
   return _cngSliderVector.isEmpty() ? -1 : _cngSliderVector.at(0)->maximum();
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
   for (int i = 0; i < _cngSliderVector.count(); i++)
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
   for (int i = 0; i < _cngSliderVector.count(); i++)
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
   // values are synchronized ...
   return _cngSliderVector.isEmpty() ? -1 : _cngSliderVector.at(0)->value();
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
   disconnect(this, SLOT(slotClickNGo(int)));
   disconnect(this, SLOT(slotPosSliderReleased()));
   disconnect(this, SLOT(slotPosSliderValueChanged(int)));
   _cngSliderVector.clear();
}

/////////////////////////////////////////////////////////////////////////////
/// label region (mute, time) ...
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
   for (int i = 0; i < _timeLabVector.count(); i++)
   {
      _timeLabVector.at(i)->setTime(time);
   }
}

//---------------------------------------------------------------------------
//
//! \brief   set buffer value
//
//! \author  Jo2003
//! \date    21.04.2013
//
//! \param   iPercent new buffer value in percent
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::setBuff(int iPercent)
{
   for (int i = 0; i < _timeLabVector.count(); i++)
   {
      _timeLabVector.at(i)->setBuffPercent(iPercent);
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
   _muteLabel.clear();
   _infoLabel.clear();
}

//---------------------------------------------------------------------------
//
//! \brief   put mute label under control
//
//! \author  Jo2003
//! \date    29.11.2012
//
//! \param   pLab pointer to QLabel
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::addMuteLab(QLabel *pLab)
{
   _muteLabel.append(pLab);
}

//---------------------------------------------------------------------------
//
//! \brief   set pixmap to all mute labels
//
//! \author  Jo2003
//! \date    29.11.2012
//
//! \param   pix new pixmap to set
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::setMutePixmap(const QPixmap &pix)
{
   for (int i = 0; i < _muteLabel.count(); i++)
   {
      _muteLabel.at(i)->setPixmap(pix);
   }
}

/////////////////////////////////////////////////////////////////////////////
/// Video display format comboboxes region ...
/////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//
//! \brief   put one of the video display format comboboxes under control
//
//! \author  Jo2003
//! \date    03.12.2012
//
//! \param   cbx pointer to combobox to add
//! \param   role role of this combobox (aspect or crop)
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::addVidFormCbx(QComboBoxEx *cbx, eVidForCbxRole role)
{
   switch (role)
   {
   case CBX_ASPECT:
      connect (cbx, SIGNAL(currentIndexChanged(int)), this, SLOT(slotAspectCurrentIndexChanged(int)));
      connect (cbx, SIGNAL(sigShownPopup()), this, SLOT(slotAddPopup()));
      connect (cbx, SIGNAL(sigHidePopup()), this, SLOT(slotRemPopup()));
      _aspectCbxVector.append(cbx);
      break;
   case CBX_CROP:
      connect (cbx, SIGNAL(currentIndexChanged(int)), this, SLOT(slotCropCurrentIndexChanged(int)));
      connect (cbx, SIGNAL(sigShownPopup()), this, SLOT(slotAddPopup()));
      connect (cbx, SIGNAL(sigHidePopup()), this, SLOT(slotRemPopup()));
      _cropCbxVector.append(cbx);
      break;
   default:
      break;
   }
}

//---------------------------------------------------------------------------
//
//! \brief   release signal -> slot connections, remove cbx pointer
//
//! \author  Jo2003
//! \date    03.12.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::disconnectCbx()
{
   disconnect(this, SLOT(slotCropCurrentIndexChanged(int)));
   disconnect(this, SLOT(slotAspectCurrentIndexChanged(int)));
   disconnect(this, SLOT(slotAddPopup()));
   disconnect(this, SLOT(slotRemPopup()));
   disconnect(this, SLOT(slotJumpValChanged(int)));
   _jumpCbxVector.clear();
   _aspectCbxVector.clear();
   _cropCbxVector.clear();
}

//---------------------------------------------------------------------------
//
//! \brief   clear combobox content
//
//! \author  Jo2003
//! \date    03.12.2012
//
//! \param   role combobox role (aspect or crop)
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::vidFormCbxClear (eVidForCbxRole role)
{
   QVector<QComboBoxEx *> *pVector = NULL;

   switch (role)
   {
   case CBX_ASPECT:
      pVector = &_aspectCbxVector;
      break;
   case CBX_CROP:
      pVector = &_cropCbxVector;
      break;
   default:
      break;
   }

   if (pVector)
   {
      for (int i = 0; i < pVector->count(); i++)
      {
         pVector->at(i)->clear();
      }
   }
}

//---------------------------------------------------------------------------
//
//! \brief   fill comboboxes with content
//
//! \author  Jo2003
//! \date    03.12.2012
//
//! \param   idx(int) index to start with
//! \param   items stringlist with items to add
//! \param   role combobox role (aspect o crop)
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::vidFormCbxInsertValues (int idx, const QStringList &items, eVidForCbxRole role)
{
   QVector<QComboBoxEx *> *pVector = NULL;

   switch (role)
   {
   case CBX_ASPECT:
      pVector = &_aspectCbxVector;
      break;
   case CBX_CROP:
      pVector = &_cropCbxVector;
      break;
   default:
      break;
   }

   if (pVector)
   {
      for (int i = 0; i < pVector->count(); i++)
      {
         pVector->at(i)->insertItems(idx, items);
      }
   }
}

//---------------------------------------------------------------------------
//
//! \brief   set new current index on comboboxes
//
//! \author  Jo2003
//! \date    03.12.2012
//
//! \param   idx(int) new current index to set
//! \param   role combobox role (aspect o crop)
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::vidFormCbxSetCurrentIndex (int idx, eVidForCbxRole role)
{
   QVector<QComboBoxEx *> *pVector = NULL;

   switch (role)
   {
   case CBX_ASPECT:
      pVector = &_aspectCbxVector;
      break;
   case CBX_CROP:
      pVector = &_cropCbxVector;
      break;
   default:
      break;
   }

   if (pVector)
   {
      for (int i = 0; i < pVector->count(); i++)
      {
         pVector->at(i)->setCurrentIndex(idx);
      }
   }
}

//---------------------------------------------------------------------------
//
//! \brief   get current text from combobox, please note that all comboboxes
//!          with equal role are synchronized
//
//! \author  Jo2003
//! \date    03.12.2012
//
//! \param   role combobox role (aspect o crop)
//
//! \return  current text as QString
//---------------------------------------------------------------------------
QString QFusionControl::vidFormCbxCurrentText (eVidForCbxRole role)
{
   QVector<QComboBoxEx *> *pVector = NULL;
   QString                 sRV;

   switch (role)
   {
   case CBX_ASPECT:
      pVector = &_aspectCbxVector;
      break;
   case CBX_CROP:
      pVector = &_cropCbxVector;
      break;
   default:
      break;
   }

   if (pVector)
   {
      // please note: values are synchronized ...
      if(!pVector->isEmpty())
      {
         sRV = pVector->at(0)->currentText();
      }
   }

   return sRV;
}

//---------------------------------------------------------------------------
//
//! \brief   get current index from combobox, please note that all comboboxes
//!          with equal role are synchronized
//
//! \author  Jo2003
//! \date    03.12.2012
//
//! \param   role combobox role (aspect o crop)
//
//! \return  current index (int)
//---------------------------------------------------------------------------
int QFusionControl::vidFormCbxCurrentIndex (eVidForCbxRole role)
{
   QVector<QComboBoxEx *> *pVector = NULL;
   int                     iRV     = -1;

   switch (role)
   {
   case CBX_ASPECT:
      pVector = &_aspectCbxVector;
      break;
   case CBX_CROP:
      pVector = &_cropCbxVector;
      break;
   default:
      break;
   }

   if (pVector)
   {
      // please note: values are synchronized ...
      if(!pVector->isEmpty())
      {
         iRV = pVector->at(0)->currentIndex();
      }
   }

   return iRV;
}

//---------------------------------------------------------------------------
//
//! \brief   get item count from combobox, please note that all comboboxes
//!          with equal role are synchronized
//
//! \author  Jo2003
//! \date    03.12.2012
//
//! \param   role combobox role (aspect o crop)
//
//! \return  item count (int)
//---------------------------------------------------------------------------
int QFusionControl::vidFormCbxCount(eVidForCbxRole role)
{
   QVector<QComboBoxEx *> *pVector = NULL;
   int                     iRV     = -1;

   switch (role)
   {
   case CBX_ASPECT:
      pVector = &_aspectCbxVector;
      break;
   case CBX_CROP:
      pVector = &_cropCbxVector;
      break;
   default:
      break;
   }

   if (pVector)
   {
      // please note: values are synchronized ...
      if(!pVector->isEmpty())
      {
         iRV = pVector->at(0)->count();
      }
   }

   return iRV;
}

//---------------------------------------------------------------------------
//
//! \brief   get position of text in combobox, please note
//!          that all comboboxes with equal role are synchronized
//
//! \author  Jo2003
//! \date    03.12.2012
//
//! \param   str string to find
//! \param   role combobox role (aspect o crop)
//
//! \return  -1 -> not found; else index of entry in cbx
//---------------------------------------------------------------------------
int QFusionControl::vidFormCbxFindText(const QString &str, eVidForCbxRole role)
{
   QVector<QComboBoxEx *> *pVector = NULL;
   int                     iRV     = -1;

   switch (role)
   {
   case CBX_ASPECT:
      pVector = &_aspectCbxVector;
      break;
   case CBX_CROP:
      pVector = &_cropCbxVector;
      break;
   default:
      break;
   }

   if (pVector)
   {
      // please note: values are synchronized ...
      if(!pVector->isEmpty())
      {
         iRV = pVector->at(0)->findText(str);
      }
   }

   return iRV;
}

//---------------------------------------------------------------------------
//
//! \brief   aspect ratio on one aspect CBX changed, emit signal,
//!          synchronize with comboboxes with equal role
//
//! \author  Jo2003
//! \date    03.12.2012
//
//! \param   str string to find
//! \param   role combobox role (aspect o crop)
//
//! \return  -1 -> not found; else index of entry in cbx
//---------------------------------------------------------------------------
void QFusionControl::slotAspectCurrentIndexChanged (int pos)
{
   vidFormCbxSetCurrentIndex(pos, CBX_ASPECT);
   emit sigAspectCurrentIndexChanged(pos);
}

//---------------------------------------------------------------------------
//
//! \brief   crop ratio on one crop CBX changed, emit signal,
//!          synchronize with comboboxes with equal role
//
//! \author  Jo2003
//! \date    03.12.2012
//
//! \param   str string to find
//! \param   role combobox role (aspect o crop)
//
//! \return  -1 -> not found; else index of entry in cbx
//---------------------------------------------------------------------------
void QFusionControl::slotCropCurrentIndexChanged (int pos)
{
   vidFormCbxSetCurrentIndex(pos, CBX_CROP);
   emit sigCropCurrentIndexChanged(pos);
}

//---------------------------------------------------------------------------
//
//! \brief   combobox shows popup
//
//! \author  Jo2003
//! \date    03.12.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::slotAddPopup ()
{
   _mPopup.lock();
   _iPopup ++;
   _mPopup.unlock();
}

//---------------------------------------------------------------------------
//
//! \brief   combobox hides popup
//
//! \author  Jo2003
//! \date    03.12.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::slotRemPopup ()
{
   _mPopup.lock();
   if (_iPopup > 0)
   {
      _iPopup --;
   }
   _mPopup.unlock();
}

//---------------------------------------------------------------------------
//
//! \brief   is a popup from any combobox active ?
//
//! \author  Jo2003
//! \date    03.12.2012
//
//! \param   --
//
//! \return  true -> active, false -> not active
//---------------------------------------------------------------------------
bool QFusionControl::isPopupActive()
{
   return !!_iPopup;
}

//---------------------------------------------------------------------------
//
//! \brief   put video label under control
//
//! \author  Jo2003
//! \date    05.12.2012
//
//! \param   pLab pointer to QLabel to control
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::addInfoLab (QMoveHandle *pLab)
{
   _infoLabel.append(pLab);
}

//---------------------------------------------------------------------------
//
//! \brief   set video info to all info labels
//
//! \author  Jo2003
//! \date    05.12.2012
//
//! \param   str text to set
//
//! \return  --
//---------------------------------------------------------------------------
void QFusionControl::setVideoInfo(const QString &str)
{
   // cut string ...
   QString info;
   QFont   labFont;

   for (int i = 0; i < _infoLabel.count(); i++)
   {
      info    = str;
      labFont = _infoLabel.at(i)->font();

      labFont.setBold(true);
      labFont.setPointSize(labFont.pointSize() + 1);

      QFontMetrics fmLab(labFont);

      // the 400 comes from the width of the overlay control widget!
      CSmallHelpers::cutProgString(info, fmLab, 400);
      _infoLabel.at(i)->setText(info);
   }
}
