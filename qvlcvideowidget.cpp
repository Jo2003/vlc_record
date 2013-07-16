/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qvlcvideowidget.cpp
 *
 *  @author   Jo2003, inspired by Tadej Novak (VLC-Qt)
 *
 *  @date     08.02.2012
 *
 *  $Id$
 *
 *///------------------------- (c) 2012 by Jo2003  --------------------------
#include "qvlcvideowidget.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QRegExp>

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
   _shortcuts(0),
   _extFullScreen(false),
   _ctrlPanel(NULL),
   _mouseOnPanel(false),
   _panelPositioned(false),
   _contextMenu(NULL)
{
   setMouseTracking(true);

   QVBoxLayout *pLayout = new QVBoxLayout();
   _render              = new QWidget(this);
   _mouseHide           = new QTimer(this);
   _render->setMouseTracking(true);
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

   // create player control panel ...
   _ctrlPanel = new QOverlayedControl(_render, Qt::Tool | Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);

   // hide panel initially ...
   _ctrlPanel->hide();

   // use own context menu ...
   setContextMenuPolicy (Qt::CustomContextMenu);

   // fill context menu ...
   _contextMenu = new QMenu(this);
   touchContextMenu();

   connect (_mouseHide, SIGNAL(timeout()), this, SLOT(hideMouse()));
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
   QWidget::changeEvent(event);

   switch (event->type())
   {
   case QEvent::LanguageChange:
      // retranslate context menu ...
      touchContextMenu();
      break;
   default:
      break;
   }
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
void QVlcVideoWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
   event->ignore();

   emit fullScreen();
}

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
   event->ignore();

   if(isFullScreen() || _extFullScreen)
   {
      emit mouseShow(event->globalPos());
      QApplication::restoreOverrideCursor();
      _ctrlPanel->show();
      _ctrlPanel->raise();
      _mouseHide->start(1500);
   }

   if (!_mouseOnPanel)
   {
      activateWindow();
      setFocus(Qt::OtherFocusReason);
   }
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
   event->ignore();

   if(event->button() == Qt::RightButton)
   {
      QApplication::restoreOverrideCursor();
      emit rightClick(event->globalPos());
   }
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
   event->ignore();

   emit wheel ((event->delta() > 0) ? true : false);
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

   event->ignore();

   // make key sequence from key event ...
   if (!keyEventToKeySequence(event, seq))
   {
      fakeShortCut(seq);
   }
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
//! \brief   fullscreen toggled, do cursor hide stuff
//
//! \author  Jo2003
//! \date    30.04.2012
//
//! \return  --
//---------------------------------------------------------------------------
void QVlcVideoWidget::fullScreenToggled(int on)
{
   if (on)
   {
      _extFullScreen = true;

      if (!_panelPositioned)
      {
         // position player control panel correctly ...
         QDesktopWidget *pDesc = QApplication::desktop();
         QRect            rect = pDesc->screenGeometry(_render);

         int x = rect.width() / 2 - _ctrlPanel->width() / 2;
         int y = rect.height() - (int)((float)_ctrlPanel->height() * 1.33);

         _ctrlPanel->setGeometry(x, y, _ctrlPanel->width(), _ctrlPanel->height());

         // mark as positioned ...
         _panelPositioned = true;
      }

      // start mouse hiding ...
      _mouseHide->start(1000);

      // make sure we have the focus ...
      activateWindow();
      setFocus(Qt::OtherFocusReason);
   }
   else
   {
      _extFullScreen = false;

      _ctrlPanel->hide();

      // restore visible cursor ...
      QApplication::restoreOverrideCursor();
   }
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

   // in case of retranslation or update we should take care of
   // interlaced setting ...
   if (!contActions.isEmpty())
   {
      // action 0 will always be the interlaced stuff since this
      // is always the first element we add ...
      bIntl = contActions.at(0)->isChecked();
   }

   // remove all menue entries ...
   _contextMenu->clear();

   // interlace stuff ...
   pAct = _contextMenu->addAction(tr("Deinterlace Video"));

   // prepare data ...
   contAct.actType = vlcvid::ACT_Deinterlace;
   contAct.actName = "n.a.";
   contAct.actVal.setValue(-1);

   // set data ...
   pAct->setData(QVariant::fromValue(contAct));
   pAct->setCheckable(true);
   pAct->setChecked(bIntl);

   // add seperator ...
   pAct = _contextMenu->addSeparator();

   // --------------------------------------------------------
   // language stuff ...
   // --------------------------------------------------------
   _mtxLv.lock();

   // go through language vector and add context menu entries ...
   for (i = 0; i < _langVector.count(); i++)
   {
      // try to grab language from track description ...
      if (rx.indexIn(_langVector.at(i).desc) > -1)
      {
         // create context menu entry ...
         pAct = _contextMenu->addAction(QIcon(_langVector.at(i).current ? ":player/atrack" : ""), rx.cap(1));

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
      case vlcvid::ACT_Deinterlace:
         emit sigDeinterlace(pAct->isChecked());
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
