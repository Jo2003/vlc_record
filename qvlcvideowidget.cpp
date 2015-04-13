/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/sunduk.tv/qvlcvideowidget.cpp $
 *
 *  @file     qvlcvideowidget.cpp
 *
 *  @author   Jo2003, inspired by Tadej Novak (VLC-Qt)
 *
 *  @date     08.02.2012
 *
 *  $Id: qvlcvideowidget.cpp 1505 2015-03-04 09:57:41Z Olenka.Joerg $
 *
 *///------------------------- (c) 2012 by Jo2003  --------------------------
#include "qvlcvideowidget.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QRegExp>
#include <QStackedLayout>
#include <QPixmap>
#include <QMessageBox>

#include "qfusioncontrol.h"

// fusion control ...
extern QFusionControl missionControl;

// log file functions ...
extern CLogFile VlcLog;

//---------------------------------------------------------------------------
//
//! \brief   constructs QVlcVideoWidget object
//
//! \author  Jo2003
//! \date    08.02.2012
//
//! \param   parent pointer to parent widget
//
//! \return  --
//---------------------------------------------------------------------------
QVlcVideoWidget::QVlcVideoWidget(QWidget *parent) :
   QWidget(parent),
   _render(NULL),
   _mouseHide(0),
   _placePanel(0),
   _shortcuts(0),
   _extFullScreen(false),
   _ctrlPanel(NULL),
   _mouseOnPanel(false),
   _bWindowed(false),
   _bCPanelIfWnd(true),
   _contextMenu(NULL),
   _lastPos(vlcvid::Pos_Ukwn)
{
   setMouseTracking(true);

   QVBoxLayout *pLayout = new QVBoxLayout();
   _render              = new QWidget(this);
   _mouseHide           = new QTimer(this);
   _placePanel          = new QTimer(this);
#ifdef Q_OS_LINUX
   /// double click emulator on Linux only
   _tDoubleClick        = new QTimer(this);
   _tDoubleClick->setSingleShot(true);
   _tDoubleClick->setInterval(500);
#elif defined Q_OS_WIN
   /// Strange! On Windows WA_TransparentForMouseEvents alone doesn't
   /// work. So we need to enable mouse tracking.
   _render->setMouseTracking(true);
#endif // Q_OS_WIN
   _render->setAttribute(Qt::WA_TransparentForMouseEvents);
   _render->setAutoFillBackground(true);
   _render->setObjectName("renderView");
   _render->setStyleSheet("QWidget#renderView {"
                          "background-color: black;"
                          "background-image: url(branding:video/logo);"
                          "background-repeat: no-repeat;"
                          "background-position: center middle;}");

   pLayout->setMargin(0);
   pLayout->addWidget(_render);
   setLayout(pLayout);

   // window flags for overlay widgets ...
   Qt::WindowFlags f = Qt::Window | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint;
#ifdef Q_OS_LINUX
   f |= Qt::X11BypassWindowManagerHint;
#endif // Q_OS_LINUX

   // create player control panels ...
   _ctrlPanel     = new QOverlayedControl(_render, f);

   // timer settings ...
   _placePanel->setSingleShot(true);

   // hide panel initially ...
   _ctrlPanel->hide();

   // use own context menu ...
   setContextMenuPolicy (Qt::CustomContextMenu);

   // fill context menu ...
   _contextMenu = new QMenu(this);
   touchContextMenu();

   connect (_mouseHide, SIGNAL(timeout()), this, SLOT(hideMouse()));
   connect (_placePanel, SIGNAL(timeout()), this, SLOT(slotPositionCtrlPanel()));
   connect (_ctrlPanel, SIGNAL(sigMouseAboveOverlay()), this, SLOT(slotMouseEntersPanel()));
   connect (_ctrlPanel, SIGNAL(sigMouseLeavesOverlay()), this, SLOT(slotMouseLeavesPanel()));
   connect (_ctrlPanel, SIGNAL(wheel(bool)), this, SLOT(slotWheel(bool)));

   // create context menu ...
   connect (this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCustContextMenu(QPoint)));

   // all content menu actions handled by one slot ...
   connect (_contextMenu, SIGNAL(triggered(QAction*)), this, SLOT(slotContentActionTriggered(QAction*)));
}

//---------------------------------------------------------------------------
//
//! \brief   destroys QVlcVideoWidget object
//
//! \author  Jo2003
//! \date    08.02.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
QVlcVideoWidget::~QVlcVideoWidget()
{
   // Qt memory management takes care of
   // object deletion e.g. will delete
   // all child objects!
}

//---------------------------------------------------------------------------
//
//! \brief   retranslate on language change event
//
//! \author  Jo2003
//! \date    16.07.2013
//
//! \param   event (QEvent *) pointer to change event
//
//! \return  --
//---------------------------------------------------------------------------
void QVlcVideoWidget::changeEvent(QEvent *event)
{
   switch (event->type())
   {
   case QEvent::LanguageChange:
      // retranslate context menu ...
      touchContextMenu();
      break;
   default:
      break;
   }

   QWidget::changeEvent(event);
}

//---------------------------------------------------------------------------
//
//! \brief   get window id from render view
//
//! \author  Jo2003
//! \date    08.02.2012
//
//! \param   --
//
//! \return  WId from render view
//---------------------------------------------------------------------------
WId QVlcVideoWidget::widgetId()
{
   return (_render) ? _render->winId() : (WId)0;
}

//---------------------------------------------------------------------------
//
//! \brief   override QWidget::mouseDoubleClickEvent()
//
//! \author  Jo2003
//! \date    08.02.2012
//
//! \param   event pointer to mouse event
//
//! \return  --
//---------------------------------------------------------------------------
#ifndef Q_OS_LINUX
/// workaround for Qt bug. Have a look at mousePressEvent() below for details!
void QVlcVideoWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
   emit fullScreen();

   QWidget::mouseDoubleClickEvent(event);
}
#endif // Q_OS_LINUX

//---------------------------------------------------------------------------
//
//! \brief   override QWidget::mouseMoveEvent()
//
//! \author  Jo2003
//! \date    08.02.2012
//
//! \param   event pointer to mouse event
//
//! \return  --
//---------------------------------------------------------------------------
void QVlcVideoWidget::mouseMoveEvent(QMouseEvent *event)
{
   if(isFullScreen() || _extFullScreen)
   {
      emit mouseShow(event->globalPos());
      QApplication::restoreOverrideCursor();
      _ctrlPanel->fadeIn();
      _ctrlPanel->raise();
      _mouseHide->start(1500);

      if (!_mouseOnPanel)
      {
         activateWindow();
         setFocus(Qt::OtherFocusReason);
      }
   }
   else if (_bWindowed && _bCPanelIfWnd)
   {
      _ctrlPanel->fadeIn();
      _ctrlPanel->raise();
      _mouseHide->start(1500);

      if (!_mouseOnPanel)
      {
         activateWindow();
         setFocus(Qt::OtherFocusReason);
      }
   }

   QWidget::mouseMoveEvent(event);
}

//---------------------------------------------------------------------------
//
//! \brief   override QWidget::mousePressEvent()
//
//! \author  Jo2003
//! \date    08.02.2012
//
//! \param   event pointer to mouse event
//
//! \return  --
//---------------------------------------------------------------------------
void QVlcVideoWidget::mousePressEvent(QMouseEvent *event)
{
   if(event->button() == Qt::RightButton)
   {
      QApplication::restoreOverrideCursor();
      emit rightClick(event->globalPos());
   }
#ifdef Q_OS_LINUX
   /// When making render widget transparent for mouse
   /// clicks we are not able to catch any double
   /// click event on Linux (wtf?)!
   /// So we catch single left clicks and try to find
   /// out if this is a double click!
   /// Another piece of shit in this code!
   else if(event->button() == Qt::LeftButton)
   {
      if(!_tDoubleClick->isActive())
      {
         _tDoubleClick->start();
      }
      else
      {
         mInfo(tr("Emulate double click!"));
         _tDoubleClick->stop();
         emit fullScreen();
      }
   }
#endif // Q_OS_LINUX
   QWidget::mousePressEvent(event);
}

//---------------------------------------------------------------------------
//
//! \brief   override QWidget::wheelEvent()
//
//! \author  Jo2003
//! \date    08.02.2012
//
//! \param   event pointer to wheel event
//
//! \return  --
//---------------------------------------------------------------------------
void QVlcVideoWidget::wheelEvent(QWheelEvent *event)
{
   emit wheel ((event->delta() > 0) ? true : false);

   QWidget::wheelEvent(event);
}

//---------------------------------------------------------------------------
//
//! \brief   wheel signal from overlay panel
//
//! \author  Jo2003
//! \date    08.02.2012
//
//! \param   w wheel up or down
//
//! \return  --
//---------------------------------------------------------------------------
void QVlcVideoWidget::slotWheel(bool w)
{
   emit wheel (w);
}

//---------------------------------------------------------------------------
//
//! \brief   switch between normal screen and full screen [slot]
//
//! \author  Jo2003
//! \date    08.02.2012
//
//! \param   event pointer to wheel event
//
//! \return  --
//---------------------------------------------------------------------------
void QVlcVideoWidget::toggleFullScreen()
{
   if (!isFullScreen())
   {
      // make it Window ...
      setWindowFlags(windowFlags() | Qt::Window);

      // show fullscreen ...
      showFullScreen();

      // make sure render window is raised ...
      _render->show();
      _render->activateWindow();
      _render->raise();

      // start mouse hiding ...
      _mouseHide->start(1000);
   }
   else
   {
      // embedd it ...
      setWindowFlags(windowFlags() & ~(Qt::WindowFlags)Qt::Window);

      // end fullscreen ...
      showNormal();

      QApplication::restoreOverrideCursor();
   }
}

//---------------------------------------------------------------------------
//
//! \brief   hide mouse when fullScreen [slot]
//
//! \author  Jo2003
//! \date    08.02.2012
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QVlcVideoWidget::hideMouse()
{
   if((isFullScreen() || _extFullScreen)
         && !_mouseOnPanel
         && !missionControl.isPopupActive()
         && !_contextMenu->isVisible()
         && (hasFocus() || _ctrlPanel->hasFocus() || _render->hasFocus()))
   {
      QApplication::setOverrideCursor(Qt::BlankCursor);
      _ctrlPanel->fadeOut();
      _mouseHide->stop();
      emit mouseHide();
   }
   else if(_bWindowed
         && !_mouseOnPanel
         && !missionControl.isPopupActive()
         && !_contextMenu->isVisible())
   {
      _ctrlPanel->fadeOut();
      _mouseHide->stop();
   }
}

//---------------------------------------------------------------------------
//
//! \brief   override QWidget::keyPressEvent()
//
//! \author  Jo2003
//! \date    08.02.2012
//
//! \param   event pointer to key event
//
//! \return  --
//---------------------------------------------------------------------------
void QVlcVideoWidget::keyPressEvent(QKeyEvent *event)
{
   QKeySequence seq;

   // make key sequence from key event ...
   if (!keyEventToKeySequence(event, seq))
   {
      fakeShortCut(seq);
   }

   QWidget::keyPressEvent(event);
}

//---------------------------------------------------------------------------
//
//! \brief   store a pointer to shortcuts vector
//
//! \author  Jo2003
//! \date    08.02.2012
//
//! \param   pvSc pointer to shortcuts vector
//
//! \return  --
//---------------------------------------------------------------------------
void QVlcVideoWidget::setShortCuts(QVector<CShortcutEx *> *pvSc)
{
   _shortcuts = pvSc;
}

//---------------------------------------------------------------------------
//
//! \brief   fake shortcut press if needed
//
//! \author  Jo2003
//! \date    08.02.2012
//
//! \param   seq ref. to key sequence
//
//! \return  --
//---------------------------------------------------------------------------
int QVlcVideoWidget::fakeShortCut (const QKeySequence &seq)
{
   int iRV = -1;
   QVector<CShortcutEx *>::const_iterator cit;

   if (_shortcuts)
   {
      // test all shortcuts if one matches the now incoming ...
      for (cit = _shortcuts->constBegin(); (cit != _shortcuts->constEnd()) && iRV; cit ++)
      {
         // is key sequence equal ... ?
         if ((*cit)->key() == seq)
         {
            // this shortcut matches ...
            mInfo (tr("Activate shortcut: %1").arg(seq.toString()));

            // fake shortcut keypress ...
            (*cit)->activate();

            // only one shortcut should match this sequence ...
            // so we're done!
            iRV = 0;
         }
      }
   }

   return iRV;
}

//---------------------------------------------------------------------------
//
//! \brief   fake shortcut press if needed
//
//! \author  Jo2003
//! \date    08.02.2012
//
//! \param   event pointer to key event
//! \param   seq ref. to key sequence
//
//! \return  0 --> key sequence filled
//! \return -1 --> not a key sequence
//---------------------------------------------------------------------------
int QVlcVideoWidget::keyEventToKeySequence(QKeyEvent *event, QKeySequence &seq)
{
   int iKey                    = event->key();
   Qt::KeyboardModifiers state = event->modifiers();
   QString text                = event->text();

   if ((iKey == Qt::Key_Control) || (iKey == Qt::Key_Shift) ||
       (iKey == Qt::Key_Meta)    || (iKey == Qt::Key_Alt)   ||
       (iKey == Qt::Key_Super_L) || (iKey == Qt::Key_AltGr))
   {
      // a single modifier can't be a shortcut / key sequence ...
      return -1;
   }

   // does shift modifier make sense ... ?
   if ((state & Qt::ShiftModifier) && ((text.size() == 0) || !text.at(0).isPrint() || text.at(0).isLetter() || text.at(0).isSpace()))
   {
      iKey |= Qt::SHIFT;
   }

   if (state & Qt::ControlModifier)
   {
      iKey |= Qt::CTRL;
   }

   if (state & Qt::MetaModifier)
   {
      iKey |= Qt::META;
   }

   if (state & Qt::AltModifier)
   {
      iKey |= Qt::ALT;
   }

   seq = QKeySequence(iKey);

   return 0;
}

//---------------------------------------------------------------------------
//
//! \brief   raise render view
//
//! \author  Jo2003
//! \date    27.04.2012
//
//! \return  --
//---------------------------------------------------------------------------
void QVlcVideoWidget::raiseRender()
{
   if (_render)
   {
      _render->raise();
   }
}

//---------------------------------------------------------------------------
//
//! \brief   position control panel on screen [slot]
//
//! \author  Jo2003
//! \date    19.12.2013
//
//! \return  --
//---------------------------------------------------------------------------
void QVlcVideoWidget::slotPositionCtrlPanel()
{
   vlcvid::ePanelPos actPos = vlcvid::Pos_Ukwn;

   if (_bCPanelIfWnd && _bWindowed)
   {
      actPos = vlcvid::Pos_Wnd;
   }
   else if(_extFullScreen)
   {
      actPos = vlcvid::Pos_FS;
   }

   if (((_lastPos == vlcvid::Pos_Ukwn) || (_lastPos != actPos))
       && (actPos != vlcvid::Pos_Ukwn))
   {
      switch (actPos)
      {
      // position for windowed mode ...
      case vlcvid::Pos_Wnd:
         _ctrlPanel->move(mapToGlobal(geometry().bottomLeft()));
         break;

      // position for fullscreen mode ...
      case vlcvid::Pos_FS:
         {
            QDesktopWidget *pDesc = QApplication::desktop();
            QRect            rect = pDesc->screenGeometry(_render);

            int x = rect.width() / 2 - _ctrlPanel->width() / 2;
            int y = rect.height() - (int)((float)_ctrlPanel->height() * 1.33);

            _ctrlPanel->setGeometry(x, y, _ctrlPanel->width(), _ctrlPanel->height());
         }
         break;

      default:
         break;
      }

      // mark as positioned ...
      _lastPos = actPos;
   }

   if ((_bCPanelIfWnd && _bWindowed) || _extFullScreen)
   {
      // make sure we have the focus ...
      _ctrlPanel->raise();
      _ctrlPanel->fadeIn();
      _ctrlPanel->activateWindow();
      _ctrlPanel->setFocus(Qt::OtherFocusReason);

      // start mouse hiding ...
      _mouseHide->start(1000);
   }
}

//---------------------------------------------------------------------------
//
//! \brief   fullscreen toggled, do cursor hide stuff
//
//! \author  Jo2003
//! \date    30.04.2012
//
//! \return  --
//---------------------------------------------------------------------------
void QVlcVideoWidget::fullScreenToggled(int on)
{
   _extFullScreen = !!on;

   _ctrlPanel->chgFullscreen(_extFullScreen);
   _ctrlPanel->chgWindowed(!_extFullScreen);

   if (_extFullScreen)
   {
      // position panel if needed ...
      _placePanel->start(25);
   }
   else
   {
      _ctrlPanel->hide();

      // restore visible cursor ...
      QApplication::restoreOverrideCursor();
   }
}

//---------------------------------------------------------------------------
//
//! \brief   windowed toggled, do cursor hide stuff
//
//! \author  Jo2003
//! \date    25.11.2013
//
//! \return  --
//---------------------------------------------------------------------------
void QVlcVideoWidget::windowed(int on)
{
   _bWindowed = !!on;

   _ctrlPanel->chgWindowed(_bWindowed);
   _ctrlPanel->chgFullscreen(!_bWindowed);

   if (_bWindowed)
   {
      // position panel if needed ...
      _placePanel->start(25);
   }
   else
   {
      _ctrlPanel->hide();
   }

   touchContextMenu();
}

//---------------------------------------------------------------------------
//
//! \brief   mouse enters control panel [slot]
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \return  --
//---------------------------------------------------------------------------
void QVlcVideoWidget::slotMouseEntersPanel()
{
   _mouseOnPanel = true;
}

//---------------------------------------------------------------------------
//
//! \brief   mouse leaves control panel [slot]
//
//! \author  Jo2003
//! \date    26.11.2012
//
//! \return  --
//---------------------------------------------------------------------------
void QVlcVideoWidget::slotMouseLeavesPanel()
{
   _mouseOnPanel = false;
}

//---------------------------------------------------------------------------
//
//! \brief   display custom context menu
//
//! \author  Jo2003
//! \date    15.07.2013
//
//! \return  --
//---------------------------------------------------------------------------
void QVlcVideoWidget::slotCustContextMenu(QPoint pt)
{
   _contextMenu->exec(mapToGlobal(pt));
}

//---------------------------------------------------------------------------
//
//! \brief   (re-)create context menu for video widget
//
//! \author  Jo2003
//! \date    16.07.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QVlcVideoWidget::touchContextMenu()
{
   QAction                *pAct;
   vlcvid::SContextAction  contAct;
   QRegExp                 rx("^.*\\[(.*)\\].*$");
   int                     i;
   QList<QAction*>         contActions = _contextMenu->actions();
   bool                    bIntl       = false;
   bool                    bOnTop      = false;
   QMenu*                  pSubm       = NULL;
   QString                 name;

   // in case of retranslation or update we should take care of
   // interlaced setting ...
   if (!contActions.isEmpty())
   {
      for (i = 0; i < contActions.count(); i++)
      {
         // search for current check states ...
         if (contActions.at(i)->objectName() == "Act_dintl")
         {
            bIntl  = contActions.at(i)->isChecked();
         }
         else if (contActions.at(i)->objectName() == "mini_sub")
         {
            pSubm  = contActions.at(i)->menu();
         }
      }
   }

   // take care for values in submenu ...
   if (pSubm)
   {
      contActions = pSubm->actions();

      if (!contActions.isEmpty())
      {
         for (i = 0; i < contActions.count(); i++)
         {
            // search for current check states ...
            if (contActions.at(i)->objectName() == "Act_stontop")
            {
               bOnTop = contActions.at(i)->isChecked();
               // we search only one ...
               break;
            }
         }
      }
   }

   // remove all menue entries ...
   _contextMenu->clear();

   // interlace stuff ...
   pAct = _contextMenu->addAction(tr("Deinterlacing"));

   // prepare data ...
   contAct.actType = vlcvid::ACT_Deinterlacing;
   contAct.actName = "n.a.";
   contAct.actVal.setValue(-1);

   // set data ...
   pAct->setData(QVariant::fromValue(contAct));
   pAct->setCheckable(true);
   pAct->setChecked(bIntl);
   pAct->setObjectName("Act_dintl");

   // add seperator ...
   pAct = _contextMenu->addSeparator();

   // --------------------------------------------------------
   // mini interface stuff ...
   // --------------------------------------------------------
   pSubm = _contextMenu->addMenu(tr("Minimal Interface"));
   pSubm->menuAction()->setObjectName("mini_sub");

   // minmal mode stuff ...
   pAct = pSubm->addAction(tr("Enable"));

   // prepare data ...
   contAct.actType = vlcvid::ACT_TglMiniMd;
   contAct.actName = "n.a.";
   contAct.actVal.setValue(-1);

   // set data ...
   pAct->setData(QVariant::fromValue(contAct));
   pAct->setCheckable(true);
   pAct->setObjectName("Act_minim");
   pAct->setChecked(_bWindowed);

   // stay on top stuff ...
   pAct = pSubm->addAction(tr("Stay on top"));

   // prepare data ...
   contAct.actType = vlcvid::ACT_StayOnTop;
   contAct.actName = "n.a.";
   contAct.actVal.setValue(-1);

   // set data ...
   pAct->setData(QVariant::fromValue(contAct));
   pAct->setCheckable(true);
   pAct->setObjectName("Act_stontop");
   pAct->setChecked(bOnTop);

   if (_bWindowed)
   {
      pAct->setEnabled(true);
   }
   else
   {
      pAct->setEnabled(false);
   }

   // show control panel stuff
   pAct = pSubm->addAction(tr("Show Control Panel"));

   // prepare data ...
   contAct.actType = vlcvid::ACT_ShowCtrlPanel;
   contAct.actName = "n.a.";
   contAct.actVal.setValue(-1);

   // set data ...
   pAct->setData(QVariant::fromValue(contAct));
   pAct->setCheckable(true);
   pAct->setObjectName("Act_showctrl");
   pAct->setChecked(_bCPanelIfWnd);

   if (_bWindowed)
   {
      pAct->setEnabled(true);
   }
   else
   {
      pAct->setEnabled(false);
   }

   // --------------------------------------------------------
   // language stuff ...
   // --------------------------------------------------------
   _mtxLv.lock();

   // only display audio change stuff if we have more then one entry ...
   if (_langVector.count() > 1)
   {
      // add seperator ...
      pAct = _contextMenu->addSeparator();

      // create submenu ---
      pSubm = _contextMenu->addMenu(tr("Audio Streams"));

      // go through language vector and add context menu entries ...
      for (i = 0; i < _langVector.count(); i++)
      {
         // try to grab language name from track description ...
         if (rx.indexIn(_langVector.at(i).desc) > -1)
         {
            name = rx.cap(1);
         }
         else
         {
            // filter doesn't match so give it a understandable name ...
            name = tr("Audio %1").arg(i + 1);
         }
#ifndef Q_OS_LINUX
         // create context menu entry ...
         pAct = pSubm->addAction(QIcon(_langVector.at(i).current ? ":/player/atrack" : ""), name);
#else
         // linux doesn't support icons on context menu within this context ...
         pAct = pSubm->addAction(name);
         pAct->setCheckable(true);
         pAct->setChecked(_langVector.at(i).current);
#endif
         // prepare data ...
         contAct.actType = vlcvid::ACT_ChgLang;
         contAct.actName = _langVector.at(i).desc;
         contAct.actVal.setValue(_langVector.at(i).id);

         // set data ...
         pAct->setData(QVariant::fromValue(contAct));
      }
   }
   _mtxLv.unlock();
}

//---------------------------------------------------------------------------
//
//! \brief   handle all(!) context menu action triggers such as deinterlace
//!          and audio track selection
//
//! \author  Jo2003
//! \date    16.07.2013
//
//! \param   pAct (QAction *) pointer to triggered action
//
//! \return  --
//---------------------------------------------------------------------------
void QVlcVideoWidget::slotContentActionTriggered(QAction *pAct)
{
   vlcvid::SContextAction actData;

   // do we have valid data attached to this action ... ?
   if (pAct->data().isValid())
   {
      // get data ...
      actData = pAct->data().value<vlcvid::SContextAction>();

      // which action to do ...
      switch(actData.actType)
      {
      // deinterlacing enabled / disabled ...
      case vlcvid::ACT_Deinterlacing:
         emit sigDeinterlace(pAct->isChecked());
         break;

      // toggle minimal mode ...
      case vlcvid::ACT_TglMiniMd:
         emit sigWindowed();
         break;

      // change stay on top mode ...
      case vlcvid::ACT_StayOnTop:
         emit sigStayOnTop(pAct->isChecked());
         break;

      // show control panel state changed ...
      case vlcvid::ACT_ShowCtrlPanel:
         // this only will happen when in windowed mode ...
         _bCPanelIfWnd = pAct->isChecked();

         if (_bCPanelIfWnd)
         {
            _placePanel->start(25);
         }
         break;

      // audio track selected ...
      case vlcvid::ACT_ChgLang:
         // has current track changed ( == 1) ... ?
         if (setCurrentATrack(actData.actVal.toInt()) == 1)
         {
            // changed -> adapt context menu ...
            touchContextMenu();

            // tell about new audio track ...
            emit sigNewATrack(actData.actVal.toInt());
         }
         break;

      default:
         break;
      }
   }
}

//---------------------------------------------------------------------------
//
//! \brief   new audio track data available, update context menu
//
//! \author  Jo2003
//! \date    16.07.2013
//
//! \param   lv (QLangVector) vector with audio track data
//
//! \return  --
//---------------------------------------------------------------------------
void QVlcVideoWidget::slotUpdLangVector(QLangVector lv)
{
   _mtxLv.lock();
   _langVector = lv;
   _mtxLv.unlock();
   touchContextMenu();
}

//---------------------------------------------------------------------------
//
//! \brief   set new current track in audio track vector
//
//! \author  Jo2003
//! \date    16.07.2013
//
//! \param   id (int) id of new track
//
//! \return  -1 --> track id not found;
//!           0 --> current track id hasn't changed;
//!           1 --> new current track successfully set
//---------------------------------------------------------------------------
int QVlcVideoWidget::setCurrentATrack (int id)
{
   int iRV = -1;
   QLangVector::iterator it;

   _mtxLv.lock();
   for (it = _langVector.begin(); it != _langVector.end(); it++)
   {
      if ((*it).id == id)
      {
         // changed (1) or unchanged (0) ?
         iRV = (*it).current ? 0 : 1;

         (*it).current = true;
      }
      else
      {
         (*it).current = false;
      }
   }
   _mtxLv.unlock();

   return iRV;
}

//---------------------------------------------------------------------------
//
//! \brief   get current audio track
//
//! \author  Jo2003
//! \date    16.07.2013
//
//! \param   --
//
//! \return  -1 --> nothing found
//!        else --> current track id
//---------------------------------------------------------------------------
int QVlcVideoWidget::getCurrentATrack ()
{
   int i, iRV = -1;

   _mtxLv.lock();
   for (i = 0; i < _langVector.count(); i++)
   {
      if (_langVector.at(i).current)
      {
         iRV = _langVector.at(i).id;
         break;
      }
   }
   _mtxLv.unlock();

   return iRV;
}
