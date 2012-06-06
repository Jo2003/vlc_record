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
};


#endif // __20120229_SMALL_HELPERS_H
