/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 20.06.2010 / 19:50:35
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#ifndef __062010__CVIDEOFRAME_H
   #define __062010__CVIDEOFRAME_H

#include <QFrame>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QVector>
#include <QTimer>

#include "clogfile.h"
#include "cshortcutex.h"


/********************************************************************\
|  Class: CVideoFrame
|  Date:  21.06.2010 / 11:00:28
|  Author: Jo2003
|  Description: extend QFrame for video functions
|
\********************************************************************/
class CVideoFrame : public QFrame
{
   Q_OBJECT

public:
   CVideoFrame(QWidget * parent = NULL, Qt::WindowFlags f = 0);
   virtual ~CVideoFrame();
   void setShortCuts (QVector<CShortcutEx *> *pvSc);

protected:
   int  fakeShortCut (const QKeySequence &seq);

   virtual void mouseDoubleClickEvent(QMouseEvent *pEvent);
   virtual void mouseMoveEvent (QMouseEvent *pEvent);
   virtual void keyPressEvent (QKeyEvent *pEvent);

private:
   QVector<CShortcutEx *> *pvShortcuts;
   QTimer                  tMouseHide;
   int keyEventToKeySequence (QKeyEvent *pEvent, QKeySequence & seq);

signals:
   void sigToggleFullscreen ();

public slots:
   void slotHideMouse ();
};

#endif // __062010__CVIDEOFRAME_H
/************************* History ***************************\
| $Log$
\*************************************************************/
