/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 19.01.2010 / 15:58:18
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#ifndef __011910__TEMPLATES_H
   #define __011910__TEMPLATES_H

/**********************************************************/
/*                      PATH templates                    */
/**********************************************************/
#define TMPL_PLAYER      "{[%PLAYER%]}"
#define TMPL_URL         "{[%URL%]}"
#define TMPL_MUX         "{[%MUX%]}"
#define TMPL_DST         "{[%DST%]}"
#define TMPL_CACHE       "{[%CACHE%]}"
#define TMPL_TEMP        "{[%TMP%]}"

#define CMD_PLAY_LIVE    "LIVE_PLAY"
#define CMD_PLAY_ARCH    "ARCH_PLAY"
#define CMD_REC_LIVE     "LIVE_REC"
#define CMD_REC_ARCH     "ARCH_REC"
#define CMD_SIL_REC_LIVE "LIVE_SIL_REC"
#define CMD_SIL_REC_ARCH "ARCH_SIL_REC"
#define FLAG_TRANSLIT    "TRANSLIT"
#define FORCE_MUX        "FORCE_MUX"
#define DOWN_FIRST       "DOWN_FIRST"
#define ADD_LVLC_OPTS    "LVLC_OPTS"

/**********************************************************/
/*                      HTML templates                    */
/**********************************************************/
#define TMPL_TIME      "<!--{[%TIME%]}-->"
#define TMPL_PROG      "<!--{[%PROG%]}-->"
#define TMPL_ROWS      "<!--{[%ROWS%]}-->"
#define TMPL_HEAD      "<!--{[%HEAD%]}-->"
#define TMPL_START     "<!--{[%START%]}-->"
#define TMPL_END       "<!--{[%END%]}-->"
#define TMPL_TITLE     "<!--{[%TITLE%]}-->"
#define TMPL_TITLE_L   "<!--{[%TITLE_L%]}-->"
#define TMPL_TITLE_R   "<!--{[%TITLE_R%]}-->"
#define TMPL_CONT      "<!--{[%CONTENT%]}-->"
#define TMPL_VOD_L     "<!--{[%LEFTCOL%]}-->"
#define TMPL_VOD_R     "<!--{[%RIGHTCOL%]}-->"
#define TMPL_LINK      "<!--{[%LINK%]}-->"
#define TMPL_IMG       "<!--{[%IMG%]}-->"
#define TMPL_TITLE     "<!--{[%TITLE%]}-->"
#define TMPL_ACTORS    "<!--{[%ACTORS%]}-->"
#define TMPL_DIREC     "<!--{[%DIREC%]}-->"
#define TMPL_CSS       "<!--{[%CSS%]}-->"
#define TMPL_GENRE     "<!--{[%GENRE%]}-->"
#define TMPL_NAME      "<!--{[%NAME%]}-->"
#define TMPL_DESCR     "<!--{[%DESCR%]}-->"
#define TMPL_FAVO      "<!--{[%FAVO%]}-->"
#define TMPL_ERR       "<!--{[%ERROR%]}-->"

#define EPG_TMPL  \
"<table border='0' cellpadding='0' cellspacing='1' width='100%' style='color: black; background-color: #036; width: 100%;'>\n"\
TMPL_ROWS \
"</table>\n"

#define TMPL_CODEC \
"<span style='color: #888'>" TMPL_TITLE "</span>"

#define TMPL_VIDEO_TITLE \
"<div style='font-weight: bold; color: #800; padding: 10px;'>" TMPL_TITLE "</div>"

#define TEMPL_VOD_FAV \
"<a href='" TMPL_LINK "'><img src='" TMPL_IMG "' width='20' height='20' title='" TMPL_TITLE "' /></a>\n"

#define TMPL_VIDEO_DETAILS \
"<img class='floatright' src='" TMPL_IMG "' title='" TMPL_TITLE "' />\n" \
"<h3>" TMPL_TITLE  "</h3>&nbsp;&nbsp;<span>" TMPL_FAVO "</span>\n" \
"<p style='color: #008800'>"  TMPL_GENRE  "</p>\n" \
"<p style='color: #888888'>"  TMPL_TIME   "</p>\n" \
"<p style='color: #880000'>"  TMPL_DIREC  "</p>\n" \
"<p style='color: #000088'>"  TMPL_ACTORS "</p>\n" \
"<p>"  TMPL_PROG   "</p>\n" \
TMPL_LINK "\n"

#define TMPL_VIDEO_LINKS \
"<p>"  TMPL_LINK   "</p>\n" \
"<div align='center'>[ <a href='videothek?action=backtolist'>" TMPL_END "</a> ]</div>\n"

#define HTML_SITE \
"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"\
"<html>\n" \
"<head>\n" \
"<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\">\n" \
"<title>" TMPL_TITLE "</title>\n" \
TMPL_CSS \
"</head>\n"\
"<body>" TMPL_CONT "</body></html>\n"

#define TR_VOD_LIST \
"  <tr>\n"\
"    <td style='color: black; background-color: rgb(255, 254, 212); padding: 10px;'><div align='center'>" TMPL_VOD_L "<br>" TMPL_TITLE_L "</div></td>\n"\
"    <td style='color: black; background-color: rgb(255, 254, 212); padding: 10px;'><div align='center'>" TMPL_VOD_R "<br>" TMPL_TITLE_R "</div></td>\n"\
"  </tr>\n"

#define TMPL_IMG_LINK \
"<a href='" TMPL_LINK "'><img style='border: 0px;' src='" TMPL_IMG "' title='" TMPL_TITLE "' /></a>"

#define TMPL_SIMPLE_LINK \
"<a href='" TMPL_LINK "'>" TMPL_TITLE "</a>\n"

#define TMPL_HTML_ICON \
"<img style='border: 0px;' src='" TMPL_IMG "' height='20' width='20' title='" TMPL_TITLE "' />"


#define TR_TMPL_A \
"  <tr>\n"\
"    <td style='color: black; background-color: rgb(255, 254, 212); padding: 3px;'>" TMPL_TIME "</td>\n"\
"    <td style='color: black; background-color: rgb(255, 254, 212); padding: 3px;'>" TMPL_PROG "</td>\n"\
"  </tr>\n"

#define TR_TMPL_B \
"  <tr>\n"\
"    <td style='color: black; background-color: rgb(234, 221, 166); padding: 3px;'>" TMPL_TIME "</td>\n"\
"    <td style='color: black; background-color: rgb(234, 221, 166); padding: 3px;'>" TMPL_PROG "</td>\n"\
"  </tr>\n"

#define TR_TMPL_ACTUAL \
"  <tr>\n"\
"    <td style='color: black; background-color: #fc0; padding: 3px;'>" TMPL_TIME "</td>\n"\
"    <td style='color: black; background-color: #fc0; padding: 3px;'>" TMPL_PROG "</td>\n"\
"  </tr>\n"

#define TR_HEAD \
"  <tr>\n"\
"    <th colspan='2' style='color: white; background-color: #820; padding: 3px; font: bold; text-align: center; vertical-align: middle;'>" TMPL_HEAD "</th>\n"\
"  </tr>\n"

#define TMPL_CSS_WRAPPER \
"<style type='text/css'>\n" \
TMPL_CSS \
"</style>\n"

#define TMPL_CSS_IMG_FLOAT \
".floatright{float: right; margin: 15px; padding: 2px; border: 1px solid #666;}\n" \
"body {background-color: rgb(255, 254, 212);}\n"

#define TMPL_BACKCOLOR \
"<table border='0' cellpadding='0' cellspacing='1' width='100%' style='color: black; background-color: #036; width: 100%;'>\n"\
"  <tr>\n"\
"    <td style='color: black; background-color: %1; padding: 3px;'>%2</td>\n"\
"  </tr>\n"\
"</table>\n"

#define EPG_HEAD \
"<div align='center' style='width: 100%; text-align: center;'>" TMPL_HEAD "</div>\n"

#define NAVBAR_STYLE \
"QTabBar::tab {\n"\
"  background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 #eee, stop:0.7 grey, stop:1 #dedede);\n"\
"  border: 1px solid gray;\n"\
"  border-radius: 0px;\n"\
"  border-top-left-radius: 5px;\n"\
"  border-top-right-radius: 5px;\n"\
"  min-width: 33px;\n"\
"  padding: 4px;\n"\
"  font: bold;\n"\
"}\n"\
"QTabBar::tab:selected {\n"\
"  background-color: qlineargradient(spread:pad, x1:1, y1:1, x2:1, y2:0, stop:0 #eee, stop:0.7 grey, stop:1 #dedede);\n"\
"  border-bottom-color: #eee;\n"\
"  border-top-color: #fa0;\n"\
"}\n"\
"QTabBar::tab:hover {\n"\
"  border-bottom-color: #eee;\n"\
"}\n"

#define FAVBTN_STYLE \
"QToolButton {\n"\
"  background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 #eee, stop:0.7 grey, stop:1 #dedede);\n"\
"  border: 1px solid gray;\n"\
"  border-radius: 5px;\n"\
"  min-width: 33px;\n"\
"  min-height: 33px;\n"\
"}\n"\
"QToolButton:pressed {\n"\
"  background-color: qlineargradient(spread:pad, x1:1, y1:1, x2:1, y2:0, stop:0 #eee, stop:0.7 grey, stop:1 #dedede);\n"\
"  border-bottom-color: white;\n"\
"}\n"

#define LABEL_STYLE \
"QLabel#%1 {\n"\
"  font-weight: bold;\n"\
"  color: %2;\n"\
"}\n"

#define PROG_INFO_TOOL_TIP \
"<b style='color: red;'>%1</b><br />\n"\
"<b>" TMPL_PROG  "</b> %2<br />\n"\
"<b>" TMPL_START "</b> %3<br />\n"\
"<b>" TMPL_END   "</b> %4<br />\n"\
"<b>" TMPL_TIME  "</b> %5<br />\n"

#define TMPL_DETAILED_ERROR \
"<p><b>%1</b></p>\n"\
" <br />\n"\
"<table border='0'>\n"\
   "<tr>\n"\
      "<td style='font-weight: bold;'>" TMPL_NAME  " </td>\n"\
      "<td> %2</td>\n"\
   "</tr>\n"\
   "<tr>\n"\
      "<td style='font-weight: bold;'>" TMPL_ERR   " </td>\n"\
      "<td> %3</td>\n"\
   "</tr>\n"\
   "<tr>\n"\
      "<td style='font-weight: bold;'>" TMPL_DESCR " </td>\n"\
      "<td> %4</td>\n"\
   "</tr>\n"\
"</table>\n"


#endif /* __011910__TEMPLATES_H */
/************************* History ***************************\
| $Log$
\*************************************************************/

