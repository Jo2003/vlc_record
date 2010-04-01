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
#ifndef __040110__CLCDDISPLAY_H
   #define __040110__CLCDDISPLAY_H

#include <QLabel>
#include <QImage>
#include <QPainter>
#include <QPixmap>

#define COLOR_TEXT          "#333"
#define COLOR_TEXT_SHADOW   "#084"

//===============================================================
// namespace for lcd
//===============================================================
namespace lcd
{
   enum eState
   {
      BLANK,
      BUFFER,
      END,
      ERROR,
      OPEN,
      PAUSE,
      PLAY,
      READY,
      RECORD,
      STOP,
      TIMER_REC,
      TIMER_STBY,
      WTF = 255
   };
}

/********************************************************************\
|  Class: CLCDDisplay
|  Date:  01.04.2010 / 14:22:18
|  Author: Jo2003
|  Description: advanced QLabel represents a LC display
|
\********************************************************************/
class CLCDDisplay : public QLabel
{
   Q_OBJECT

public:
   CLCDDisplay (QWidget * parent = 0, Qt::WindowFlags f = 0);
   CLCDDisplay (const QString & text, QWidget * parent = 0, Qt::WindowFlags f = 0);
   virtual ~CLCDDisplay ();

protected:
   void addHeader ();
   void addFooter ();
   void loadImage (lcd::eState state);
   void showLCD ();

private:
   lcd::eState lcdState;
   QString sHeader, sFooter;
   QImage  lcdImg;

public slots:
   void updateState (int iState);
   void resetState ();
   void clearState ();
   void setState (int iState, const QString& sHead = QString(), const QString& sFoot = QString());
   void setHeader (const QString& str);
   void setFooter (const QString& str);
};

#endif // __040110__CLCDDISPLAY_H
/************************* History ***************************\
| $Log$
\*************************************************************/

