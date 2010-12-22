/*=============================================================================\
| $HeadURL: https://vlc-record.googlecode.com/svn/trunk/vlc-record/ckartinaclnt.h $
|
| Author: Jo2003
|
| last changed by: $Author: Olenka.Joerg $
|
| Begin: Monday, January 04, 2010 16:11:14
|
| $Id: ckartinaclnt.h 357 2010-12-20 16:17:33Z Olenka.Joerg $
|
\=============================================================================*/
#ifndef __21122010_CVODBROWSER_H
   #define __21122010_CVODBROWSER_H

#include <QTextBrowser>
#include <QVector>
#include <QString>
#include <QFileInfo>

#include "clogfile.h"
#include "ckartinaxmlparser.h"
#include "templates.h"
#include "defdef.h"
#include "cdirstuff.h"

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

    void displayVodList (const QVector<cparser::SVodVideo> &vList, const QString &sGenre, int iIdx);
    void displayVideoDetails (const cparser::SVodVideo &sInfo);
    const QString& getName ();

private:
    QVector<cparser::SVodVideo> vVideos;
    QString sName;
};

#endif // __21122010_CVODBROWSER_H
