/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/sunduk.tv/qtimelabel.h $
 *
 *  @file     qtimelabel.h
 *
 *  @author   Jo2003
 *
 *  @date     27.02.2012
 *
 *  $Id: qtimelabel.h 1171 2013-08-11 10:02:12Z Olenka.Joerg $
 *
 *///------------------------- (c) 2012 by Jo2003  --------------------------
#ifndef __20120227_QTIMELABEL_H
   #define __20120227_QTIMELABEL_H

#include <QLabel>

//---------------------------------------------------------------------------
//! \class   QTimeLabel
//! \date    27.02.2012
//! \author  Jo2003
//! \brief   a custom label to display time values
//---------------------------------------------------------------------------
class QTimeLabel : public QLabel
{
   Q_OBJECT

public:
   /////////////////////////////////////////////////////////////////////
   // constructors / destructor without special functionality ...
   QTimeLabel (QWidget * parent = 0, Qt::WindowFlags f = 0)
      : QLabel (parent, f), iPerc(0), bWithBuffer(true)
   {

   }

   QTimeLabel (const QString & text, QWidget * parent = 0, Qt::WindowFlags f = 0)
      : QLabel (text, parent, f), iPerc(0), bWithBuffer(true)
   {

   }

   virtual ~QTimeLabel()
   {

   }
   /////////////////////////////////////////////////////////////////////

   //---------------------------------------------------------------------------
   //
   //! \brief   set formatted time to label
   //
   //! \author  Jo2003
   //! \date    27.02.2012
   //
   //! \param   iSec time in seconds
   //
   //! \return  --
   //---------------------------------------------------------------------------
   void setTime (qint64 iSec)
   {
      if ((iPerc >= 100) || !bWithBuffer)
      {
         int     h, m, s;
         QString lab = (iSec < 0) ? "-" : " ";

         iSec = abs(iSec);

         h =  iSec / 3600;
         m = (iSec % 3600) / 60;
         s =  iSec % 60;

         lab += QString("%1:%2:%3").arg(h, 1, 10, QChar('0'))
                                   .arg(m, 2, 10, QChar('0'))
                                   .arg(s, 2, 10, QChar('0'));

         setText(lab);
      }
   }

   //---------------------------------------------------------------------------
   //
   //! \brief   set formatted time to label
   //
   //! \author  Jo2003
   //! \date    27.02.2012
   //
   //! \param   iSec time in seconds
   //
   //! \return  --
   //---------------------------------------------------------------------------
   void setBuffPercent (int iPercent)
   {
      if (iPercent < 100)
      {
         setText(tr("Buf: %1%").arg(iPercent));
      }

      iPerc = iPercent;
   }

   //---------------------------------------------------------------------------
   //
   //! \brief   enable / disable buffer usage
   //
   //! \author  Jo2003
   //! \date    11.08.2013
   //
   //! \param   flag (bool) enable (true) or disable (false) buffer feature
   //
   //! \return  --
   //---------------------------------------------------------------------------
   void useBuffer(bool flag)
   {
      bWithBuffer = flag;
   }


private:
   int  iPerc;
   bool bWithBuffer;
};

#endif // __20120227_QTIMELABEL_H
