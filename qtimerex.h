/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/qtimerex.h $
 *
 *  @file     qtimeerex.h
 *
 *  @author   Jo2003
 *
 *  @date     18.12.2012
 *
 *  $Id: qtimerex.h 996 2012-12-18 18:31:27Z Olenka.Joerg@gmail.com $
 *
 *///------------------------- (c) 2012 by Jo2003  --------------------------
#ifndef __20121217_QTIMEREX_H
   #define __20121217_QTIMEREX_H
#include <QTimer>

//---------------------------------------------------------------------------
//! \class   QTimerEx
//! \date    18.12.2012
//! \author  Jo2003
//! \brief   a custom timer class to count seconds (supports pause)
//---------------------------------------------------------------------------
class QTimerEx : public QTimer
{
	Q_OBJECT
	
public:
   //---------------------------------------------------------------------------
   //
   //! \brief   construct object, init values
   //
   //! \author  Jo2003
   //! \date    18.12.2012
   //
   //! \param   parent optional pointer to parent object
   //
   //! \return  --
   //---------------------------------------------------------------------------
   explicit QTimerEx(QObject * parent = 0) : QTimer(parent), uiElapsed(0), uiOffset(0)
   {
      setInterval(1000);
      connect (this, SIGNAL(timeout()), this, SLOT(slotTick()));
   }
   
   //---------------------------------------------------------------------------
   //
   //! \brief   reset timer
   //
   //! \author  Jo2003
   //! \date    18.12.2012
   //
   //! \param   bClearOffset flag if offset value should be cleared too
   //
   //! \return  --
   //---------------------------------------------------------------------------
   void reset (bool bClearOffset = false)
   {
      QTimer::stop();
      uiElapsed = 0;

      if (bClearOffset)
      {
         uiOffset  = 0;
      }
   }
   
   //---------------------------------------------------------------------------
   //
   //! \brief   set offset in seconds, used to compute real gmt position
   //
   //! \author  Jo2003
   //! \date    18.12.2012
   //
   //! \param   uiOffs offset in seconds
   //
   //! \return  --
   //---------------------------------------------------------------------------
   void setOffset (uint uiOffs)
   {
      uiOffset = uiOffs;
   }
   
   //---------------------------------------------------------------------------
   //
   //! \brief   get position + offset
   //
   //! \author  Jo2003
   //! \date    18.12.2012
   //
   //! \param   --
   //
   //! \return  current position
   //---------------------------------------------------------------------------
   uint pos ()
   {
      return uiElapsed + uiOffset;
   }
   
public slots:

   //---------------------------------------------------------------------------
   //
   //! \brief   on pause simply stop timer but don't reset values
   //
   //! \author  Jo2003
   //! \date    18.12.2012
   //
   //! \param   --
   //
   //! \return  --
   //---------------------------------------------------------------------------
   void pause ()
   {
      QTimer::stop();
   }
   
private slots:

   //---------------------------------------------------------------------------
   //
   //! \brief   count seconds
   //
   //! \author  Jo2003
   //! \date    18.12.2012
   //
   //! \param   --
   //
   //! \return  --
   //---------------------------------------------------------------------------
   void slotTick()
   {
      uiElapsed ++;
   }
   
private:
   uint uiElapsed;
   uint uiOffset;
};

#endif // __20121217_QTIMEREX_H
