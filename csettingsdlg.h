/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 19.01.2010 / 15:41:34
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#ifndef __011910__CSETTINGSDLG_H
   #define __011910__CSETTINGSDLG_H

#ifdef __QT_5
   #include <QtWidgets/QDialog>
#else
   #include <QtGui/QDialog>
#endif

#include <QFileDialog>
#include <QMessageBox>
#include <QDir>
#include <QTableWidget>
#include <QTimer>
#include <QSystemTrayIcon>

#include "defdef.h"
#include "cshortcutex.h"
#include "cshortcutgrabber.h"
#include "clogfile.h"
#include "cparser.h"

//===================================================================
// namespace
//===================================================================
namespace Ui
{
    class CSettingsDlg;
}

/********************************************************************\
|  Class: CSettingsDlg
|  Date:  19.01.2010 / 15:42:24
|  Author: Jo2003
|  Description: dialog to configure needed settings
|
\********************************************************************/
class CSettingsDlg : public QDialog
{
    Q_OBJECT

public:
    CSettingsDlg(QWidget *parent = 0);
    ~CSettingsDlg();

    QString GetVLCPath ();
    QString GetTargetDir ();
    QString GetUser ();
    QString GetPasswd ();
    QString GetErosPasswd ();
    QString GetProxyHost ();
    QString GetProxyUser ();
    QString GetProxyPasswd ();
    QString GetLanguage ();
    QString GetShutdownCmd ();
    QString GetPlayerModule ();
    QString GetCookie ();
    QString GetAPIServer ();
    QString getDeinlMode ();
    QString getStreamServer();

    bool UseProxy ();
    bool AllowEros ();
    bool HideToSystray ();
    bool AskForRecFile ();
    bool TranslitRecFile ();
    bool DetachPlayer ();
    int  getTimeShift();
    bool doubleClickToPlay();
    bool useGpuAcc();
    bool showAds();
    bool extEpg();

    int GetProxyPort ();
    int GetBufferTime ();
    vlclog::eLogLevel GetLogLevel ();

    void  SaveSplitterSizes (const QString &name, const QList<int> &sz);
    QList<int> GetSplitterSizes (const QString &name, bool *ok = NULL);
    void  SaveFavourites (const QList<int> &favList);
    QList<int> GetFavourites (bool *ok = NULL);
    void  SetStreamServerCbx (const QVector<cparser::SSrv>& vSrvList, const QString& sActSrv);
    void  SetBitrateCbx (const QVector<int>& vValues, int iActrate);
    void  SaveCookie (const QString &str);
    bool  DisableSplashScreen ();
    int   GetBitRate ();
    void  addShortCut (const QString& descr, const QString& target, const QString& slot, const QString& keys);
    void  delShortCut (const QString& target, const QString& slot);
    void  updateShortcutDescr(const QString& descr, const QString& target, const QString& slot);
    QString shortCut (const QString& target, const QString& slot) const;
    int  shortCutCount();
    void readSettings ();
    void fillTimeShiftCbx(const QVector<int> &vVals, int iAct);
    void saveChannel(int cid);
    int lastChannel();
    void saveEpgDay(const QString &dateString);
    QString lastEpgDay();
    uint libVlcVerboseLevel();
    void setAccountInfo(const cparser::SAccountInfo *pInfo);
    void setGeometry(const QByteArray &ba);
    QByteArray getGeometry();
    int setLanguage (const QString& lng);

    void setUser(const QString& str);
    void setPasswd(const QString& str);
    void setApiSrv(const QString& str);

    void setActiveStreamServer(const QString& str);
    void setActiveTimeshift(int val);
    void setActiveBitrate(int val);
    void setActiveBuffer(int val);

    int  getFontDelta ();
    void setFontDelta (int i);

protected:
    virtual void changeEvent(QEvent *e);
    bool checkBitrateAndTimeShift(int iBitRate, int iTimeShift, const QString& what);

private:
    Ui::CSettingsDlg  *m_ui;
    QString            sTempPasswd;
    CShortcutEx       *pShortApiServer;
    CShortcutEx       *pShortVerbLevel;
    QVector<float>     vBuffs;
    QVector<cparser::SChan>      channelVector;
    QVector<cparser::SVodRate>   vodRatesVector;
    const cparser::SAccountInfo *pAccountInfo;
    bool                         bSettingsRead;
    int                          m_iServerBitrate;
    int                          m_iServerTimeShift;

signals:
    void sigReloadLogos ();
    void sigSetServer (QString sIp);
    void sigSetBitRate (int iRate);
    void sigSetBuffer (int iBuffer);
    void sigSetTimeShift (int iShift);
    void sigFontDeltaChgd (int i);

private slots:
    void on_btnResetShortcuts_clicked();
    void on_checkAdvanced_clicked(bool checked);
    void on_pushDelLogos_clicked();
    void on_pushSave_clicked();
    void on_pushDir_clicked();
    void on_pushVLC_clicked();
    void slotEnableApiServer ();
    void slotEnableVlcVerbLine ();
    void on_cbxStreamServer_activated(int index);
    void on_cbxBitRate_activated(int index);
    void on_cbxTimeShift_activated(int index);
    void on_btnSaveExitManager_clicked();
    void on_btnEnterManager_clicked();
    void slotLockParentalManager ();
    void on_btnChgPCode_clicked();
    void on_linePasswd_returnPressed();
    void on_cbxLanguage_currentIndexChanged(const QString &lng);
    void on_cbxLanguage_activated(const QString &lng);
    void on_spinBoxFontDelta_valueChanged(int arg1);

public slots:
    void slotSplashStateChgd (bool bChecked);
    void slotBuildChanManager (const QString &str);
    void slotBuildVodManager (const QString &str);
    void slotNewPCodeSet ();
    void slotEnablePCodeForm ();
};

#endif /* __011910__CSETTINGSDLG_H */
/************************* History ***************************\
| $Log$
\*************************************************************/

