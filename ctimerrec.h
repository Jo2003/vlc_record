/*********************** Information *************************\
| $HeadURL$
|
| Author: Joerg Neubert
|
| Begin: 24.01.2010 / 15:41:34
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#ifndef __012410__CTIMERREC_H
   #define __012410__CTIMERREC_H

#include <QtGui/QDialog>
#include <QDateTime>
#include <QTimer>
#include <QVector>
#include <QMap>
#include <QXmlStreamReader>
#include <QFile>
#include <QString>
#include <QTextStream>

#include "defdef.h"
#include "ckartinaxmlparser.h"

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

   struct SRecEntry
   {
      uint id;
      int  cid;
      uint uiStart;
      uint uiEnd;
      QString sName;
   };
}

class CTimerRec : public QDialog
{
    Q_OBJECT

public:
   CTimerRec(QWidget *parent = 0);
   virtual ~CTimerRec();

   void SetTimeShift (int iTs);
   void SetChanList  (const QVector<cparser::SChan> &chanList);
   void SetRecInfo (uint uiStart, uint uiEnd, int cid);
   void SetLogoPath (const QString &str);
   int SaveRecordList ();
   int ReadRecordList ();
   int AddRow (const rec::SRecEntry &entry);

   void InitTab ();

protected:
   void changeEvent(QEvent *e);

private:
   Ui::CTimerRec *r_ui;
   int     iTimeShift;
   QString sLogoPath;
   QMap<uint, rec::SRecEntry> JobList;
   QMap<int, rec::SChanEntry> ChanList;
   QString sListFile;
   uint    uiActId;

private slots:
    void on_pushOK_clicked();
    void on_btnSet_clicked();
};

#endif /* __012410__CTIMERREC_H */
/************************* History ***************************\
| $Log$
\*************************************************************/
