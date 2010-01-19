/*********************** Information *************************\
| $HeadURL$
|
| Author: Joerg Neubert
|
| Begin: 18.01.2010 / 16:05:56
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#ifndef __011810__CEPGBROWSER_H
   #define __011810__CEPGBROWSER_H

#include <QTextBrowser>
#include "ckartinaxmlparser.h"
#include "templates.h"

#include "clogfile.h"
#include "defdef.h"

/********************************************************************\
|  Class: CEpgBrowser
|  Date:  18.01.2010 / 16:06:32
|  Author: Joerg Neubert
|  Description: textbrowser with epg functionality
|
\********************************************************************/
class CEpgBrowser : public QTextBrowser
{
   Q_OBJECT

public:
    CEpgBrowser(QWidget *parent = 0);
    void DisplayEpg(QVector<cparser::SEpg> epglist,
                    const QString &sName, int iChanID,
                    uint uiGmt, bool bHasArchiv);

    void SetTimeShift (int iTs) { iTimeShift = iTs; }
    void SetLogoDir (const QString &sDir) { sLogoDir = sDir; }
    int  GetCid () { return iCid; }

protected:
    bool NowRunning (const QDateTime &startThis, const QDateTime &startNext = QDateTime());
    bool ArchivAvailable (uint uiThisShow, uint uiNextShow = 0);

private:
    int     iTimeShift;
    int     iCid;
    QString sLogoDir;
};

#endif /* __011810__CEPGBROWSER_H */
/************************* History ***************************\
| $Log$
\*************************************************************/

