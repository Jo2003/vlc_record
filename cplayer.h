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

#ifdef __QT_5
   #include <QtWidgets/QApplication>
#else
   #include <QtGui/QApplication>
#endif

#include <QMessageBox>
#include <QWidget>
#include <QFrame>
#include <QTimer>
#include <QEvent>
#include <QTime>
#include <QDesktopWidget>
#include <QMutex>

#include <vlc/vlc.h>
#include "playstates.h"
#include "defdef.h"
#include "qtimerex.h"
#include "csettingsdlg.h"
#include "qvlcvideowidget.h"

//===================================================================
// namespace
//===================================================================
namespace Ui
{
   class CPlayer;
}

namespace Player
{
   struct SPauseResume
   {
      SPauseResume() :timeStamp(0),id(-1),bArch(false){}
      ulong timeStamp;
      int   id;
      bool  bArch;
   };

   struct SErrHelper
   {
      SErrHelper():errCount(0), lastBuffer(0), enabled(false){}
      QString mrl;
      int     errCount;
      int     lastBuffer;
      bool    enabled;
   };
}

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
   int  initPlayer (const QString &sOpts);
   bool isPlaying ();
   void setShortCuts (QVector<CShortcutEx *> *pvSc);
   void startPlayTimer ();
   void pausePlayTimer ();
   void stopPlayTimer ();
   void setSettings (CSettingsDlg *pDlg);
   bool isPositionable();
   void initSlider ();
   uint getSliderPos();
   QVlcVideoWidget* getAndRemoveVideoWidget();
   void  addAndEmbedVideoWidget();
   ulong libvlcVersion();
   void resetBuffPercent();
   QVlcVideoWidget*& getVideoWidget();
   void aboutToClose();

   static QVector<libvlc_event_type_t> _eventQueue;
   static const char*                  _pAspect[];
   static const char*                  _pCrop[];
   static QMutex                       _mtxEvt;
   static float                        _flBuffPrt;
   static void eventCallback (const libvlc_event_t *ev, void *userdata);
   static libvlc_media_t              *_pCurrentMedia;
   static bool                         _bVoutSent;

protected:
   virtual void changeEvent(QEvent *e);
   void enableDisablePlayControl (bool bEnable);
   void connectToVideoWidget ();
   void disconnectVideoWidget();
   int  addAd ();
   int  clearMediaList();
   QString aspectCropToString (const char *pFormat);
   void cleanupLibVLC();

private:
   Ui::CPlayer                 *ui;
   QTimer                       sliderTimer;
   QTimer                       tEventPoll;
   QTimerEx                     timer;
   QTime                        tPaused;
   libvlc_media_t              *videoMediaItem;
   libvlc_media_t              *addMediaItem;
   libvlc_instance_t           *pVlcInstance;
   libvlc_media_player_t       *pMediaPlayer;
   libvlc_media_list_player_t  *pMedialistPlayer;
   libvlc_media_list_t         *pMediaList;
   CSettingsDlg                *pSettings;
   bool                         bSpoolPending;
   uint                         uiDuration;
   ulong                        ulLibvlcVersion;
   bool                         bOmitNextEvent;
   const char*                  vlcArgs[MAX_LVLC_ARGS];
   QVector<QByteArray>          vArgs;
   bool                         bScanAuTrk;
   QLangVector                  vAudTrk;
   Player::SPauseResume         pauseResume;
   IncPlay::ePlayStates         libPlayState;
   Player::SErrHelper           errHelper;
   bool                         _bVoutHandled;

private slots:
   void slotPositionChanged(int value);
   void slotCbxAspectCurrentIndexChanged(int idx);
   void slotCbxCropCurrentIndexChanged(int idx);
   void slotChangeVolume(int newVolume);
   void slotUpdateSlider ();
   void slotChangeVolumeDelta (const bool up);
   void slotSliderPosChanged();
   void slotToggleFullscreen();
   void slotEventPoll();
   void slotFinallyPlays(int percent);
   void slotBtnSaveAspectCropClicked();
   void slotTimedLibClean();


public slots:
   int  playMedia (const QString &sCmdLine, const QString &sOpts);
   int  play();
   int  stop();
   int  silentStop();
   int  pause();
   int  slotToggleAspectRatio ();
   int  slotToggleCropGeometry ();
   int  slotTimeJumpRelative (int iSeconds);
   void slotStoredAspectCrop ();
   void slotMoreLoudly();
   void slotMoreQuietly();
   void slotMute();
   void slotShowInfoUpdated();
   void slotFsToggled (int on);
   void slotWindowed (int on);
   void slotResetVideoFormat();
   void slotDeinterlace (bool bDeintl);
   void slotChangeATrack (int id);
   void slotTakeScreenShot();

signals:
   void sigPlayState (int ps);
   void sigTriggerAspectChg ();
   void sigCheckArchProg(ulong ulArchGmt);
   void sigToggleFullscreen();
   void sigBuffPercent(int);
   void sigAudioTracks(QLangVector);
   void sigStateMessage(int, const QString&, int);
};

#endif /* __022410__CPLAYER_H */
/************************* History ***************************\
| $Log$
\*************************************************************/
