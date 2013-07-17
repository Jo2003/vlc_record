/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qvlcvideowidget.h
 *
 *  @author   Jo2003, inspired by Tadej Novak (VLC-Qt)
 *
 *  @date     08.02.2012
 *
 *  $Id$
 *
 *///------------------------- (c) 2012 by Jo2003  --------------------------
#ifndef __20120208_QVLCVIDEOWIDGET_H
   #define __20120208_QVLCVIDEOWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QMenu>
#include <QVector>
#include <QAction>
#include <QMutex>

#include "clogfile.h"
#include "cshortcutex.h"
#include "qoverlayedcontrol.h"

// ----------------------------------------------------------
//    namespace ...
// ----------------------------------------------------------
namespace vlcvid {

   enum eActType
   {
      ACT_Deinterlacing,
      ACT_ChgLang,
      ACT_Unknown = 255
   };

   struct SContextAction
   {
      SContextAction():actType(ACT_Unknown) {}
      eActType actType;
      QString  actName;
      QVariant actVal;
   };

   struct SContLang
   {
      int     id;
      QString desc;
      bool    current;
   };
}

typedef QVector<vlcvid::SContLang> QLangVector;

Q_DECLARE_METATYPE(vlcvid::SContextAction)
Q_DECLARE_METATYPE(QLangVector)

//---------------------------------------------------------------------------
//! \class   QVlcVideoWidget
//! \date    08.02.2012
//! \author  Jo2003
//! \brief   a widget to handle VLC video display
//---------------------------------------------------------------------------
class QVlcVideoWidget : public QWidget
{
    Q_OBJECT

public:
   QVlcVideoWidget(QWidget *parent = 0);
   virtual ~QVlcVideoWidget();
   void setShortCuts (QVector<CShortcutEx *> *pvSc);
   WId widgetId();
   void raiseRender();

protected:
   virtual void mouseDoubleClickEvent(QMouseEvent *event);
   virtual void mouseMoveEvent(QMouseEvent *event);
   virtual void mousePressEvent(QMouseEvent *event);
   virtual void wheelEvent(QWheelEvent *event);
   virtual void keyPressEvent (QKeyEvent *event);
   virtual void changeEvent(QEvent *event);

   int fakeShortCut (const QKeySequence &seq);
   int keyEventToKeySequence (QKeyEvent *event, QKeySequence &seq);

   void touchContextMenu ();
   int setCurrentATrack (int id);
   int getCurrentATrack ();

private:
   QWidget                *_render;
   QTimer                 *_mouseHide;
   QVector<CShortcutEx *> *_shortcuts;
   bool                    _extFullScreen;
   QOverlayedControl      *_ctrlPanel;
   bool                    _mouseOnPanel;
   bool                    _panelPositioned;
   QMenu                  *_contextMenu;
   QLangVector             _langVector;
   QMutex                  _mtxLv;

signals:
   void fullScreen();
   void mouseShow(const QPoint pt);
   void mouseHide();
   void rightClick(const QPoint pt);
   void wheel (const bool up);
   void sigDeinterlace (bool);
   void sigNewATrack (int);

public slots:
   void toggleFullScreen ();
   void hideMouse ();
   void fullScreenToggled(int on);
   void slotMouseEntersPanel ();
   void slotMouseLeavesPanel ();
   void slotWheel(bool w);
   void slotUpdLangVector(QLangVector lv);

private slots:
   void slotCustContextMenu(QPoint pt);
   void slotContentActionTriggered (QAction *pAct);
};

#endif // __20120208_QVLCVIDEOWIDGET_H
