/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/sunduk.tv/qfadewidget.h $
 *
 *  @file     qfadewidget.h
 *
 *  @author   Jo2003
 *
 *  @date     26.11.2013
 *
 *  $Id: qfadewidget.h 1275 2013-12-19 12:33:03Z Olenka.Joerg $
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#ifndef __20131125_QFADEWIDGET_H
   #define __20131125_QFADEWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QShowEvent>

//---------------------------------------------------------------------------
//! \class   QFadeWidget
//! \date    26.11.2013
//! \author  Jo2003
//! \brief   a transparent widget which can fade in and out
//---------------------------------------------------------------------------
class QFadeWidget : public QWidget
{
   Q_OBJECT

public:
   //---------------------------------------------------------------------------
   //
   //! \brief   constructs a transparent widget which can fade out ...
   //
   //! \author  Jo2003
   //! \date    26.11.2013
   //
   //! \param   --
   //
   //! \return  --
   //---------------------------------------------------------------------------
   QFadeWidget(QWidget* parent = 0, Qt::WindowFlags f = 0)
      : QWidget(parent, f)
   {
      _fOpaqueMaster = 0.75;
      _fOpaque       = 0.0;

      // transparent background ...
      setAttribute(Qt::WA_TranslucentBackground);

      connect (&_tFadeOut, SIGNAL(timeout()), this, SLOT(slotFadeOutMore()));
      connect (&_tFadeIn,  SIGNAL(timeout()), this, SLOT(slotFadeInMore()));
   }

   //---------------------------------------------------------------------------
   //
   //! \brief   destructor
   //
   //! \author  Jo2003
   //! \date    26.11.2013
   //
   //! \param   --
   //
   //! \return  --
   //---------------------------------------------------------------------------
   virtual ~QFadeWidget()
   {
   }

   //---------------------------------------------------------------------------
   //
   //! \brief   set default opaque value
   //
   //! \author  Jo2003
   //! \date    26.11.2013
   //
   //! \param   fOpaque (qreal) new default opaque value
   //
   //! \return  --
   //---------------------------------------------------------------------------
   void setOpaque (qreal fOpaque)
   {
      _fOpaqueMaster = fOpaque;
   }

   //---------------------------------------------------------------------------
   //
   //! \brief   start fade out
   //
   //! \author  Jo2003
   //! \date    27.11.2013
   //
   //! \param   --
   //
   //! \return  --
   //---------------------------------------------------------------------------
   virtual void fadeOut()
   {
      // fade a step every 10 msec ...
      _fOpaque = _fOpaqueMaster;
      _tFadeIn.stop();
      _tFadeOut.start(10);
   }

   //---------------------------------------------------------------------------
   //
   //! \brief   start fade in
   //
   //! \author  Jo2003
   //! \date    26.11.2013
   //
   //! \param   --
   //
   //! \return  --
   //---------------------------------------------------------------------------
   virtual void fadeIn()
   {
      _tFadeOut.stop();

      // fade in only:
      // - if we not already started fade in
      // - if we haven't reached wanted opaque value
      if (!_tFadeIn.isActive() && (_fOpaque != _fOpaqueMaster))
      {
         // fade a step every 10 msec ...
         _fOpaque = 0;
         _tFadeIn.start(10);

         // fade without to show widget ... ? Makes no sense!
         if (!isVisible())
         {
            show();
         }
      }
   }

protected:
   //---------------------------------------------------------------------------
   //
   //! \brief   widget is about to be shown
   //
   //! \author  Jo2003
   //! \date    27.11.2013
   //
   //! \param   e pointer to QShowEvent
   //
   //! \return  --
   //---------------------------------------------------------------------------
   virtual void showEvent(QShowEvent *e)
   {
      // stop fader timer ...
      _tFadeOut.stop();

      // set transparancy to panel only if no fade in in progress ...
      if (!_tFadeIn.isActive())
      {
         setWindowOpacity(_fOpaqueMaster);
         _fOpaque = _fOpaqueMaster;
      }

      // set pseudo caption ...
      QWidget::showEvent(e);
   }

   //---------------------------------------------------------------------------
   //
   //! \brief   widget is about to be hidden
   //
   //! \author  Jo2003
   //! \date    19.12.2013
   //
   //! \param   e pointer to QHideEvent
   //
   //! \return  --
   //---------------------------------------------------------------------------
   virtual void hideEvent(QHideEvent *e)
   {
      // stop fader timer ...
      _tFadeIn.stop();

      // set transparancy to panel only if no fade in in progress ...
      if (!_tFadeOut.isActive())
      {
         setWindowOpacity(0.0);
         _fOpaque = 0.0;
      }

      // set pseudo caption ...
      QWidget::hideEvent(e);
   }

private slots:
   //---------------------------------------------------------------------------
   //
   //! \brief   fade out more and finally hide widget
   //
   //! \author  Jo2003
   //! \date    27.11.2013
   //
   //! \param   --
   //
   //! \return  --
   //---------------------------------------------------------------------------
   void slotFadeOutMore()
   {
      _fOpaque -= 0.03;

      if (_fOpaque <= 0.0)
      {
         _tFadeOut.stop();
         _fOpaque = 0.0;
         hide();
      }
      else
      {
         setWindowOpacity(_fOpaque);
      }
   }

   //---------------------------------------------------------------------------
   //
   //! \brief   fade out more and finally hide widget
   //
   //! \author  Jo2003
   //! \date    27.11.2013
   //
   //! \param   --
   //
   //! \return  --
   //---------------------------------------------------------------------------
   void slotFadeInMore()
   {
      // fade in should be faster than fade out ...
      _fOpaque += 0.08;

      if (_fOpaque >= _fOpaqueMaster)
      {
         _tFadeIn.stop();
         _fOpaque = _fOpaqueMaster;
         setWindowOpacity(_fOpaqueMaster);
      }
      else
      {
         setWindowOpacity(_fOpaque);
      }
   }

private:
   qreal  _fOpaqueMaster;
   qreal  _fOpaque;
   QTimer _tFadeOut;
   QTimer _tFadeIn;
};

#endif // __20131125_QFADEWIDGET_H
