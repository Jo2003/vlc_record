/*=============================================================================\
| $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/cvodbrowser.h $
|
| Author: Jo2003
|
| last changed by: $Author: Olenka.Joerg $
|
| Begin: Monday, January 04, 2010 16:11:14
|
| $Id: cvodbrowser.h 1188 2013-08-28 10:48:01Z Olenka.Joerg $
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
#include "templates.h"
#include "defdef.h"
#include "cdirstuff.h"
#include "csettingsdlg.h"
#include "cpixloader.h"
#include "api_inc.h"

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
    void setPixCache (CPixLoader *pCache);

    void displayVodList (const QVector<cparser::SVodVideo> &vList, const QString &sGenre);
    void displayVideoDetails (const cparser::SVodVideo &sInfo);
    const QString& getName ();
    const QString& getShortContent ();
    void setSettings (CSettingsDlg *pDlg);

protected:
    QString createVodListTableCell (const cparser::SVodVideo& entry, bool& delay);

private slots:
    void slotSetBufferedHtml();

private:
    QString        _name;
    QString        _shortContent;
    QString        _contentBuffer;
    CSettingsDlg  *pSettings;
    CPixLoader    *pPixCache;
};

#endif // __21122010_CVODBROWSER_H
/************************* History ***************************\
| $Log$
\*************************************************************/
