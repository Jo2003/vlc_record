/*********************** Information *************************\
| $HeadURL$
| 
| Author: Joerg Neubert
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
#define TMPL_VLC "{[%VLCPATH%]}"
#define TMPL_URL "{[%URL%]}"
#define TMPL_MUX "{[%MUX%]}"
#define TMPL_DST "{[%DST%]}"

#define VLC_REC_TEMPL  "\"" TMPL_VLC "\" \"" TMPL_URL "\" --sout \"#duplicate{dst=display, dst=std{access=file,mux=" TMPL_MUX ",dst='" TMPL_DST "'}}\""
#define VLC_PLAY_TEMPL "\"" TMPL_VLC "\" \"" TMPL_URL "\""


/**********************************************************/
/*                      HTML templates                    */
/**********************************************************/
#define TMPL_TIME "{[%TIME%]}"
#define TMPL_PROG "{[%PROG%]}"
#define TMPL_ROWS "{[%ROWS%]}"
#define TMPL_HEAD "{[%HEAD%]}"

#define EPG_TMPL  \
"<table border='0' cellpadding='0' cellspacing='1' width='100%' style='color: black; background-color: #036; width: 100%;'>\n"\
TMPL_ROWS \
"</table>\n"


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

#endif /* __011910__TEMPLATES_H */
/************************* History ***************************\
| $Log$
\*************************************************************/

