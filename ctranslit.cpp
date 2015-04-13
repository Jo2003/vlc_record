/*********************** Information *************************\
| $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/ctranslit.cpp $
|
| Author: Jo2003
|
| Begin: 08.02.2010 / 10:50:35
|
| Last edited by: $Author: Olenka.Joerg $
|
| $Id: ctranslit.cpp 82 2010-02-24 08:11:05Z Olenka.Joerg $
\*************************************************************/
#include "ctranslit.h"

/********************************************************************\
 *  This translit stuff only works, if you save this file in        *
 *  UTF-8 format!                                                   *
 *                                                                  *
 *    !!! Using another coding will break the functionality !!!     *
 *                                                                  *
 *  Make sure that you project uses UTF-8 encoding for text!        *
 *  To do this, check out the project settings!                     *
\********************************************************************/

/* -----------------------------------------------------------------\
|  Method: CTranslit / constructor
|  Begin: 08.02.2010 / 11:05:00
|  Author: Jo2003
|  Description: init translation tables
|
|  Parameters: pointer to parent object
|
|  Returns: --
\----------------------------------------------------------------- */
CTranslit::CTranslit()
{
   // fill map (cyrillic -> latin) ...
   mCyr2Lat[QString::fromUtf8("а")] = "a";
   mCyr2Lat[QString::fromUtf8("б")] = "b";
   mCyr2Lat[QString::fromUtf8("в")] = "v";
   mCyr2Lat[QString::fromUtf8("г")] = "g";
   mCyr2Lat[QString::fromUtf8("д")] = "d";
   mCyr2Lat[QString::fromUtf8("е")] = "e";
   mCyr2Lat[QString::fromUtf8("ё")] = "e";
   mCyr2Lat[QString::fromUtf8("ж")] = "zh";
   mCyr2Lat[QString::fromUtf8("з")] = "z";
   mCyr2Lat[QString::fromUtf8("и")] = "i";
   mCyr2Lat[QString::fromUtf8("й")] = "j";
   mCyr2Lat[QString::fromUtf8("к")] = "k";
   mCyr2Lat[QString::fromUtf8("л")] = "l";
   mCyr2Lat[QString::fromUtf8("м")] = "m";
   mCyr2Lat[QString::fromUtf8("н")] = "n";
   mCyr2Lat[QString::fromUtf8("о")] = "o";
   mCyr2Lat[QString::fromUtf8("п")] = "p";
   mCyr2Lat[QString::fromUtf8("р")] = "r";
   mCyr2Lat[QString::fromUtf8("с")] = "s";
   mCyr2Lat[QString::fromUtf8("т")] = "t";
   mCyr2Lat[QString::fromUtf8("у")] = "u";
   mCyr2Lat[QString::fromUtf8("ф")] = "f";
   mCyr2Lat[QString::fromUtf8("х")] = "h";
   mCyr2Lat[QString::fromUtf8("ц")] = "c";
   mCyr2Lat[QString::fromUtf8("ш")] = "sh";
   mCyr2Lat[QString::fromUtf8("щ")] = "sch";
   mCyr2Lat[QString::fromUtf8("ч")] = "ch";
   mCyr2Lat[QString::fromUtf8("ы")] = "y";
   mCyr2Lat[QString::fromUtf8("ъ")] = "´";
   mCyr2Lat[QString::fromUtf8("ь")] = "'";
   mCyr2Lat[QString::fromUtf8("э")] = "e'";
   mCyr2Lat[QString::fromUtf8("ю")] = "yu";
   mCyr2Lat[QString::fromUtf8("я")] = "ya";

   // fill map (latin -> cyrillic) ...
   mLat2Cyr["a"]    = QString::fromUtf8("а");
   mLat2Cyr["b"]    = QString::fromUtf8("б");
   mLat2Cyr["v"]    = QString::fromUtf8("в");
   mLat2Cyr["g"]    = QString::fromUtf8("г");
   mLat2Cyr["d"]    = QString::fromUtf8("д");
   mLat2Cyr["e"]    = QString::fromUtf8("е");
   mLat2Cyr["jo"]   = QString::fromUtf8("ё");
   mLat2Cyr["yo"]   = QString::fromUtf8("ё");
   mLat2Cyr["zh"]   = QString::fromUtf8("ж");
   mLat2Cyr["z"]    = QString::fromUtf8("з");
   mLat2Cyr["i"]    = QString::fromUtf8("и");
   mLat2Cyr["j"]    = QString::fromUtf8("й");
   mLat2Cyr["k"]    = QString::fromUtf8("к");
   mLat2Cyr["l"]    = QString::fromUtf8("л");
   mLat2Cyr["m"]    = QString::fromUtf8("м");
   mLat2Cyr["n"]    = QString::fromUtf8("н");
   mLat2Cyr["o"]    = QString::fromUtf8("о");
   mLat2Cyr["p"]    = QString::fromUtf8("п");
   mLat2Cyr["r"]    = QString::fromUtf8("р");
   mLat2Cyr["s"]    = QString::fromUtf8("с");
   mLat2Cyr["t"]    = QString::fromUtf8("т");
   mLat2Cyr["u"]    = QString::fromUtf8("у");
   mLat2Cyr["f"]    = QString::fromUtf8("ф");
   mLat2Cyr["h"]    = QString::fromUtf8("х");
   mLat2Cyr["x"]    = QString::fromUtf8("х");
   mLat2Cyr["c"]    = QString::fromUtf8("ц");
   mLat2Cyr["sh"]   = QString::fromUtf8("ш");
   mLat2Cyr["sch"]  = QString::fromUtf8("щ");
   mLat2Cyr["w"]    = QString::fromUtf8("щ");
   mLat2Cyr["shch"] = QString::fromUtf8("щ");
   mLat2Cyr["ch"]   = QString::fromUtf8("ч");
   mLat2Cyr["y"]    = QString::fromUtf8("ы");
   mLat2Cyr["´"]    = QString::fromUtf8("ъ");
   mLat2Cyr["'"]    = QString::fromUtf8("ь");
   mLat2Cyr["e'"]   = QString::fromUtf8("э");
   mLat2Cyr["je"]   = QString::fromUtf8("э");
   mLat2Cyr["ju"]   = QString::fromUtf8("ю");
   mLat2Cyr["yu"]   = QString::fromUtf8("ю");
   mLat2Cyr["ja"]   = QString::fromUtf8("я");
   mLat2Cyr["ya"]   = QString::fromUtf8("я");
}

/* -----------------------------------------------------------------\
|  Method: ~CTranslit / destructor
|  Begin: 08.02.2010 / 11:05:00
|  Author: Jo2003
|  Description: clean at destruction
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
CTranslit::~CTranslit()
{
   // nothing to do so far ...
}

/* -----------------------------------------------------------------\
|  Method: CyrToLat
|  Begin: 08.02.2010 / 11:05:00
|  Author: Jo2003
|  Description: make translit cyrillic --> latin
|
|  Parameters: ref. to cyrillic string
|
|  Returns: latin string
\----------------------------------------------------------------- */
QString CTranslit::CyrToLat(const QString &str, bool fileName)
{
   QString     sIn, sOut, sTok;
   QTextStream ts(&sOut);

   QMap<QString, QString>::const_iterator cit;

   sIn = str.toLower();

   for (int i = 0; i < str.length(); i++)
   {
      cit = mCyr2Lat.constFind(sIn.mid(i, 1));

      if (cit != mCyr2Lat.constEnd())
      {
         sTok = (*cit);

         if (str[i].isUpper())
         {
            sTok[0] = sTok[0].toUpper();
         }

         ts << sTok;
      }
      else
      {
         ts << str[i];
      }
   }

   if (fileName)
   {
      sOut.replace(" ", "_");
      sOut.replace("'", "");
      sOut.replace("´", "");
      sOut = sOut.toUpper();
   }

   return sOut;
}

/* -----------------------------------------------------------------\
|  Method: LatToCyr
|  Begin: 08.02.2010 / 11:05:00
|  Author: Jo2003
|  Description: make translit latin --> cyrillic
|
|  Parameters: ref. to latin string
|
|  Returns: cyrillic string
\----------------------------------------------------------------- */
QString CTranslit::LatToCyr(const QString &str, bool fileName)
{
   QString     sIn, sOut, sTok;
   QTextStream ts(&sOut);
   int         iNoChars;

   sIn = str.toLower();

   QMap<QString, QString>::const_iterator cit;

   for (int i = 0; i < str.length(); )
   {
      iNoChars = MAX_LAT2CYR_LEN;

      while (iNoChars > 0)
      {
         sTok = sIn.mid(i, iNoChars);
         cit  = mLat2Cyr.constFind(sTok);

         // did we found the phrase ... ?
         if (cit != mLat2Cyr.constEnd())
         {
            // yes !
            if (str[i].isUpper())
            {
               ts << (*cit).toUpper();
            }

            i += iNoChars;
            break;

         }
         else
         {
            // no, try with shortened phrase ...
            iNoChars --;
         }

         // not found ... take it as it is ...
         if (iNoChars == 0)
         {
            ts << str[i];
            i++;
         }
      }
   }

   if (fileName)
   {
      sOut.replace(" ", "_");
      sOut.replace("'", "");
      sOut.replace("´", "");
      sOut = sOut.toUpper();
   }

   return sOut;
}

/************************* History ***************************\
| $Log$
\*************************************************************/
