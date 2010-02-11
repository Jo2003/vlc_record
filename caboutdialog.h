/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 18.01.2010 / 16:12:40
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#ifndef __011810__CABOUTDIALOG_H
   #define __011810__CABOUTDIALOG_H

#include <QDialog>
#include <version_info.h>
#include "customization.h"

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
    CAboutDialog(QWidget *parent = 0);
    ~CAboutDialog();

protected:
    void changeEvent(QEvent *e);
    void FillInfo ();

private:
    Ui::CAboutDialog *ui;
    QString strAbout;
};

#endif /* __011810__CABOUTDIALOG_H */
/************************* History ***************************\
| $Log$
\*************************************************************/

