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
#include "cvideoframe.h"

// log file functions ...
extern CLogFile VlcLog;

/* -----------------------------------------------------------------\
|  Method: CVideoFrame / constructor
|  Begin: 20.06.2010 / 19:05:00
|  Author: Jo2003
|  Description: create object, init values
|
|  Parameters: pointer to parent window, windows flags
|
|  Returns: --
\----------------------------------------------------------------- */
CVideoFrame::CVideoFrame(QWidget * parent, Qt::WindowFlags f)
   : QFrame(parent, f)
{
   pvShortcuts = NULL;

   // set mouse hide timer to single shot timer ...
   tMouseHide.setSingleShot (true);

   // ... with 1000 ms. interval ...
   tMouseHide.setInterval (1000);

   // hide mouse when timer has timeout ...
   connect (&tMouseHide, SIGNAL(timeout()), this, SLOT(slotHideMouse()));
}

/* -----------------------------------------------------------------\
|  Method: ~CVideoFrame / destructor
|  Begin: 22.06.2010 / 13:05:00
|  Author: Jo2003
|  Description: clean at destruction
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
CVideoFrame::~CVideoFrame()
{

}

/* -----------------------------------------------------------------\
|  Method: mouseDoubleClickEvent
|  Begin: 22.06.2010 / 19:05:00
|  Author: Jo2003
|  Description: toggle fullscreen on double click
|
|  Parameters: pointer to event ...
|
|  Returns: --
\----------------------------------------------------------------- */
void CVideoFrame::mouseDoubleClickEvent(QMouseEvent *pEvent)
{
   emit sigToggleFullscreen();
   pEvent->accept();
}

/* -----------------------------------------------------------------\
|  Method: mouseMoveEvent
|  Begin: 27.07.2010 / 11:45:00
|  Author: Jo2003
|  Description: mouse was moved, special handling to hide mouse
|               if we are in fullScreen ...
|  Parameters: pointer to event ...
|
|  Returns: --
\----------------------------------------------------------------- */
void CVideoFrame::mouseMoveEvent (QMouseEvent *pEvent)
{
   // is cursor hidden ... ?
   if (cursor().shape() == Qt::BlankCursor)
   {
      // remove blank cursor (means unhide cursor) ...
      unsetCursor ();
   }

   // if we're in fullscreen, start mouse hide timer ...
   if (parentWidget()->isFullScreen())
   {
      // trigger or re-trigger mouse hide timer ...
      tMouseHide.start ();
   }

   // always accept event ...
   pEvent->accept ();
}

/* -----------------------------------------------------------------\
|  Method: slotHideMouse
|  Begin: 27.07.2010 / 11:45:00
|  Author: Jo2003
|  Description: hide mouse cursore if we're in fullscreen
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CVideoFrame::slotHideMouse()
{
   // hide mouse if we're in fullscreen ...
   if (parentWidget()->isFullScreen())
   {
      // hide cursor if not hidden ...
      if (cursor().shape() != Qt::BlankCursor)
      {
         setCursor(QCursor(Qt::BlankCursor));
      }
   }
}

/* -----------------------------------------------------------------\
|  Method: keyPressEvent
|  Begin: 23.03.2010 / 22:46:10
|  Author: Jo2003
|  Description: catch keypress events to emulate shortcuts
|
|  Parameters: pointer to event
|
|  Returns: --
\----------------------------------------------------------------- */
void CVideoFrame::keyPressEvent(QKeyEvent *pEvent)
{
   QString sShortCut;
   int     iRV;

   // can we create a shortcut string for this key ... ?
   iRV = CShortcutEx::createShortcutString(pEvent->modifiers(),
                                           pEvent->text(), sShortCut);

   if (!iRV)
   {
      // check if shortcut string matches one of our shortcuts ...
      iRV = fakeShortCut(QKeySequence (sShortCut));

      if (!iRV)
      {
         pEvent->accept();
      }
   }

   // event not yet handled ... give it to base class ...
   if (iRV == -1)
   {
      QWidget::keyPressEvent(pEvent);
   }
}

/* -----------------------------------------------------------------\
|  Method: setShortCuts
|  Begin: 24.03.2010 / 14:17:51
|  Author: Jo2003
|  Description: store a pointer to shortcuts vector
|
|  Parameters: pointer to shortcuts vector
|
|  Returns: --
\----------------------------------------------------------------- */
void CVideoFrame::setShortCuts(QVector<CShortcutEx *> *pvSc)
{
   pvShortcuts = pvSc;
}

/* -----------------------------------------------------------------\
|  Method: fakeShortCut
|  Begin: 24.03.2010 / 14:30:51
|  Author: Jo2003
|  Description: fake shortcut press if needed
|
|  Parameters: key sequence
|
|  Returns: 0 --> shortcut sent
|          -1 --> not handled
\----------------------------------------------------------------- */
int CVideoFrame::fakeShortCut (const QKeySequence &seq)
{
   int iRV = -1;
   QVector<CShortcutEx *>::const_iterator cit;

   if (pvShortcuts)
   {
      // test all shortcuts if one matches the now incoming ...
      for (cit = pvShortcuts->constBegin(); (cit != pvShortcuts->constEnd()) && iRV; cit ++)
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

/************************* History ***************************\
| $Log$
\*************************************************************/
