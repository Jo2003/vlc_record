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

#include <QtGui/QDialog>
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

#include "csettingsdlg.h"
#include "ckartinaclnt.h"
#include "ckartinaxmlparser.h"
#include "cwaittrigger.h"
#include "templates.h"
#include "caboutdialog.h"
#include "cchanlogo.h"
#include "ctimerrec.h"
#include "cvlcctrl.h"
#include "ctranslit.h"
#include "cfavaction.h"

// for logging ...
extern CLogFile VlcLog;

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
    Recorder(QTranslator *trans = 0, QWidget *parent = 0);
    ~Recorder();

public slots:
    virtual void show();

private:
    Ui::Recorder                  *ui;
    CSettingsDlg                   Settings;
    CKartinaClnt                   KartinaTv;
    CKartinaXMLParser              XMLParser;
    CWaitTrigger                   Trigger;
    QTranslator                   *pTranslator;
    QTimer                         Refresh;
    bool                           bLogosReady;
    CChanLogo                      dwnLogos;
    QString                        sLogoPath;
    int                            iEpgOffset;
    QTabBar                       *pEpgNavbar;
    CTimerRec                      timeRec;
    uint                           uiArchivGmt;
    QSystemTrayIcon                trayIcon;
    QRect                          sizePos;
    CVlcCtrl                       vlcCtrl;
    CTranslit                      translit;
    int                            iFontSzChg;
    QList<int>                     lFavourites;
    QToolButton                   *pFavBtn[MAX_NO_FAVOURITES];
    CFavAction                    *pFavAct[MAX_NO_FAVOURITES];
    QMap<int, QString>             chanMap;
    QMenu                          favContext;
    CFavAction                    *pContextAct[MAX_NO_FAVOURITES];
    IncPlay::ePlayStates           ePlayState;

protected:
    int FillChannelList (const QVector<cparser::SChan> &chanlist);
    int StartVlcRec (const QString &sURL, const QString &sChannel, bool bArchiv = false);
    int StartVlcPlay (const QString &sURL, bool bArchiv = false);
    void TouchPlayCtrlBtns (bool bEnable = true);
    void SetProgress (const uint &start, const uint &end);
    void changeEvent(QEvent *e);
    void TouchEpgNavi (bool bCreate);
    QString CleanShowName (const QString &str);
    void CreateSystray ();
    bool WantToStopRec ();
    bool WantToClose ();
    void HandleFavourites ();
    void FillChanMap (const QVector<cparser::SChan> &chanlist);
    void CleanContextMenu ();
    int  CheckCookie (const QString &cookie);
    int  AllowAction (IncPlay::ePlayStates newState);
    void FillStateMap ();
    QString GetStatePixmap (IncPlay::ePlayStates state);
    bool TimeJumpAllowed ();

    virtual void showEvent (QShowEvent * event);
    virtual void hideEvent (QHideEvent * event);
    virtual void closeEvent (QCloseEvent *event);
    virtual void keyPressEvent (QKeyEvent *event);

private slots:
    void on_btnFontSmaller_clicked();
    void on_btnFontLarger_clicked();
    void on_pushStop_clicked();
    void on_pushTimerRec_clicked();
    void on_lineSearch_returnPressed();
    void on_btnSearch_clicked();
    void on_listWidget_itemDoubleClicked(QListWidgetItem* item);
    void slotbtnNext_clicked();
    void slotbtnBack_clicked();
    void on_pushAbout_clicked();
    void on_cbxChannelGroup_activated(int index);
    void on_listWidget_currentRowChanged(int currentRow);
    void on_cbxTimeShift_currentIndexChanged(QString str);
    void on_pushPlay_clicked();
    void on_pushRecord_clicked();
    void on_pushSettings_clicked();
    void slotErr (QString str);
    void slotChanList (QString str);
    void slotEPG(QString str);
    void slotStreamURL (QString str);
    void slotArchivURL (QString str);
    void slotServerForm (QString str);
    void slotCookie (QString sCookie);
    void slotTimeShift ();
    void slotEpgAnchor (const QUrl & link);
    void slotLogosReady ();
    void slotReloadLogos ();
    void slotDayTabChanged (int iIdx);
    void slotSetSServer (int iSrv);
    void slotTimerRecActive (int iState);
    void slotTimerRecordDone ();
    void slotVlcStarts (int iState);
    void slotVlcEnds (int iState);
    void slotShutdown ();
    void slotSystrayActivated (QSystemTrayIcon::ActivationReason reason);
    void slotChanListContext (const QPoint &pt);
    void slotChgFavourites (QAction *pAct);
    void slotHandleFavAction (QAction *pAct);
    void slotFavBtnContext (const QPoint &pt);
    void slotSplashScreen ();
    void slotIncPlayState (int);

signals:
    void sigShow ();
    void sigHide ();
    void sigToggleFullscreen ();
    void sigToggleAspectRatio ();
    void sigLCDStateChange (const QPixmap &pic);
    void sigTimeJmp (int);
};

#endif /* __011910__RECORDER_H */
/************************* History ***************************\
| $Log$
\*************************************************************/

