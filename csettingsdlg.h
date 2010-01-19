/*********************** Information *************************\
| $HeadURL: svn://172.25.1.40/joergn/trunk/vlc-record/csettingsdlg.h $
| 
| Author: Joerg Neubert
|
| Begin: 19.01.2010 / 15:41:34
| 
| Last edited by: $Author: joergn $
| 
| $Id: csettingsdlg.h 176 2010-01-19 15:29:52Z joergn $
\*************************************************************/
#ifndef __011910__CSETTINGSDLG_H
   #define __011910__CSETTINGSDLG_H

#include <QtGui/QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QTranslator>

#include "cinifile.h"
#include "clogfile.h"
#include "defdef.h"

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
|  Author: Joerg Neubert
|  Description: dialog to configure needed settings
|
\********************************************************************/
class CSettingsDlg : public QDialog 
{
    Q_OBJECT

public:
    CSettingsDlg(QTranslator *trans = 0, QWidget *parent = 0);
    ~CSettingsDlg();

    QString GetVLCPath ();
    QString GetTargetDir ();
    QString GetUser ();
    QString GetPasswd ();
    QString GetProxyHost ();
    QString GetProxyUser ();
    QString GetProxyPasswd ();
    QString GetLanguage ();
    bool UseProxy ();
    bool AllowEros ();
    bool FixTime ();
    bool DoRefresh ();
    int GetRefrInt ();
    int GetProxyPort ();
    vlclog::eLogLevel GetLogLevel ();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::CSettingsDlg *m_ui;
    CIniFile IniFile;
    QTranslator *pTranslator;

signals:
    void sigReloadLogos ();

private slots:
    void on_pushDelLogos_clicked();
    void on_cbxLanguage_currentIndexChanged(QString str);
    void on_pushSave_clicked();
    void on_pushDir_clicked();
    void on_pushVLC_clicked();

 public slots:
    virtual int exec ();
};

#endif /* __011910__CSETTINGSDLG_H */
/************************* History ***************************\
| $Log$
\*************************************************************/

