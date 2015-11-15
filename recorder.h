/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 19.01.2010 / 15:59:00
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#ifndef __011910__RECORDER_H
   #define __011910__RECORDER_H

#ifdef __QT_5
   #include <QtWidgets/QDialog>
#else
   #include <QtGui/QDialog>
#endif

#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QTabBar>
#include <QToolButton>
#include <QRegExp>
#include <QTextStream>
#include <QListWidgetItem>
#include <QApplication>
#include <QProcess>
#include <QStringList>
#include <QFileDialog>
#include <QDateTime>
#include <QIcon>
#include <QPixmap>
#include <QTimer>
#include <QTranslator>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QWindowStateChangeEvent>
#include <QMap>
#include <QStandardItemModel>
#include <QScrollBar>

#include "csettingsdlg.h"
#include "templates.h"
#include "caboutdialog.h"
#include "cpixloader.h"
#include "ctimerrec.h"
#include "cvlcctrl.h"
#include "ctranslit.h"
#include "cfavaction.h"
#include "cshortcutex.h"
#include "cstreamloader.h"
#include "qchanlistdelegate.h"
#include "cepgbrowser.h"
#include "qseccodedlg.h"
#include "qhelpdialog.h"
#include "qrecordtimerwidget.h"
#include "qupdatenotifydlg.h"
#include "qexpirenotifydlg.h"
#include "qstringfilterwidgetaction.h"
#include "qwatchlistdlg.h"
#include <QStackedLayout>
#include "qvlcvideowidget.h"
#include "qauthdlg.h"
#include "qwaitwidget.h"

//------------------------------------------------------------------
/// \name definition of start flags
//------------------------------------------------------------------
// @{
#define FLAG_INITDIALOG     (ulong)(1<<0) ///< should we run initDialog()
#define FLAG_CONN_CHAIN     (ulong)(1<<1) ///< should we start connection chain
#define FLAG_CHAN_LIST      (ulong)(1<<2) ///< should we set channel from former session
#define FLAG_EPG_DAY        (ulong)(1<<3) ///< should we set epg day from former session
#define FLAG_CLOGO_COMPL    (ulong)(1<<4) ///< channel logos completely loaded
// @}

//===================================================================
// namespace
//===================================================================
namespace Ui
{
    class Recorder;
    struct SChanListElement
    {
       int iIndex;
       QString sChannel;
       QString sURL;
    };

    struct SShortCuts
    {
       QString     sDescr;
       QObject    *pObj;
       const char *pSlot;
       QString     sShortCut;
    };

    struct STabWidget
    {
       QString  sText;
       QIcon    icon;
       int      iPos;
       QWidget *pWidget;
    };

    enum EDisplayMode
    {
       DM_FULLSCREEN,
       DM_WINDOWED,
       DM_NORMAL,
       DM_UNKNOWN
    };
}

/********************************************************************\
|  Class: Recorder
|  Date:  19.01.2010 / 16:00:28
|  Author: Jo2003
|  Description: dialog class for the recorder gui
|
\********************************************************************/
class Recorder : public QDialog
{
   Q_OBJECT

public:
   Recorder(QWidget *parent = 0);
    ~Recorder();

   // cache request for later use ...
   typedef struct ReqItem {
      ReqItem():bValid(false){}
      bool            bValid;
      CIptvDefs::EReq req;
      QVariant        par_1;
      QVariant        par_2;
   } reqItem_t;

public slots:
    virtual void show();
    void slotRestoreMinimized ();

private:
    Ui::Recorder                   *ui;
    CSettingsDlg                    Settings;
    QHelpDialog                    *pHelp;
    QSecCodeDlg                     secCodeDlg;
    CStreamLoader                   streamLoader;
    QTimer                          Refresh;
    QTimer                          m_tVodSearch;
    CPixLoader                      pixCache;
    int                             iEpgOffset;
    QTabBar                        *pEpgNavbar;
    CTimerRec                       timeRec;
    QSystemTrayIcon                 trayIcon;
    CVlcCtrl                        vlcCtrl;
    CTranslit                       translit;
    int                             iFontSzChg;
    QList<int>                      lFavourites;
    QToolButton                    *pFavBtn[MAX_NO_FAVOURITES];
    CFavAction                     *pFavAct[MAX_NO_FAVOURITES];
    QMenu                           favContext;
    CFavAction                     *pContextAct[MAX_NO_FAVOURITES];
    IncPlay::ePlayStates            ePlayState;
    QVector<CShortcutEx *>          vShortcutPool;
    int                             iDwnReqId;
    QStandardItemModel             *pModel;
    QChanListDelegate              *pDelegate;
    QVector<Ui::SShortCuts>         vShortCutTab;
    cparser::SAccountInfo           accountInfo;
    cparser::SGenreInfo             genreInfo;
    ulong                           ulStartFlags;
    Ui::STabWidget                  vodTabWidget;
    CIptvDefs                       metaKartina;
    QRecordTimerWidget              timerWidget;
    QUpdateNotifyDlg                updNotifier;
    QExpireNotifyDlg                expNotifier;
    QMenu                          *pFilterMenu;
    QStringFilterWidgetAction      *pFilterWidget;
    QMenu                          *pMnLangFilter;
    QWatchListDlg                  *pWatchList;
    bool                            bStayOnTop;
    QStackedLayout                 *stackedLayout;
    QVlcVideoWidget                *pVideoWidget;
    Ui::EDisplayMode                eCurDMode;
    Ui::EDisplayMode                eOldDMode;
    QRect                           rectBackup;
    QRect                           playWndRect;
    QAuthDlg                        authDlg;
    QWaitWidget                     waitWidget;
    reqItem_t                       reRequest;
    QVector<cparser::SEpg>          epgBuff;
    QTimer                          m_tTimeJump;
    QTimer                          m_tServiceTimeout;
    cparser::ServiceSettings        servSettings;
    int                             m_iJumpValue;
    QString                         m_StrSrvAuto;

protected:
    void setDisplayMode(Ui::EDisplayMode newMode);
    void fillShortCutTab();
    void touchLastOrBestCbx ();
    void touchGenreCbx();
    int FillChannelList (const QVector<cparser::SChan> &chanlist);
    int StartVlcRec (const QString &sURL, const QString &sChannel);
    int StartVlcPlay (const QString &sURL);
    void StartStreamDownload (const QString &sURL, const QString &sName, const QString &sFileExt = "ts");
    void TouchPlayCtrlBtns (bool bEnable = true);
    void touchVodNavBar(const cparser::SGenreInfo &gInfo);
    void TouchEpgNavi (bool bCreate);
    QString CleanShowName (const QString &str);
    void CreateSystray ();
    bool WantToStopRec ();
    void HandleFavourites ();
    void CleanContextMenu ();
    int  CheckCookie (const QString &cookie);
    int  AllowAction (IncPlay::ePlayStates newState);
    bool TimeJumpAllowed ();
    void InitShortCuts ();
    void ClearShortCuts ();
    void savePositions ();
    void initDialog ();
    int  getCurrentCid();
    void retranslateShortcutTable();
    void correctEpgOffset();
    int  grantAdultAccess (bool bProtected);
    void toggleFullscreen();
    QString recFileName (const QString& name, QString& ext);
    void loginOnly(const QString& resp);
    QString createVideoInfo(bool checkVod = true);
    void strSrvAuto(const QString& resp);
    void speedTestData(const QString& resp);

    virtual void changeEvent(QEvent *e);
    virtual void showEvent (QShowEvent * event);
    virtual void hideEvent (QHideEvent * event);
    virtual void closeEvent (QCloseEvent *event);
    virtual void keyPressEvent (QKeyEvent *event);

private slots:
    void slotBwd();
    void slotFwd();
    void slotToggleFullscreen();
    void slotWindowed();
    void slotDoVodSearch();
    void on_cbxGenre_activated(int index);
    void on_cbxLastOrBest_activated(int index);
    void on_cbxVodLang_activated(int index);
    void slotFontSmaller();
    void slotFontLarger();
    void slotStop();
    void on_pushTimerRec_clicked();
    void on_lineSearch_returnPressed();
    void on_btnSearch_clicked();
    void on_channelList_doubleClicked(const QModelIndex & index);
    void slotbtnNext_clicked();
    void slotbtnBack_clicked();
    void on_pushAbout_clicked();
    void on_cbxChannelGroup_activated(int index);
    void slotPlay();
    void slotRecord();
    void on_pushSettings_clicked();
    void on_cbxSites_activated (int index);
    void on_btnPrevSite_clicked();
    void on_btnNextSite_clicked();
    void on_pushLive_clicked();
    void on_channelList_clicked(QModelIndex index);
    void on_pushFilter_clicked();
    void slotChanList (const QString &str);
    void slotEPG(const QString &str, bool bExtEpg = false);
    void slotEPGCurrent (const QString &str);
    void slotStreamURL (const QString &str);
    void slotArchivURL (const QString &str);
    void slotCookie (const QString &str);
    void slotTimeShift (const QString &str);
    void slotEpgAnchor (const QUrlEx &link);
    void slotReloadLogos ();
    void slotDayTabChanged (int iIdx);
    void slotSetSServer (QString sIp);
    void slotSetStrStd (QString s);
    void slotTimerRecActive (int iState);
    void slotTimerRecordDone ();
    void slotVlcStarts (int iState);
    void slotVlcEnds (int iState);
    void slotShutdown ();
    void slotChanListContext (const QPoint &pt);
    void slotChgFavourites (QAction *pAct);
    void slotHandleFavAction (QAction *pAct);
    void slotFavBtnContext (const QPoint &pt);
    void slotSplashScreen ();
    void slotIncPlayState (int);
    void slotLogout (const QString &str);
    void slotDownloadStarted (int id, QString sFileName);
    void slotGotVodGenres (const QString &str);
    void slotGotVideos (const QString &str, bool bVodFavs = false);
    void slotVodAnchor (const QUrlEx &link);
    void slotGotVideoInfo (const QString &str);
    void slotVodURL(const QString &str);
    void slotSetBitrate (int iRate);
    void slotSetTimeShift (int iShift);
    void slotChannelDown();
    void slotChannelUp();
    void slotToggleEpgVod();
    void slotCurrentChannelChanged(const QModelIndex & current);
    void slotPlayNextChannel();
    void slotPlayPreviousChannel();
    void slotStartConnectionChain();
    void slotUpdateAnswer (const QString& str);
    void slotCheckArchProg(ulong ulArcGmt);
    void slotKartinaErr (QString str, int req, int err);
    void slotKartinaResponse(QString resp, int req);
    void slotUnused(const QString &str);
    void slotRefreshChanLogos ();
    void slotPCodeChangeResp (const QString &str);
    void slotAddFav(int cid);
    void on_pushHelp_clicked();
    void slotDownStreamRequested (int id);
    void slotUpdateChannelList (const QList<int> &cidList = QList<int>());
    void slotRecordTimerEnded ();
    void slotGlobalError (int iType, const QString& sCaption, const QString& sDescr);
    void slotTriggeredLogout();
    void slotFilterChannelList(QString filter);
    void slotLangFilterChannelList(QAction *pAct);
    void slotALang(const QString& str);
    void slotWlClick(QUrlEx url);
    void on_pushWatchList_clicked();
    void slotUpdWatchListCount();
    void slotStayOnTop(bool on);
    void slotPlayHls(const QString& s);
    void slotVodLang(const QString &str);
    void slotChgFontSize (int i);
    void slotFinallyJump();
    void slotService(const QString& s = QString());
    void on_btnCleanVodSearch_clicked();
    void slotReqStrSrvAuto();
    void slotReqSpeedTestData();
    void rerequestStream(const QString &resp);

signals:
    void sigShow ();
    void sigHide ();
    void sigToggleFullscreen ();
    void sigToggleAspectRatio ();
    void sigToggleCropGeometry ();
    void sigLCDStateChange (int iState);
    void sigJmpFwd ();
    void sigJmpBwd ();
    void sigShowInfoUpdated();
    void sigFullScreenToggled (int on);
    void sigLockParentalManager();
    void sigWindowed(int);
    void sigWLRecEnded();
    void sigOverlay (const QString& s, int iTmOut);
    void sigSpeedTestData(QSpeedDataVector spdData);
};

#endif /* __011910__RECORDER_H */
/************************* History ***************************\
| $Log$
\*************************************************************/

