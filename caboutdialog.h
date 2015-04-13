/*********************** Information *************************\
| $HeadURL: https://vlc-record.googlecode.com/svn/branches/sunduk.tv/caboutdialog.h $
|
| Author: Jo2003
|
| Begin: 18.01.2010 / 16:12:40
|
| Last edited by: $Author: Olenka.Joerg $
|
| $Id: caboutdialog.h 1265 2013-12-13 14:40:16Z Olenka.Joerg $
\*************************************************************/
#ifndef __011810__CABOUTDIALOG_H
   #define __011810__CABOUTDIALOG_H

#include <QDialog>
#include <QString>
#include <vlc/vlc.h>
#include <version_info.h>
#include "csettingsdlg.h"
#include "defdef.h"


namespace Ui {
    class CAboutDialog;
}

/********************************************************************\
|  Class: CAboutDialog
|  Date:  18.01.2010 / 16:13:10
|  Author: Jo2003
|  Description: simple about dialog class to show info
|
\********************************************************************/
class CAboutDialog : public QDialog {
    Q_OBJECT
public:
    CAboutDialog(QWidget *parent, QString sExpires);
    ~CAboutDialog();
    void ConnectSettings (CSettingsDlg *pSet);

protected:
    virtual void changeEvent(QEvent *e);
    void FillInfo (QString sExpires);

private:
    Ui::CAboutDialog *ui;
    QString strAbout;
};

#endif /* __011810__CABOUTDIALOG_H */
/************************* History ***************************\
| $Log$
\*************************************************************/

