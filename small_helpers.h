/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     small_helpers.h
 *
 *  @author   Jo2003
 *
 *  @date     29.02.2012
 *
 *  $Id$
 *
 *///------------------------- (c) 2012 by Jo2003  --------------------------
#ifndef __20120229_SMALL_HELPERS_H
   #define __20120229_SMALL_HELPERS_H

#include <QString>
#include <QFont>
#include <QFontMetrics>
#include <QCryptographicHash>
#include <QDateTime>
#include "defdef.h"
#include "qdatetimesyncro.h"

#define __VLCRECORD_KEY "O20J03N05+v205+n1907#80730108"

//---------------------------------------------------------------------------
/// a place for small helpers ...
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//! \class   CSmallHelpers
//! \date    04.06.2012
//! \author  Jo2003
//! \brief   a class which has small helpers as static functions ...
//---------------------------------------------------------------------------
class CSmallHelpers
{
public:
   // nothing todo on contruction / destruction ...
   CSmallHelpers()
   {

   }

   ~CSmallHelpers()
   {

   }

   //---------------------------------------------------------------------------
   //
   //! \brief   check if value is in between a min and max range
   //
   //! \author  Jo2003
   //! \date    29.02.2012
   //
   //! \param   min minimal value
   //! \param   max maximum value
   //! \param   val value to check
   //
   //! \return  true --> in range
   //! \return  false --> out of range
   //---------------------------------------------------------------------------
   template <typename T>
   static bool inBetween(const T &min, const T &max, const T &val)
   {
      return ((val >= min) && (val <= max)) ? true : false;
   }

   //---------------------------------------------------------------------------
   //
   //! \brief   get larger value back
   //
   //! \author  Jo2003
   //! \date    29.02.2012
   //
   //! \param   a first value
   //! \param   b second value
   //
   //! \return  larger value
   //---------------------------------------------------------------------------
   template <typename T>
   static const T& getMax(const T& a, const T& b)
   {
      return (a < b) ? b : a;
   }

   //---------------------------------------------------------------------------
   //
   //! \brief   get smaller value back
   //
   //! \author  Jo2003
   //! \date    29.02.2012
   //
   //! \param   a first value
   //! \param   b second value
   //
   //! \return  smaller value
   //---------------------------------------------------------------------------
   template <typename T>
   static const T& getMin(const T& a, const T& b)
   {
      return (a < b) ? a : b;
   }

   //---------------------------------------------------------------------------
   //
   //! \brief   encrypt a string using simple stream cipher method
   //
   //! \author  Jo2003
   //! \date    04.06.2012
   //
   //! \param   pass string to encrypt
   //
   //! \return  encrypted string
   //---------------------------------------------------------------------------
   static QString streamCipher (const QString& pass)
   {
      QString sKey = __VLCRECORD_KEY;
      QString strEncrypted;
      int     i, j = pass.length() % sKey.length();
      char    a, b, c;

      for (i = 0; i < pass.length(); i++)
      {
         a = pass.at(i).toAscii();
         b = sKey.at(j--).toAscii();
         c = a ^ b;
         strEncrypted += QString("%1").arg((uint)c, 2, 16, QChar('0'));

         if (j <= 0)
         {
            j = sKey.length() - 1;
         }
      }

      return strEncrypted;
   }

   //---------------------------------------------------------------------------
   //
   //! \brief   decrypt an encrypted string using simple stream cipher method
   //
   //! \author  Jo2003
   //! \date    04.06.2012
   //
   //! \param   pass string to decrypt
   //
   //! \return  decrypted string
   //---------------------------------------------------------------------------
   static QString streamDecipher (const QString& pass)
   {
      QString sKey = __VLCRECORD_KEY;
      QString strDecrypted;
      int     i, j = (pass.length() / 2) % sKey.length();
      char    a, b, c;

      for (i = 0; i < pass.length() / 2; i++)
      {
         a = (char)pass.mid(i * 2, 2).toInt(0, 16);
         b = sKey.at(j--).toAscii();
         c = a ^ b;
         strDecrypted[i] = c;

         if (j <= 0)
         {
            j = sKey.length() - 1;
         }
      }

      return strDecrypted;
   }

   //---------------------------------------------------------------------------
   //
   //! \brief   cut string so it fits a length
   //
   //! \author  Jo2003
   //! \date    23.03.2011
   //
   //! \param   str string to cut
   //! \param   fm font metrics to be used in widget
   //! \param   width width string should fit in
   //
   //! \return  --
   //---------------------------------------------------------------------------
   static void cutProgString (QString &str, const QFontMetrics &fm, int width)
   {
      if (width > 0)
      {
         str = str.section('\n', 0, 0);
         int iLength = str.length();

         // check that text width matches ...
         while (fm.size(Qt::TextSingleLine, str).width() > width)
         {
            // to wide --> shrink ...
            str = str.left(str.length() - 1);
         }

         if (str.length() != iLength)
         {
            str = str.left(str.length() - 3) + QString("...");
         }
      }
   }

   //---------------------------------------------------------------------------
   //
   //! \brief   create a md5 hash from a string (ascii only)
   //
   //! \author  Jo2003
   //! \date    19.03.2013
   //
   //! \param   str (const QString&) ref. to string to hash
   //
   //! \return  hash code as string
   //---------------------------------------------------------------------------
   static QString md5(const QString& str)
   {
      return QString(QCryptographicHash::hash(str.toAscii(), QCryptographicHash::Md5).toHex());
   }

   //---------------------------------------------------------------------------
   //
   //! \brief   create random number between low and high
   //
   //! \author  Jo2003
   //! \date    19.03.2013
   //
   //! \param   low (int) low value
   //! \param   high (int) high value
   //
   //! \return  random int
   //---------------------------------------------------------------------------
   static int randInt(int low, int high)
   {

      QDateTime now = QDateTime::currentDateTime();

      // random seed ...
      qsrand(now.toTime_t() + now.time().msec());

      // Random number between low and high
      return qrand() % ((high + 1) - low) + low;
   }

   //---------------------------------------------------------------------------
   //
   //! \brief   is archive available for this time
   //
   //! \author  Jo2003
   //! \date    07.08.2013
   //
   //! \param   uiStart (uint) unix time stamp
   //! \param   tmSync (const QDateTimeSyncro&) synced timer
   //
   //! \return  1 -> available; -1 -> not yet av.; -2 -> no more av.
   //---------------------------------------------------------------------------
   static int archiveAvailable (uint uiStart, const QDateTimeSyncro& tmSync)
   {
      int  iRet = 0;
      uint now        = tmSync.syncronizedTime_t();
      uint uiArchLow  = now - MAX_ARCHIV_AGE;   // no older than 2 weeks
      uint uiArchHigh = now - ARCHIV_OFFSET;    // 15 minutes in the past


      // archiv should be available 10 minutes after show start
      // in a time frame of 2 weeks ...
      if (inBetween(uiArchLow, uiArchHigh, uiStart))
      {
         iRet = 1;
      }
      else if (uiStart < uiArchLow)
      {
         // to old ...
         iRet = -2;
      }
      else
      {
         // not yet available ...
         iRet = -1;
      }

      return iRet;
   }
};


#endif // __20120229_SMALL_HELPERS_H
