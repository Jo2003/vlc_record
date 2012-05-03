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

#include "clogfile.h"
#include "cshortcutex.h"


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

   int fakeShortCut (const QKeySequence &seq);
   int keyEventToKeySequence (QKeyEvent *event, QKeySequence &seq);

private:
   QWidget                *_render;
   QTimer                 *_mouseHide;
   QVector<CShortcutEx *> *_shortcuts;
   bool                    _extFullScreen;

signals:
   void fullScreen();
   void mouseShow(const QPoint pt);
   void mouseHide();
   void rightClick(const QPoint pt);
   void wheel (const bool up);

public slots:
   void toggleFullScreen ();
   void hideMouse ();
   void fullScreenToggled(int on);
};

#endif // __20120208_QVLCVIDEOWIDGET_H
