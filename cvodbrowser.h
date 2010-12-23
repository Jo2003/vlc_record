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

namespace vodbrowser {
   enum eSearchArea
   {
      IN_TITLE,
      IN_DESCRIPTION,
      IN_YEAR,
      IN_DIRECTOR,
      IN_ACTORS,
      IN_EVERYWHERE,
      IN_UNKNOWN
   };
}

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

    void displayVodList (const QVector<cparser::SVodVideo> &vList, const QString &sGenre, bool bSaveList = true);
    void displayVideoDetails (const cparser::SVodVideo &sInfo);
    const QString& getName ();
    void findVideos (const QString &str, vodbrowser::eSearchArea eArea = vodbrowser::IN_TITLE);
    void setSettings (CSettingsDlg *pDlg);

private:
    QVector<cparser::SVodVideo> vVideos;
    QString sName;
    CSettingsDlg *pSettings;
};

#endif // __21122010_CVODBROWSER_H
/************************* History ***************************\
| $Log$
\*************************************************************/
