/*=============================================================================\
| $HeadURL$
|
| Author: Jo2003
|
| last changed by: $Author$
|
| Begin: Monday, January 04, 2010 16:11:14
|
| $Id$
|
\=============================================================================*/
#ifndef __21122010_CVODBROWSER_H
   #define __21122010_CVODBROWSER_H

#include <QTextBrowser>
#include <QVector>
#include <QString>
#include <QFileInfo>
#include <QRegExp>

#include "clogfile.h"
#include "ckartinaxmlparser.h"
#include "templates.h"
#include "defdef.h"
#include "cdirstuff.h"
#include "csettingsdlg.h"

/********************************************************************\
|  Class: CVodBrowser
|  Date:  21.12.2010 / 9:41
|  Author: Jo2003
|  Description: browser for vod stuff
|
\********************************************************************/
class CVodBrowser : public QTextBrowser
{
   Q_OBJECT

public:
    CVodBrowser(QWidget *parent = 0);
    ~CVodBrowser();

    void EnlargeFont ();
    void ReduceFont ();
    void ChangeFontSize (int iSz);

    void displayVodList (const QVector<cparser::SVodVideo> &vList, const QString &sGenre, bool bSaveList = true);
    void displayVideoDetails (const cparser::SVodVideo &sInfo);
    const QString& getName ();
    const QString& getShortContent ();
    void setSettings (CSettingsDlg *pDlg);

private:
    QVector<cparser::SVodVideo> vVideos;
    QString sName;
    QString sShortContent;
    CSettingsDlg *pSettings;
};

#endif // __21122010_CVODBROWSER_H
/************************* History ***************************\
| $Log$
\*************************************************************/
