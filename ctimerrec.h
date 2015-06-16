/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 24.01.2010 / 15:41:34
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#ifndef __012410__CTIMERREC_H
   #define __012410__CTIMERREC_H

#include <QtWidgets/QDialog>
#include <QDateTime>
#include <QTimer>
#include <QVector>
#include <QMap>
#include <QString>
#include <QTextStream>

#include "defdef.h"
#include "templates.h"
#include "csettingsdlg.h"
#include "cvlcctrl.h"
#include "cstreamloader.h"
#include "qhlscontrol.h"

//===================================================================
// namespace
//===================================================================
namespace Ui
{
   class  CTimerRec;
}

namespace rec
{
   struct SChanEntry
   {
      int     cid;
      QString Name;
   };

   enum eRecState
   {
      REC_READY,
      REC_STBY,
      REC_RUNNING,
      REC_DONE,
      REC_UNKNOWN = 255
   };

   struct SRecEntry
   {
      SRecEntry():dbId(0),id((uint)-1),cid(-1),iTimeShift(-1),uiStart(0),uiEnd(0),sName(""),eState(REC_UNKNOWN){}
      uint      dbId;
      uint      id;
      int       cid;
      int       iTimeShift;
      uint      uiStart;
      uint      uiEnd;
      QString   sName;
      eRecState eState;
   };
}

/********************************************************************\
|  Class: CTimerRec
|  Date:  19.01.2010 / 16:00:28
|  Author: Jo2003
|  Description: dialog class for timer record stuff
|
\********************************************************************/
class CTimerRec : public QDialog
{
    Q_OBJECT

public:
   CTimerRec(QWidget *parent = 0);
   virtual ~CTimerRec();

   void SetChanList  (const QVector<cparser::SChan> &chanList);
   void SetRecInfo (uint uiStart, uint uiEnd, int cid, const QString &name = QString());
   void SetSettings (CSettingsDlg *pSet);
   void SetVlcCtrl (CVlcCtrl *pCtrl);
   void SetStreamLoader (CStreamLoader *pLoader);
   void setHlsControl (QHlsControl *pCtrl);
   int  ReadRecordList ();
   int  AddRow (const rec::SRecEntry &entry);
   void AddJob (rec::SRecEntry &entry);
   void DelRow (uint uiId);
   void InitTab ();
   void StartTimer ();
   void ShutDown ();
   bool silentRec();

protected:
   virtual void changeEvent(QEvent *e);
   int  SanityCheck (const QDateTime &start, const QDateTime &end, uint uiUpdId = INVALID_ID);
   void delDbEntry (int id);

private:
   Ui::CTimerRec              *r_ui;
   QMap<uint, rec::SRecEntry>  JobList;
   QMap<int, rec::SChanEntry>  ChanList;
   rec::SRecEntry              actJob;
   uint                        uiActId;
   uint                        uiEdtId;
   QTimer                      recTimer;
   CSettingsDlg               *pSettings;
   CVlcCtrl                   *pVlcCtrl;
   CStreamLoader              *pStreamLoader;
   QHlsControl                *pHlsControl;
   int                         iReqId;

signals:
   void sigRecDone ();
   void sigRecActive (int);
   void sigShutdown ();

public slots:
   void slotTimerStreamUrl (const QString &str);

private slots:
   void on_btnDel_clicked();
   void on_tableRecordEntries_cellDoubleClicked(int row, int column);
   void on_btnSet_clicked();
   void slotRecTimer ();
   int  slotSaveRecordList ();
   void slotStreamReady (int Id, QString sName);
};

#endif /* __012410__CTIMERREC_H */
/************************* History ***************************\
| $Log$
\*************************************************************/
