/*********************** Information *************************\
| $HeadURL: https://vlc-record.googlecode.com/svn/branches/sunduk.tv/templates.h $
|
| Author: Jo2003
|
| Begin: 19.01.2010 / 15:58:18
|
| Last edited by: $Author: Olenka.Joerg $
|
| $Id: templates.h 1290 2014-01-28 12:16:02Z Olenka.Joerg $
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
#define TMPL_CONT      "<!--{[%CONTENT%]}-->"
#define TMPL_LINK      "<!--{[%LINK%]}-->"
#define TMPL_TITLE     "<!--{[%TITLE%]}-->"
#define TMPL_CSS       "<!--{[%CSS%]}-->"
#define TMPL_NAME      "<!--{[%NAME%]}-->"
#define TMPL_WIDTH     "<!--{[%WIDTH%]}-->"
#define TMPL_HEIGHT    "<!--{[%HEIGHT%]}-->"
#define TMPL_ALIGN     "<!--{[%ALIGN%]}-->"
#define TMPL_VALIGN    "<!--{[%VALIGN%]}-->"
#define TMPL_ATTR      "<!--{[%ATTRIBUTE%]}-->"

#ifdef Q_OS_MAC
   // hack to force a ca. 1px cell border ...
   #define DEF_CELLSPACING  "1.99"
#else
   #define DEF_CELLSPACING  "1"
#endif

#ifndef _HAS_OWN_STYLES
   // css styles for html writer ...
   #define TMPL_TAB_STYLE  "color: black; background-color: #036; width: 100%;"
   #define TMPL_A_STYLE    "color: black; background-color: rgb(255, 254, 212); padding: 3px;"
   #define TMPL_B_STYLE    "color: black; background-color: rgb(234, 221, 166); padding: 3px;"
   #define TMPL_INNER_TAB  "color: black; background-color: rgb(242, 230, 187); padding: 5px;"
   #define TMPL_TH_STYLE   "color: white; background-color: #820; padding: 3px; font: bold; text-align: center; vertical-align: middle;"
   #define TMPL_CUR_STYLE  "color: black; background-color: #fc0; padding: 3px;"
   #define TMPL_VOD_STYLE  "color: #800; background-color: rgb(255, 254, 212); font-weight: bold; padding: 10px;"
   #define TMPL_IMG_RFLOAT "float: right; margin: 15px; padding: 2px; border: 1px solid #666;"
   #define TMPL_ONE_CELL   "background-color: rgb(255, 254, 212); color: black; padding: 3px;"
   #define TMPL_VOD_BODY   "body {background-color: rgb(255, 254, 212);}"
#endif // _HAS_OWN_STYLES

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


#endif /* __011910__TEMPLATES_H */
/************************* History ***************************\
| $Log$
\*************************************************************/

