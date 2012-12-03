/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qfusioncontrol.h
 *
 *  @author   Jo2003
 *
 *  @date     23.11.2012
 *
 *  $Id$
 *
 *///------------------------- (c) 2012 by Jo2003  --------------------------
#ifndef __20121123_QFUSIONCONTROL_H
   #define __20121123_QFUSIONCONTROL_H

#include <QObject>
#include <QVector>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QMutex>

#include "qclickandgoslider.h"
#include "qtimelabel.h"
#include "qcomboboxex.h"


//---------------------------------------------------------------------------
//! \class   QFusionControl
//! \date    23.11.2012
//! \author  Jo2003
//! \brief   combine control elements and mirror actions / behavior / look
//---------------------------------------------------------------------------
class QFusionControl : public QObject
{
   Q_OBJECT

public:
   /// Button role
   enum eBtnRole
   {
      BTN_PLAY,      ///< play
      BTN_REC,       ///< record
      BTN_STOP,      ///< stop
      BTN_FWD,       ///< forward
      BTN_BWD,       ///< backward
      BTN_FS,        ///< fullscreen
      BTN_FRMT,      ///< save video format
      BTN_ANY = 255  ///< undefined button
   };

   /// combobox role
   enum eVidForCbxRole
   {
      CBX_ASPECT,    ///< aspct ratio combobax
      CBX_CROP,      ///< crop ratio combobox
      CBX_ANY = 255  ///< undefined combobox
   };

   QFusionControl(QObject *parent = 0);
   virtual ~QFusionControl();

   // add control elements to its vector ...
   void addButton (QPushButton *pBtn, eBtnRole role);
   void addVolSlider (QSlider *pSli);
   void addJumpBox (QComboBoxEx *pBox);
   void addCngSlider (QClickAndGoSlider *pSli);
   void addTimeLab (QTimeLabel *pLab);
   void addMuteLab (QLabel *pLab);
   void enableBtn (bool enable, eBtnRole role);
   void setVolSliderPosition (int vol);
   void setVolume (int vol);
   void disconnectCtrls ();
   int  getJumpValue ();
   int  getVolume ();
   void enableJumpBox (bool enable);
   bool isPosSliderDown ();
   void setPosValue (int pos);
   int  posMinimum ();
   int  posMaximum ();
   int  volMinimum ();
   int  volMaximum ();
   void enablePosSlider (bool enable);
   void setPosRange (int min, int max);
   int  posValue ();
   void setTime (quint64 time);
   void btnSetIcon (const QIcon &icon, eBtnRole role);
   void setMutePixmap (const QPixmap &pix);

   // video display format comboboxes ...
   void addVidFormCbx (QComboBoxEx *cbx, eVidForCbxRole role);
   void vidFormCbxClear (eVidForCbxRole role);
   void vidFormCbxInsertValues (int idx, const QStringList &items, eVidForCbxRole role);
   void vidFormCbxSetCurrentIndex (int idx, eVidForCbxRole role);
   QString vidFormCbxCurrentText (eVidForCbxRole role);
   int  vidFormCbxCurrentIndex (eVidForCbxRole role);
   int  vidFormCbxCount (eVidForCbxRole role);
   int  vidFormCbxFindText(const QString &str, eVidForCbxRole role);
   bool isPopupActive ();

   
signals:
   void sigVolSliderMoved (int vol);
   void sigPlay ();
   void sigStop ();
   void sigRec ();
   void sigFwd ();
   void sigBwd ();
   void sigSaveVideoFormat ();
   void sigFullScreen ();
   void sigPosClickNGo (int pos);
   void sigPosSliderReleased ();
   void sigPosSliderValueChanged (int pos);
   void sigAspectCurrentIndexChanged (int pos);
   void sigCropCurrentIndexChanged (int pos);

protected:
   void disconnectBtn ();
   void disconnectVol ();
   void disconnectCbx ();
   void disconnectCng ();
   void disconnectLab ();
   
public slots:

private slots:
   void slotPlay ();
   void slotRec ();
   void slotStop ();
   void slotFwd ();
   void slotBwd ();
   void slotFs ();
   void slotVolSliderMoved (int vol);
   void slotJumpValChanged (int idx);
   void slotClickNGo(int pos);
   void slotPosSliderReleased ();
   void slotPosSliderValueChanged (int pos);
   void slotAspectCurrentIndexChanged (int pos);
   void slotCropCurrentIndexChanged (int pos);
   void slotAddPopup ();
   void slotRemPopup ();
   void slotSaveVideoFormat ();

private:
   // volume slider ...
   QVector<QSlider *>            _volSliderVector;

   // push buttons ...
   QVector<QPushButton *>        _playBtnVector;
   QVector<QPushButton *>        _recBtnVector;
   QVector<QPushButton *>        _stopBtnVector;
   QVector<QPushButton *>        _fwdBtnVector;
   QVector<QPushButton *>        _bwdBtnVector;
   QVector<QPushButton *>        _fsBtnVector;
   QVector<QPushButton *>        _frmtBtnVector;

   // time jump combobox ...
   QVector<QComboBoxEx *>          _jumpCbxVector;

   // click'n'go slider ...
   QVector<QClickAndGoSlider *>  _cngSliderVector;

   // timer label ...
   QVector<QTimeLabel *>         _timeLabVector;

   // mute label ...
   QVector<QLabel *>             _muteLabel;

   // video display format comboboxes ...
   QVector<QComboBoxEx *>          _aspectCbxVector;
   QVector<QComboBoxEx *>          _cropCbxVector;

   // how many popups are open ... ?
   int                             _iPopup;
   QMutex                          _mPopup;
};

#endif // __20121123_QFUSIONCONTROL_H
