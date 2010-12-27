/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 24.02.2010 / 10:41:34
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#ifndef __022410__CPLAYER_H
   #define __022410__CPLAYER_H

#include <QtGui/QApplication>
#include <QMessageBox>
#include <QWidget>
#include <QFrame>
#include <QTimer>
#include <QEvent>
#include <QTime>
#include <QDesktopWidget>

#include <vlc/vlc.h>

#include "cvlcrecdb.h"
#include "clogfile.h"
#include "playstates.h"
#include "defdef.h"
#include "ctimerex.h"
#include "cshowinfo.h"
#include "csettingsdlg.h"
#include "cwaittrigger.h"
#include "cvideoframe.h"

//===================================================================
// namespace
//===================================================================
namespace Ui
{
   class CPlayer;
   typedef struct _vlcArgs
   {
      char** argArray;
      int    argc;
   } vlcArgs;
}

//===================================================================
// macro to connect player to hardware ...
//===================================================================
#ifdef Q_OS_WIN        // on windows ...
   #define connect_to_wnd(a, b) libvlc_media_player_set_hwnd (a, b)
#elif defined Q_OS_MAC // on MAC OS
   #define connect_to_wnd(a, b) libvlc_media_player_set_agl (a, b)
#else                  // on Linux
   #define connect_to_wnd(a, b) libvlc_media_player_set_xwindow (a, b)
#endif

/********************************************************************\
|  Class: CPlayer
|  Date:  14.02.2010 / 11:42:24
|  Author: Jo2003
|  Description: widget with vlc player (using libvlc)
|
\********************************************************************/
class CPlayer : public QWidget
{
   Q_OBJECT

public:
   CPlayer(QWidget *parent = 0);
   ~CPlayer();
   void cleanExit ();
   int  initPlayer ();
   bool isPlaying ();
   void setShortCuts (QVector<CShortcutEx *> *pvSc);
   void startPlayTimer ();
   void pausePlayTimer ();
   void stopPlayTimer ();
   void setSettings (CSettingsDlg *pDlg);
   void setTrigger (CWaitTrigger *pTrig);
   static void eventCallback (const libvlc_event_t *ev, void *player);
   bool isPositionable();
   void initSlider ();

protected:
   void changeEvent(QEvent *e);
   int  myToggleFullscreen ();
   void enableDisablePlayControl (bool bEnable);

private:
   Ui::CPlayer            *ui;
   QTimer                  poller;
   QTimer                  sliderTimer;
   QTimer                  tAspectShot;
   CTimerEx                timer;
   libvlc_instance_t      *pVlcInstance;
   libvlc_media_player_t  *pMediaPlayer;
   libvlc_event_manager_t *pEMPlay;
   libvlc_log_t           *pLibVlcLog;
   uint                    uiVerboseLevel;
   bool                    bCtrlStream;
   CSettingsDlg           *pSettings;
   CWaitTrigger           *pTrigger;
   bool                    bSpoolPending;
   uint                    uiDuration;

private slots:
   void on_posSlider_valueChanged(int value);
   void on_posSlider_sliderReleased();
   void on_btnFullScreen_clicked();
   void on_cbxAspect_currentIndexChanged(QString str);
   void on_cbxCrop_currentIndexChanged(QString str);
   void slotChangeVolume(int newVolume);
   void slotLibVLCLog ();
   void slotUpdateSlider ();

public slots:
   int  playMedia (const QString &sCmdLine, bool bAllowCtrl);
   int  play();
   int  stop();
   int  pause();
   int  slotToggleFullScreen ();
   int  slotToggleAspectRatio ();
   int  slotToggleCropGeometry ();
   int  slotTimeJumpRelative (int iSeconds);
   void slotStoredAspectCrop ();

signals:
   void sigPlayState (int ps);
   void sigTriggerAspectChg ();
};

#endif /* __022410__CPLAYER_H */
/************************* History ***************************\
| $Log$
\*************************************************************/
