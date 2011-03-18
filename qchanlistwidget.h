/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 19.01.2010 / 15:34:39
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#ifndef __031811__QCHANLISTWIDGET_H
   #define __031811__QCHANLISTWIDGET_H

#include <QListWidget>
#include "cchanlistwidgetitem.h"

/********************************************************************\
|  Class: QChanListWidget
|  Date:  17.03.2011 / 9:50
|  Author: Jo2003
|  Description: special list widget used for channel list
|
\********************************************************************/
class QChanListWidget : public QListWidget
{
public:
   /* -----------------------------------------------------------------\
   |  Method: QChanListWidget / constructor
   |  Begin:  18.03.2011 / 9:15
   |  Author: Jo2003
   |  Description: construct QChanListWidget object
   |
   |  Parameters: pointer to parent widget
   |
   |  Returns: --
   \----------------------------------------------------------------- */
   QChanListWidget(QWidget * parent = 0) : QListWidget(parent)
   {
      iActFontSize = iInitialFontSize = font().pointSize();
   }

   /* -----------------------------------------------------------------\
   |  Method: ~QChanListWidget / destructor
   |  Begin:  18.03.2011 / 9:15
   |  Author: Jo2003
   |  Description: destroy QChanListWidget object
   |
   |  Parameters: --
   |
   |  Returns: --
   \----------------------------------------------------------------- */
   virtual ~QChanListWidget ()
   {
   }

   /* -----------------------------------------------------------------\
   |  Method: fontSizeDiff
   |  Begin:  18.03.2011 / 9:50
   |  Author: Jo2003
   |  Description: get font size difference to normal
   |
   |  Parameters: --
   |
   |  Returns: delat
   \----------------------------------------------------------------- */
   int fontSizeDiff()
   {
      return iActFontSize - iInitialFontSize;
   }

   /* -----------------------------------------------------------------\
   |  Method: changeFontSize
   |  Begin:  18.03.2011 / 9:15
   |  Author: Jo2003
   |  Description: change font size
   |
   |  Parameters: font point size change value
   |
   |  Returns: --
   \----------------------------------------------------------------- */
   void changeFontSize (int delta)
   {
      iActFontSize += delta;
      CChanListWidgetItem *pItem;

      for (int i = 0; i < count(); i ++)
      {
         pItem = (CChanListWidgetItem *)item(i);
         pItem->changeFontSize(delta);
      }
   }

private:
   int iInitialFontSize;
   int iActFontSize;
};

#endif // __031811__QCHANLISTWIDGET_H
