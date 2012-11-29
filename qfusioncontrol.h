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
#include <QComboBox>
#include <QLabel>

#include "qclickandgoslider.h"
#include "qtimelabel.h"


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
   /// Button rol
   enum eBtnRol
   {
      BTN_PLAY,      ///< play
      BTN_REC,       ///< record
      BTN_STOP,      ///< stop
      BTN_FWD,       ///< forward
      BTN_BWD,       ///< backward
      BTN_FS,        ///< fullscreen
      BTN_ANY = 255  ///< undefined button
   };

   QFusionControl(QObject *parent = 0);
   virtual ~QFusionControl();

   // add control elements to its vector ...
   void addButton (QPushButton *pBtn, eBtnRol rol);
   void addVolSlider (QSlider *pSli);
   void addJumpBox (QComboBox *pBox);
   void addCngSlider (QClickAndGoSlider *pSli);
   void addTimeLab (QTimeLabel *pLab);
   void addMuteLab (QLabel *pLab);
   void enableBtn (bool enable, eBtnRol rol);
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
   void btnSetIcon (const QIcon &icon, eBtnRol rol);
   void setMutePixmap (const QPixmap &pix);
   
signals:
   void sigVolSliderMoved (int vol);
   void sigPlay ();
   void sigStop ();
   void sigRec ();
   void sigFwd ();
   void sigBwd ();
   void sigFullScreen ();
   void sigPosClickNGo (int pos);
   void sigPosSliderReleased ();
   void sigPosSliderValueChanged (int pos);

protected:
   void disconnectBtn ();
   void disconnectVol ();
   void disconnectJmp ();
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

   // time jump combobox ...
   QVector<QComboBox *>          _jumpCbxVector;

   // click'n'go slider ...
   QVector<QClickAndGoSlider *>  _cngSliderVector;

   // timer label ...
   QVector<QTimeLabel *>         _timeLabVector;

   // mute label ...
   QVector<QLabel *>             _muteLabel;
};

#endif // __20121123_QFUSIONCONTROL_H
