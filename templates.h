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

#define CMD_PLAY_LIVE    "LIVE_PLAY"
#define CMD_PLAY_ARCH    "ARCH_PLAY"
#define CMD_REC_LIVE     "LIVE_REC"
#define CMD_REC_ARCH     "ARCH_REC"
#define CMD_SIL_REC_LIVE "LIVE_SIL_REC"
#define CMD_SIL_REC_ARCH "ARCH_SIL_REC"
#define FLAG_TRANSLIT    "TRANSLIT"
#define FORCE_MUX        "FORCE_MUX"

/*
#define TMPL_PLAY_HTTP  "\"" TMPL_URL "\" --no-http-reconnect --http-caching=" TMPL_CACHE
#define TMPL_PLAY_RTSP  "\"" TMPL_URL "\" --rtsp-tcp --rtsp-caching=" TMPL_CACHE
#define TMPL_REC        " --sout=\"#duplicate{dst=display, dst=std{access=file,mux=" TMPL_MUX ",dst='" TMPL_DST "'}}\""
#define TMPL_SILENT_REC " --sout=\"#std{access=file,mux=" TMPL_MUX ",dst='" TMPL_DST "'}\""
*/

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
#define TMPL_CONT      "<!--{[%CONTENT%]}-->"

#define EPG_TMPL  \
"<table border='0' cellpadding='0' cellspacing='1' width='100%' style='color: black; background-color: #036; width: 100%;'>\n"\
TMPL_ROWS \
"</table>\n"

#define HTML_SITE \
"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"\
"<html><head><title>" TMPL_TITLE "</title></head>\n"\
"<body>" TMPL_CONT "</body></html>\n"


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

#define NAVBAR_STYLE_BOTTOM \
"QTabBar::tab {\n"\
"  background-color: qlineargradient(spread:pad, x1:1, y1:1, x2:1, y2:0, stop:0 #eee, stop:0.7 grey, stop:1 #dedede);\n"\
"  border: 1px solid gray;\n"\
"  border-bottom-left-radius: 5px;\n"\
"  border-bottom-right-radius: 5px;\n"\
"  min-width: 66px;\n"\
"  padding: 4px;\n"\
"  font: bold;\n"\
"}\n"\
"QTabBar::tab:selected {\n"\
"  background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 #eee, stop:0.7 grey, stop:1 #dedede);\n"\
"  border-top-color: #eee;\n"\
"  border-bottom-color: #fa0;\n"\
"}\n"\
"QTabBar::tab:hover {\n"\
"  border-top-color: #eee;\n"\
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
"<b>" TMPL_END   "</b> %4\n"

#endif /* __011910__TEMPLATES_H */
/************************* History ***************************\
| $Log$
\*************************************************************/

