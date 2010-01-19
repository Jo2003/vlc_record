/*********************** Information *************************\
| $HeadURL: svn://172.25.1.40/joergn/trunk/vlc-record/recorder.h $
| 
| Author: Joerg Neubert
|
| Begin: 19.01.2010 / 15:59:00
| 
| Last edited by: $Author: joergn $
| 
| $Id: recorder.h 176 2010-01-19 15:29:52Z joergn $
\*************************************************************/
#ifndef __011910__RECORDER_H
   #define __011910__RECORDER_H

#include <QtGui/QDialog>
#include <QString>
#include <QFile>
#include <QMessageBox>
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

protected:
    int FillChannelList (QVector<cparser::SChan> chanlist);
    int StartVlcRec (const QString &sURL, const QString &sChannel, bool bArchiv = false);
    int StartVlcPlay (const QString &sURL, bool bArchiv = false);
    void EnableDisableDlg (bool bEnable = true);
    void SetProgress (const QString &start, const QString &end);
    void changeEvent(QEvent *e);

private slots:
    void on_btnNext_clicked();
    void on_btnBack_clicked();
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
    void slotArchivAnchor (const QUrl & link);
    void slotLogosReady ();
    void slotReloadLogos ();
};

#endif /* __011910__RECORDER_H */
/************************* History ***************************\
| $Log$
\*************************************************************/

