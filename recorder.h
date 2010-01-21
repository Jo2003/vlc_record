/*********************** Information *************************\
| $HeadURL$
|
| Author: Joerg Neubert
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

#include "csettingsdlg.h"
#include "ckartinaclnt.h"
#include "ckartinaxmlparser.h"
#include "cwaittrigger.h"
#include "templates.h"
#include "caboutdialog.h"
#include "cchanlogo.h"

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
|  Author: Joerg Neubert
|  Description: dialog class for the recorder gui
|
\********************************************************************/
class Recorder : public QDialog
{
    Q_OBJECT

public:
    Recorder(QTranslator *trans = 0, QWidget *parent = 0);
    ~Recorder();

private:
    Ui::Recorder                  *ui;
    CSettingsDlg                  *pSettings;
    CKartinaClnt                   KartinaTv;
    CKartinaXMLParser              XMLParser;
    CWaitTrigger                   Trigger;
    QTranslator                   *pTranslator;
    QTimer                         Refresh;
    bool                           bRecord;
    bool                           bLogosReady;
    CChanLogo                      dwnLogos;
    QString                        sLogoPath;
    int                            iEpgOffset;
    QTabBar                       *pEpgNavbar;

protected:
    int FillChannelList (QVector<cparser::SChan> chanlist);
    int StartVlcRec (const QString &sURL, const QString &sChannel, bool bArchiv = false);
    int StartVlcPlay (const QString &sURL, bool bArchiv = false);
    void EnableDisableDlg (bool bEnable = true);
    void SetProgress (const QString &start, const QString &end);
    void changeEvent(QEvent *e);
    void TouchEpgNavi (bool bCreate);

private slots:
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
    void slotCookie ();
    void slotTimeShift ();
    void slotEpgAnchor (const QUrl & link);
    void slotLogosReady ();
    void slotReloadLogos ();
    void slotDayTabChanged (int iIdx);
    void slotSetSServer (int iSrv);
    void slotSetHttpBuffer (int iTime);
};

#endif /* __011910__RECORDER_H */
/************************* History ***************************\
| $Log$
\*************************************************************/

