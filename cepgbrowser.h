/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
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
#include <QMap>
#include "ckartinaxmlparser.h"
#include "templates.h"

#include "clogfile.h"
#include "defdef.h"

namespace epg
{
   struct SShow
   {
      uint uiStart;
      uint uiEnd;
      QString sShowName;
      QString sShowDescr;
   };
}

/********************************************************************\
|  Class: CEpgBrowser
|  Date:  18.01.2010 / 16:06:32
|  Author: Jo2003
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

    void recreateEpg ();

    void SetTimeShift (int iTs) { iTimeShift = iTs; }
    int  GetCid () { return iCid; }
    int  GetTimeShift () { return iTimeShift; }
    const epg::SShow epgShow (uint uiTimeT);
    void EnlargeFont ();
    void ReduceFont ();
    void ChangeFontSize (int iSz);
    QMap<uint, epg::SShow> exportProgMap();

protected:
    bool NowRunning (const QDateTime &startThis, const QDateTime &startNext = QDateTime());
    bool ArchivAvailable (uint uiThisShow);
    QString createHtmlCode();

private:
    int                    iTimeShift;
    int                    iCid;
    bool                   bArchive;
    uint                   uiTime;
    QString                sChanName;
    QMap<uint, epg::SShow> mProgram;
};

#endif /* __011810__CEPGBROWSER_H */
/************************* History ***************************\
| $Log$
\*************************************************************/

