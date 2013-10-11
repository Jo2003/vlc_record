/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     chtmlwriter.cpp
 *
 *  @author   Jo2003
 *
 *  @date     09.08.2013
 *
 *  $Id$
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#include "chtmlwriter.h"
#include "templates.h"
#include "defdef.h"
#include <QFileInfo>
#include <QStringList>
#include <QDateTime>

//---------------------------------------------------------------------------
//
//! \brief   constructs CHtmlWriter object
//
//! \author  Jo2003
//! \date    12.08.2013
//
//! \param   parent (QObject *) pointer to parent object
//
//! \return  --
//---------------------------------------------------------------------------
CHtmlWriter::CHtmlWriter(QObject *parent) :
   QObject(parent)
{
}

//---------------------------------------------------------------------------
//
//! \brief   destroys CHtmlWriter object
//
//! \author  Jo2003
//! \date    12.08.2013
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
CHtmlWriter::~CHtmlWriter()
{
}

//---------------------------------------------------------------------------
//
//! \brief   create whole html page
//
//! \author  Jo2003
//! \date    12.08.2013
//
//! \param   content (const QString&) page content
//! \param   title (const QString&) optional page title
//! \param   css (const QString&) optional page style sheet
//
//! \return  html page as string
//---------------------------------------------------------------------------
QString CHtmlWriter::htmlPage(const QString &content, const QString &title, const QString &css)
{
   QString s =
         "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">"
           "<html>"
             "<head>"
               "<meta http-equiv=\"content-type\" content=\"text/html; charset=UTF-8\" />"
               "<meta name=\"qrichtext\" content=\"1\" />"
               "<title>" TMPL_TITLE "</title>"
               "<style type=\"text/css\">"
                 TMPL_CSS
               "</style>"
             "</head>"
           "<body>"
             TMPL_CONT
           "</body>"
         "</html>\n";

   s.replace(TMPL_TITLE, title.isEmpty() ? "Qt Site"         : title);
   s.replace(TMPL_CSS  , css.isEmpty()   ? "// no style ..." : css);
   s.replace(TMPL_CONT , content);

   return s;
}

//---------------------------------------------------------------------------
//
//! \brief   create a html tag (open + close tag)
//
//! \author  Jo2003
//! \date    12.08.2013
//
//! \param   tag (const QString&) tag name e.g. span, div, etc.
//! \param   content (const QString&) content within this tag
//! \param   style (const QString&) css for this tag
//! \param   attr (const QString&) additional html attributes
//
//! \return  html code of this tag
//---------------------------------------------------------------------------
QString CHtmlWriter::htmlTag(const QString &tag, const QString &content, const QString &style, const QString &attr)
{
   QString s = "<" TMPL_NAME TMPL_CSS TMPL_ATTR">" TMPL_CONT "</" TMPL_NAME ">";

   s.replace(TMPL_NAME, tag);
   s.replace(TMPL_CSS , style.isEmpty() ? "" : QString(" style='" TMPL_CSS "'").replace(TMPL_CSS, style));
   s.replace(TMPL_ATTR, attr.isEmpty()  ? "" : (" " + attr));
   s.replace(TMPL_CONT, content);

   return s;
}

//---------------------------------------------------------------------------
//
//! \brief   create a simple html tag (without close tag)
//
//! \author  Jo2003
//! \date    12.08.2013
//
//! \param   tag (const QString&) tag name e.g. img, br, etc.
//! \param   content (const QString&) content within this tag
//! \param   style (const QString&) css for this tag
//
//! \return  html code of this tag
//---------------------------------------------------------------------------
QString CHtmlWriter::simpleTag(const QString& tag, const QString &content, const QString& style)
{
   QString s = "<" TMPL_NAME " " TMPL_CSS " " TMPL_CONT " />";
   s.replace(TMPL_NAME, tag);
   s.replace(TMPL_CSS , style.isEmpty() ? "" : QString("style='" TMPL_CSS "'").replace(TMPL_CSS, style));
   s.replace(TMPL_CONT, content);

   return s;
}

//---------------------------------------------------------------------------
//
//! \brief   create a table cell
//
//! \author  Jo2003
//! \date    12.08.2013
//
//! \param   content (const QString&) content within this tag
//! \param   style (const QString&) css for this tag
//! \param   colspan (int) td colspan attribute
//! \param   align (const QString&) text align attribute
//! \param   valign (const QString&) vertical align attribute
//
//! \return  html code of table cell
//---------------------------------------------------------------------------
QString CHtmlWriter::tableCell (const QString &content, const QString& style, int colspan, const QString &align, const QString &valign)
{
   QString s = "<td" TMPL_ATTR TMPL_ALIGN TMPL_VALIGN TMPL_CSS">" TMPL_CONT "</td>";

   s.replace(TMPL_ALIGN , align.isEmpty() ? "" : QString(" align='" TMPL_ALIGN "'").replace(TMPL_ALIGN, align));
   s.replace(TMPL_VALIGN, valign.isEmpty() ? "" : QString(" valign='" TMPL_VALIGN "'").replace(TMPL_VALIGN, valign));
   s.replace(TMPL_ATTR  , (colspan > 1) ? QString(" colspan='" TMPL_ATTR "'").replace(TMPL_ATTR, QString::number(colspan)) : "");
   s.replace(TMPL_CSS   , style.isEmpty() ? "" : QString(" style='" TMPL_CSS "'").replace(TMPL_CSS, style));
   s.replace(TMPL_CONT  , content);

   return s;
}

//---------------------------------------------------------------------------
//
//! \brief   create a table headline cell
//
//! \author  Jo2003
//! \date    12.08.2013
//
//! \param   content (const QString&) content within this tag
//! \param   style (const QString&) css for this tag
//! \param   colspan (int) td colspan attribute
//! \param   align (const QString&) text align attribute
//! \param   valign (const QString&) vertical align attribute
//
//! \return  html code of table cell
//---------------------------------------------------------------------------
QString CHtmlWriter::tableHead(const QString &content, const QString &style, int colspan, const QString &align, const QString &valign)
{
   QString s = "<th" TMPL_ATTR TMPL_ALIGN TMPL_VALIGN TMPL_CSS">" TMPL_CONT "</th>";

   s.replace(TMPL_ALIGN , align.isEmpty() ? "" : QString(" align='" TMPL_ALIGN "'").replace(TMPL_ALIGN, align));
   s.replace(TMPL_VALIGN, valign.isEmpty() ? "" : QString(" valign='" TMPL_VALIGN "'").replace(TMPL_VALIGN, valign));
   s.replace(TMPL_ATTR  , (colspan > 1) ? QString(" colspan='" TMPL_ATTR "'").replace(TMPL_ATTR, QString::number(colspan)) : "");
   s.replace(TMPL_CSS   , style.isEmpty() ? "" : QString(" style='" TMPL_CSS "'").replace(TMPL_CSS, style));
   s.replace(TMPL_CONT  , content);

   return s;
}

//---------------------------------------------------------------------------
//
//! \brief   create a page divider
//
//! \author  Jo2003
//! \date    12.08.2013
//
//! \param   content (const QString&) content within this tag
//! \param   style (const QString&) css for this tag
//! \param   align (const QString&) text align attribute
//
//! \return  html code of tag
//---------------------------------------------------------------------------
QString CHtmlWriter::div(const QString &content, const QString &style, const QString &align)
{
   QString s = "<div" TMPL_ALIGN TMPL_CSS">" TMPL_CONT "</div>";

   s.replace(TMPL_ALIGN , align.isEmpty() ? "" : QString(" align='" TMPL_ALIGN "'").replace(TMPL_ALIGN, align));
   s.replace(TMPL_CSS   , style.isEmpty() ? "" : QString(" style='" TMPL_CSS "'").replace(TMPL_CSS, style));
   s.replace(TMPL_CONT  , content);

   return s;
}

//---------------------------------------------------------------------------
//
//! \brief   create a text divider
//
//! \author  Jo2003
//! \date    12.08.2013
//
//! \param   content (const QString&) content within this tag
//! \param   style (const QString&) css for this tag
//
//! \return  html code of tag
//---------------------------------------------------------------------------
QString CHtmlWriter::span (const QString &content, const QString& style)
{
   return htmlTag("span", content, style);
}

//---------------------------------------------------------------------------
//
//! \brief   create a paragraph
//
//! \author  Jo2003
//! \date    12.08.2013
//
//! \param   content (const QString&) content within this tag
//! \param   style (const QString&) css for this tag
//
//! \return  html code of tag
//---------------------------------------------------------------------------
QString CHtmlWriter::paragraph(const QString &content, const QString& style)
{
   return htmlTag("p", content, style);
}

//---------------------------------------------------------------------------
//
//! \brief   create a visible table
//
//! \author  Jo2003
//! \date    12.08.2013
//
//! \param   content (const QString&) content within this tag
//! \param   style (const QString&) css for this tag
//
//! \return  html code of tag
//---------------------------------------------------------------------------
QString CHtmlWriter::table(const QString &content, const QString &style)
{
   QString s = "<table border='0' cellpadding='0' cellspacing='" DEF_CELLSPACING "' width='100%' " TMPL_CSS ">" TMPL_CONT "</table>";

   s.replace(TMPL_CSS , style.isEmpty() ? "" : QString(" style='" TMPL_CSS "'").replace(TMPL_CSS, style));
   s.replace(TMPL_CONT, content);

   return s;
}

//---------------------------------------------------------------------------
//
//! \brief   create a table row
//
//! \author  Jo2003
//! \date    12.08.2013
//
//! \param   content (const QString&) content within this tag
//! \param   style (const QString&) css for this tag
//
//! \return  html code of tag
//---------------------------------------------------------------------------
QString CHtmlWriter::tableRow(const QString &content, const QString &style)
{
   return htmlTag("tr", content, style);
}

//---------------------------------------------------------------------------
//
//! \brief   create a link
//
//! \author  Jo2003
//! \date    12.08.2013
//
//! \param   href (const QString&) link referer
//! \param   content (const QString&) content within this tag
//! \param   title (const QString&) title for this tag
//! \param   style (const QString&) css for this tag
//
//! \return  html code of tag
//---------------------------------------------------------------------------
QString CHtmlWriter::link(const QString &href, const QString &content, const QString& title, const QString& style)
{
   QString s = "<a href='" TMPL_LINK "' title='" TMPL_TITLE "'" TMPL_CSS ">" TMPL_CONT "</a>";

   s.replace(TMPL_LINK , href);
   s.replace(TMPL_TITLE, title);
   s.replace(TMPL_CONT , content);
   s.replace(TMPL_CSS , style.isEmpty() ? "" : QString(" style='" TMPL_CSS "'").replace(TMPL_CSS, style));

   return s;
}

//---------------------------------------------------------------------------
//
//! \brief   create an image
//
//! \author  Jo2003
//! \date    12.08.2013
//
//! \param   src (const QString&) source url
//! \param   width (int) optional width in px
//! \param   height (int) optional height in px
//! \param   style (const QString&) css for this tag
//! \param   title (const QString&) title for this tag
//
//! \return  html code of tag
//---------------------------------------------------------------------------
QString CHtmlWriter::image(const QString &src, int width, int height, const QString &style, const QString &title)
{
   QFileInfo fi(src);
   QString   s = "src='" TMPL_LINK "' alt='" TMPL_NAME "' title='" TMPL_TITLE "'";

   if (width)
   {
      s += " width='" TMPL_WIDTH "'";
      s.replace(TMPL_WIDTH , QString::number(width));
   }

   if (height)
   {
      s += " height='" TMPL_HEIGHT "'";
      s.replace(TMPL_HEIGHT, QString::number(height));
   }


   s.replace(TMPL_LINK  , src);
   s.replace(TMPL_TITLE , title);
   s.replace(TMPL_NAME  , fi.baseName());

   return simpleTag("img", s, style);
}

//---------------------------------------------------------------------------
//
//! \brief   create a table with one row and cell
//
//! \author  Jo2003
//! \date    12.08.2013
//
//! \param   content (const QString&) content within this tag
//! \param   style (const QString&) css for this tag
//
//! \return  html code of tag
//---------------------------------------------------------------------------
QString CHtmlWriter::oneCellPage (const QString& content, const QString& style)
{
   QString s = tableCell(content, style);
   s         = tableRow(s);
   s         = table(s, TMPL_TAB_STYLE);

   return htmlPage(s);
}

//---------------------------------------------------------------------------
//
//! \brief   create short program info (tooltip)
//
//! \author  Jo2003
//! \date    13.08.2013
//
//! \param   name (const QString&) show name
//! \param   prog (const QString&) program description
//! \param   start (uint) unix timestamp of start time
//! \param   end (uint) unix timestamp of end time
//! \param   is (int) optional timeshift value in seconds
//! \param   style (const QString&) css for this tag
//
//! \return  html code of tag
//---------------------------------------------------------------------------
QString CHtmlWriter::createTooltip (const QString &name, const QString &prog, uint start, uint end, int ts, const QString &style)
{
   // create tool tip with programm info ...
   QString sStart  = QDateTime::fromTime_t(start + ts).toString("dd. MMM yyyy, h:mm");
   QString sEnd    = end ? (" - " + QDateTime::fromTime_t(end + ts).toString("h:mm")) : "";
   QString sLength = end ? (" (" + tr("%1 min.").arg((end - start) / 60) + ")")  : "";
   QStringList sl  = prog.split("\n");

   QString s = tableCell(htmlTag("b", name,  "color: red; font-size: large;"), "", 1, "left" );
   s        += tableCell(span(sStart + sEnd + sLength, "color: #080;")       , "", 1, "right");
   s         = tableRow(s);
   s         = table(s);
   s        += htmlTag("b", sl.at(0)) +  "<br />";
   if (sl.count() > 1) s += span(sl.at(1), "color: #666;") + "<br />";


   return oneCellPage(s, style.isEmpty() ? TMPL_ONE_CELL : style);
}
