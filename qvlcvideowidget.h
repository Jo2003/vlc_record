/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/sunduk.tv/qvlcvideowidget.h $
 *
 *  @file     qvlcvideowidget.h
 *
 *  @author   Jo2003, inspired by Tadej Novak (VLC-Qt)
 *
 *  @date     08.02.2012
 *
 *  $Id: qvlcvideowidget.h 1500 2015-03-04 08:55:58Z Olenka.Joerg $
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
      ACT_TglMiniMd,
      ACT_StayOnTop,
      ACT_ShowCtrlPanel,
      ACT_Unknown = 255
   };

   enum ePanelPos
   {
      Pos_Wnd,
      Pos_FS,
      Pos_Ukwn = 255
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
#ifndef Q_OS_LINUX
   /// Double click doesn't work on Linux in case
   /// our render widget is transparent for mouse events.
   /// Another nice bug in Qt.
   virtual void mouseDoubleClickEvent(QMouseEvent *event);
#endif // Q_OS_LINUX
   virtual void mouseMoveEvent(QMouseEvent *event);
   virtual void mousePressEvent(QMouseEvent *event);
   virtual void wheelEvent(QWheelEvent *event);
   virtual void keyPressEvent (QKeyEvent *event);
   virtual void changeEvent(QEvent *event);

   int fakeShortCut (const QKeySequence &seq);
   int keyEventToKeySequence (QKeyEvent *event, QKeySequence &seq);

   void touchContextMenu ();
   int  setCurrentATrack (int id);
   int  getCurrentATrack ();

private:
   QWidget                *_render;
   QTimer                 *_mouseHide;
   QTimer                 *_placePanel;
   QTimer                 *_tOverlay;
#ifdef Q_OS_LINUX
   /// timer to emulate double click on Linux
   QTimer                 *_tDoubleClick;
#endif // Q_OS_LINUX
   QVector<CShortcutEx *> *_shortcuts;
   bool                    _extFullScreen;
   QOverlayedControl      *_ctrlPanel;
   bool                    _mouseOnPanel;
   bool                    _bWindowed;
   bool                    _bCPanelIfWnd;
   QMenu                  *_contextMenu;
   QLangVector             _langVector;
   QMutex                  _mtxLv;
   vlcvid::ePanelPos       _lastPos;

signals:
   void fullScreen();
   void mouseShow(const QPoint pt);
   void mouseHide();
   void rightClick(const QPoint pt);
   void wheel (const bool up);
   void sigDeinterlace (bool);
   void sigNewATrack (int);
   void sigWindowed ();
   void sigStayOnTop(bool);

public slots:
   void toggleFullScreen ();
   void hideMouse ();
   void fullScreenToggled(int on);
   void windowed(int on);
   void slotMouseEntersPanel ();
   void slotMouseLeavesPanel ();
   void slotWheel(bool w);
   void slotUpdLangVector(QLangVector lv);

private slots:
   void slotCustContextMenu(QPoint pt);
   void slotContentActionTriggered (QAction *pAct);
   void slotPositionCtrlPanel();
};

#endif // __20120208_QVLCVIDEOWIDGET_H
