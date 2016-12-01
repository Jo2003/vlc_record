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

#include "qtextbrowserex.h"
#include <QVector>
#include <QString>
#include <QFileInfo>
#include <QRegExp>
#include "templates.h"
#include "defdef.h"
#include "csettingsdlg.h"
#include "cpixloader.h"

/********************************************************************\
|  Class: CVodBrowser
|  Date:  21.12.2010 / 9:41
|  Author: Jo2003
|  Description: browser for vod stuff
|
\********************************************************************/
class CVodBrowser : public QTextBrowserEx
{
   Q_OBJECT

public:
    CVodBrowser(QWidget *parent = 0);
    ~CVodBrowser();

    void EnlargeFont ();
    void ReduceFont ();
    void ChangeFontSize (int iSz);
    void setPixCache (CPixLoader *pCache);

    void displayVodList (const QVector<cparser::SVodVideo> &vList, const QString &sGenre, bool backLink = false);
    void displayVideoDetails (const cparser::SVodVideo &sInfo);
    const QString& getName ();
    const QString& getShortContent ();
    uint  getLength();
    void setSettings (CSettingsDlg *pDlg);
    void deleteFromLastList (uint vIdx);
    void saveScrollPos();
    void recreateVodList();

protected:
    QString createVodListTableCell (const cparser::SVodVideo& entry, bool& delay);

private slots:
    void slotSetBufferedHtml();

private:
    QString        _name;
    QString        _shortContent;
    QString        _contentBuffer;
    uint           _uiLength;
    CSettingsDlg  *pSettings;
    CPixLoader    *pPixCache;
    QVector<cparser::SVodVideo> mLastList;
    QString                     mLastGenre;
    bool                        mBackLink;
    int                         mScrollPos;
};

#endif // __21122010_CVODBROWSER_H
/************************* History ***************************\
| $Log$
\*************************************************************/
