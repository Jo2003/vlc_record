/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     more_kartina_translations.h
 *
 *  @author   Jo2003
 *
 *  @date     19.09.2014
 *
 *  $Id$
 *
 *///------------------------- (c) 2014 by Jo2003  --------------------------
#ifndef MORE_KARTINA_TRANSLATIONS_H
   #define MORE_KARTINA_TRANSLATIONS_H

#ifndef __OWN_BUY_REMINDER
   #define __OWN_BUY_REMINDER \
      QT_TRANSLATE_NOOP("ownBuyReminder", \
         "<b>Your subscription ends in %1 day(s).</b><br>\n" \
         "You can renew your subscription at following site: \n" \
         "<a href='http://kartina.tv/shop'>http://kartina.tv/shop</a>,<br>\n" \
         "or by phone:\n" \
         "<table>\n" \
         "<tr><td>International:</td><td align='right'>+49 69 84 84 540</td></tr>\n" \
         "<tr><td>Germany:</td><td align='right'>069 84 84 540</td></tr>\n" \
         "<tr><td>USA:</td><td align='right'>1 888 827 0606</td></tr>\n" \
         "<tr><td>Israel:</td><td align='right'>072 2454300</td></tr>\n" \
         "<tr><td>Russia:</td><td align='right'>+7 800 77 58 329</td></tr>\n" \
         "<tr><td>Or using Skype:</td><td align='right'>" \
         "<a href='callto:zakaz.kartina.tv'>zakaz.kartina.tv</a></td></tr>\n" \
         "</table>\n")
#endif // __OWN_BUY_REMINDER

#ifndef __INFO_WINDOW_CONTENT
   #define __INFO_WINDOW_CONTENT \
      QT_TRANSLATE_NOOP("infoWindowContent", \
         "<table width='%1'>\n"\
            "<tr><td colspan='2'>\n"\
               "<b>Kartina.TV</b> - television around the world.<br />\n"\
               "Order subscription and receiver, contact technical support "\
               "and get answers at <a href=\"http://www.kartina.tv\">www.kartina.tv</a> "\
               "or call our 24-hour hotline:</td></tr>\n"\
            "<tr><td>North America:</td><td>+1 888 827 0606</td></tr>\n"\
            "<tr><td>United Kingdom:</td><td>0800 048 83 93</td></tr>\n"\
            "<tr><td>Israel:</td><td>072-2454300</td></tr>\n"\
            "<tr><td>All countries:</td><td>+49 69 8484540</td></tr>\n"\
            "<tr>"\
               "<td>Skype:</td>"\
               "<td><nowrap><a href='callto:zakaz.kartina.tv'>zakaz.kartina.tv</a>, <a href='callto:support.kartina.tv'>support.kartina.tv</a></nowrap></td>"\
            "</tr>\n"\
         "</table>\n")
#endif // __INFO_WINDOW_CONTENT

#endif // MORE_KARTINA_TRANSLATIONS_H
