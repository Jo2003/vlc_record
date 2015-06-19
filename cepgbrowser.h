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

#include "qtextbrowserex.h"
#include <QMap>
#include "templates.h"
#include "defdef.h"
#include "api_inc.h"

namespace epg
{
   struct SShow
   {
      // make sure start and end are initiated with 0 ...
      SShow():uiStart(0), uiEnd(0){}
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
class CEpgBrowser : public QTextBrowserEx
{
   Q_OBJECT

public:
    CEpgBrowser(QWidget *parent = 0);
    void DisplayEpg(QVector<cparser::SEpg> epglist,
                    const QString &sName, int iChanID,
                    uint uiGmt, bool bHasArchiv, int iTs,
                    bool bExt);

    void recreateEpg ();

    int  GetCid () { return iCid; }
    const epg::SShow epgShow (uint uiTimeT);
    void EnlargeFont ();
    void ReduceFont ();
    void ChangeFontSize (int iSz);
    QMap<uint, epg::SShow> exportProgMap();
    uint epgTime() { return uiTime; }

protected:
    bool NowRunning (const QDateTime &startThis, const QDateTime &startNext = QDateTime());
    QString createHtmlCode();
    void scrollTo();

private:
    int                    _iTs;
    int                    iCid;
    bool                   bArchive;
    bool                   bExtEPG;
    bool                   hasCurrent;
    uint                   uiTime;
    QString                sChanName;
    QMap<uint, epg::SShow> mProgram;
};

#endif /* __011810__CEPGBROWSER_H */
/************************* History ***************************\
| $Log$
\*************************************************************/

