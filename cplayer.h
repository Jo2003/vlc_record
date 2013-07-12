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
#include <QMutex>

#include <vlc/vlc.h>

#include "cvlcrecdb.h"
#include "clogfile.h"
#include "playstates.h"
#include "defdef.h"
#include "qtimerex.h"
#include "cshowinfo.h"
#include "csettingsdlg.h"
#include "qvlcvideowidget.h"
#include "api_inc.h"

//===================================================================
// namespace
//===================================================================
namespace Ui
{
   class CPlayer;
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
   void setApiClient (ApiClient *pClient);
   bool isPositionable();
   void initSlider ();
   uint getSilderPos();
   QVlcVideoWidget* getAndRemoveVideoWidget();
   void  addAndEmbedVideoWidget();
   ulong libvlcVersion();
   void resetBuffPercent();

   static QVector<libvlc_event_type_t> _eventQueue;
   static const char*                  _pAspect[];
   static const char*                  _pCrop[];
   static QMutex                       _mtxEvt;
   static float                        _flBuffPrt;
   static void eventCallback (const libvlc_event_t *ev, void *userdata);

protected:
   virtual void changeEvent(QEvent *e);
   void enableDisablePlayControl (bool bEnable);
   void connectToVideoWidget ();
   int  addAd ();
   int  clearMediaList();
   void cleanupLibVLC(bool bDestruct = false);
   QString aspectCropToString (const char *pFormat);

private:
   Ui::CPlayer                 *ui;
   QTimer                       sliderTimer;
   QTimer                       tAspectShot;
   QTimer                       tEventPoll;
   QTimerEx                     timer;
   libvlc_instance_t           *pVlcInstance;
   libvlc_media_player_t       *pMediaPlayer;
   libvlc_event_manager_t      *pEMPlay;
   libvlc_media_list_player_t  *pMedialistPlayer;
   libvlc_media_list_t         *pMediaList;
   CSettingsDlg                *pSettings;
   ApiClient                   *pApiClient;
   bool                         bSpoolPending;
   uint                         uiDuration;
   ulong                        ulLibvlcVersion;
   bool                         bOmitNextEvent;
   const char*                  vlcArgs[MAX_LVLC_ARGS];
   QVector<QByteArray>          vArgs;
   bool                         bScanAuTrk;

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
   void on_checkDeintl_toggled(bool checked);

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
   void slotResetVideoFormat();
   void slotToggleDeinterlace ();

signals:
   void sigPlayState (int ps);
   void sigTriggerAspectChg ();
   void sigCheckArchProg(ulong ulArchGmt);
   void sigToggleFullscreen();
   void sigBuffPercent(int);
};

#endif /* __022410__CPLAYER_H */
/************************* History ***************************\
| $Log$
\*************************************************************/
