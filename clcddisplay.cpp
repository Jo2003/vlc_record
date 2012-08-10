/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 01.04.2010 / 14:21:44
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#include "clcddisplay.h"

/* -----------------------------------------------------------------\
|  Method: CLCDDisplay / constructor
|  Begin: 01.04.2010 / 14:26:01
|  Author: Jo2003
|  Description: construct object, init values
|
|  Parameters: text, parent, flags
|
|  Returns: --
\----------------------------------------------------------------- */
CLCDDisplay::CLCDDisplay(const QString &text, QWidget *parent, Qt::WindowFlags f)
   : QLabel(text, parent, f)
{
   lcdState = lcd::WTF;
}

/* -----------------------------------------------------------------\
|  Method: CLCDDisplay / constructor
|  Begin: 01.04.2010 / 14:26:01
|  Author: Jo2003
|  Description: construct object, init values
|
|  Parameters: parent, flags
|
|  Returns: --
\----------------------------------------------------------------- */
CLCDDisplay::CLCDDisplay(QWidget *parent, Qt::WindowFlags f)
   : QLabel(parent, f)
{
   lcdState = lcd::WTF;
}

/* -----------------------------------------------------------------\
|  Method: ~CLCDDisplay / destructor
|  Begin: 01.04.2010 / 14:26:01
|  Author: Jo2003
|  Description: clean at destruction
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
CLCDDisplay::~CLCDDisplay()
{
   // do nothing ...
}

/* -----------------------------------------------------------------\
|  Method: setHeader
|  Begin: 01.04.2010 / 14:26:01
|  Author: Jo2003
|  Description: set header text, insert into image, display
|
|  Parameters: text string
|
|  Returns: --
\----------------------------------------------------------------- */
void CLCDDisplay::setHeader(const QString &str)
{
   sHeader = str;
   addHeader();
   setPixmap(QPixmap::fromImage(lcdImg));
}

/* -----------------------------------------------------------------\
|  Method: setFooter
|  Begin: 01.04.2010 / 14:26:01
|  Author: Jo2003
|  Description: set footer text, insert into image, display
|
|  Parameters: text string
|
|  Returns: --
\----------------------------------------------------------------- */
void CLCDDisplay::setFooter(const QString &str)
{
   sFooter = str;
   addFooter();
   setPixmap(QPixmap::fromImage(lcdImg));
}

/* -----------------------------------------------------------------\
|  Method: setState [slot]
|  Begin: 01.04.2010 / 14:26:01
|  Author: Jo2003
|  Description: set image, header, footer and display
|
|  Parameters: state, head text, foot text
|
|  Returns: --
\----------------------------------------------------------------- */
void CLCDDisplay::setState(int iState, const QString &sHead, const QString &sFoot)
{
   lcdState = (lcd::eState)iState;
   sHeader  = sHead;
   sFooter  = sFoot;

   loadImage(lcdState);

   // add header and footer if needed ...
   if (sHeader != "")
   {
      addHeader();
   }

   if (sFooter != "")
   {
      addFooter();
   }

   // set / display pixmap ...
   showLCD();
}

/* -----------------------------------------------------------------\
|  Method: updateState [slot]
|  Begin: 01.04.2010 / 14:26:01
|  Author: Jo2003
|  Description: preserver header and footer, only update state image
|
|  Parameters: state
|
|  Returns: --
\----------------------------------------------------------------- */
void CLCDDisplay::updateState(int iState)
{
   lcdState = (lcd::eState)iState;

   loadImage(lcdState);

   // add header and footer if needed ...
   if (sHeader != "")
   {
      addHeader();
   }

   if (sFooter != "")
   {
      addFooter();
   }

   // set / display pixmap ...
   showLCD();
}

/* -----------------------------------------------------------------\
|  Method: clearState [slot]
|  Begin: 01.04.2010 / 14:26:01
|  Author: Jo2003
|  Description: reset display to blank without header and footer
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CLCDDisplay::clearState()
{
   // clear image and text ...
   lcdState = lcd::BLANK;
   sHeader  = "";
   sFooter  = "";

   loadImage(lcdState);

   // set / display pixmap ...
   showLCD();
}

/* -----------------------------------------------------------------\
|  Method: resetState [slot]
|  Begin: 01.04.2010 / 14:26:01
|  Author: Jo2003
|  Description: use internal stored values and renew display
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CLCDDisplay::resetState()
{
   loadImage(lcdState);

   // add header and footer if needed ...
   if (sHeader != "")
   {
      addHeader();
   }

   if (sFooter != "")
   {
      addFooter();
   }

   // set / display pixmap ...
   showLCD();
}

/* -----------------------------------------------------------------\
|  Method: addHeader
|  Begin: 01.04.2010 / 14:26:01
|  Author: Jo2003
|  Description: insert header text + shadow into image
|
|  Parameters: optional string to add
|
|  Returns: --
\----------------------------------------------------------------- */
void CLCDDisplay::addHeader(const QString &str)
{
   QPainter painter(&lcdImg);

   // init painter ...
   painter.setPen(QColor(COLOR_TEXT_SHADOW));
   painter.setFont(QFont("Monospace", 8, QFont::Bold, false));

   // paint shadow ...
   painter.drawText(3, 10, (str == "") ? sHeader : str);

   // set text color ...
   painter.setPen(QColor(COLOR_TEXT));

   // paint text ...
   painter.drawText(2, 9, (str == "") ? sHeader : str);
}

/* -----------------------------------------------------------------\
|  Method: addFooter
|  Begin: 01.04.2010 / 14:26:01
|  Author: Jo2003
|  Description: insert footer text + shadow into image
|
|  Parameters: optional string to add
|
|  Returns: --
\----------------------------------------------------------------- */
void CLCDDisplay::addFooter(const QString &str)
{
   QPainter painter(&lcdImg);

   // init painter ...
   painter.setPen(QColor(COLOR_TEXT_SHADOW));
   painter.setFont(QFont("Monospace", 8, QFont::Bold, false));

   // paint shadow ...
   painter.drawText(3, lcdImg.height() - 1, (str == "") ? sFooter : str);

   // set text color ...
   painter.setPen(QColor(COLOR_TEXT));

   // paint text ...
   painter.drawText(2, lcdImg.height() - 2, (str == "") ? sFooter : str);
}

/* -----------------------------------------------------------------\
|  Method: loadImage
|  Begin: 01.04.2010 / 14:26:01
|  Author: Jo2003
|  Description: load image from resources
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CLCDDisplay::loadImage(lcd::eState state)
{
   switch (state)
   {
   case lcd::BUFFER:
      lcdImg.load(":/lcd/buffer");
      break;
   case lcd::END:
      lcdImg.load(":/lcd/end");
      break;
   case lcd::ERROR:
      lcdImg.load(":/lcd/error");
      break;
   case lcd::OPEN:
      lcdImg.load(":/lcd/open");
      break;
   case lcd::PAUSE:
      lcdImg.load(":/lcd/pause");
      break;
   case lcd::PLAY:
      lcdImg.load(":/lcd/play");
      break;
   case lcd::READY:
      lcdImg.load(":/lcd/ready");
      break;
   case lcd::RECORD:
      lcdImg.load(":/lcd/rec");
      break;
   case lcd::STOP:
      lcdImg.load(":/lcd/stop");
      break;
   case lcd::TIMER_REC:
      lcdImg.load(":/lcd/timer_rec");
      break;
   case lcd::TIMER_STBY:
      lcdImg.load(":/lcd/timer_stby");
      break;
   case lcd::BLANK:
   case lcd::WTF:
   default:
      lcdImg.load(":/lcd/blank");
      break;
   }
}

/* -----------------------------------------------------------------\
|  Method: showLCD
|  Begin: 01.04.2010 / 14:26:01
|  Author: Jo2003
|  Description: resize display and show it
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
void CLCDDisplay::showLCD()
{
   // resize label field ...
   resize(lcdImg.width() + frameWidth() * 2, lcdImg.height() + frameWidth() * 2);

   // set / display pixmap ...
   setPixmap(QPixmap::fromImage(lcdImg));
}

/* -----------------------------------------------------------------\
|  Method: bufferPercent
|  Begin: 10.08.2012
|  Author: Jo2003
|  Description: show download buffer in percent,
|               don't touch internally stored values
|
|  Parameters: buffer value in %
|
|  Returns: --
\----------------------------------------------------------------- */
void CLCDDisplay::bufferPercent(int percent)
{
   if (percent < 100)
   {
      loadImage(lcd::BUFFER);
      addHeader(QString("  %1% ...").arg(percent));
      showLCD();
   }
   else
   {
      resetState();
   }
}

/************************* History ***************************\
| $Log$
\*************************************************************/
