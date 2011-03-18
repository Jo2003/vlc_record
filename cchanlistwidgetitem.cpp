/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 18.01.2010 / 16:20:26
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#include "cchanlistwidgetitem.h"

/* -----------------------------------------------------------------\
|  Method: CChanListWidgetItem / constructor
|  Begin: 17.03.2011 / 15:10
|  Author: Jo2003
|  Description: construct CChanListWidgetItem object
|
|  Parameters: pointer to parent widget, type
|
|  Returns: --
\----------------------------------------------------------------- */
CChanListWidgetItem::CChanListWidgetItem (QListWidget * parent, int type)
   : QListWidgetItem(parent, type)
{
   init(parent);
}

/* -----------------------------------------------------------------\
|  Method: CChanListWidgetItem / constructor
|  Begin: 17.03.2011 / 15:10
|  Author: Jo2003
|  Description: construct CChanListWidgetItem object
|
|  Parameters: text, pointer to parent widget, type
|
|  Returns: --
\----------------------------------------------------------------- */
CChanListWidgetItem::CChanListWidgetItem (const QString & text, QListWidget * parent, int type)
   : QListWidgetItem (text, parent, type)
{
   init(parent);
}

/* -----------------------------------------------------------------\
|  Method: CChanListWidgetItem / constructor
|  Begin: 17.03.2011 / 15:10
|  Author: Jo2003
|  Description: construct CChanListWidgetItem object
|
|  Parameters: icon, text, pointer to parent widget, type
|
|  Returns: --
\----------------------------------------------------------------- */
CChanListWidgetItem::CChanListWidgetItem (const QIcon & icon, const QString & text,
                                          QListWidget * parent, int type)
   : QListWidgetItem (icon, text, parent, type)
{
   init(parent);
}

/* -----------------------------------------------------------------\
|  Method: ~CChanListWidgetItem / destructor
|  Begin: 17.03.2011 / 15:10
|  Author: Jo2003
|  Description: destroy CChanListWidgetItem object
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
CChanListWidgetItem::~CChanListWidgetItem()
{

}

/* -----------------------------------------------------------------\
|  Method: init
|  Begin: 17.03.2011 / 15:10
|  Author: Jo2003
|  Description: init values
|
|  Parameters: pointer to parent widget
|
|  Returns: --
\----------------------------------------------------------------- */
void CChanListWidgetItem::init(QListWidget *parent)
{
   pParent       =  parent;
   uiEnd         =  0;
   uiStart       =  0;
   pNestedWidget =  NULL;
   id            = -1;
   bNested       = false;
}

/* -----------------------------------------------------------------\
|  Method: setName
|  Begin: 17.03.2011 / 15:10
|  Author: Jo2003
|  Description: set channel name to display
|
|  Parameters: name
|
|  Returns: --
\----------------------------------------------------------------- */
void CChanListWidgetItem::setName (const QString &sName)
{
   sChan = sName;
}

/* -----------------------------------------------------------------\
|  Method: setCid
|  Begin: 17.03.2011 / 15:10
|  Author: Jo2003
|  Description: set channel id
|
|  Parameters: cid
|
|  Returns: --
\----------------------------------------------------------------- */
void CChanListWidgetItem::setCid (int iId)
{
   id = iId;
}

/* -----------------------------------------------------------------\
|  Method: name
|  Begin: 17.03.2011 / 15:10
|  Author: Jo2003
|  Description: get channel name
|
|  Parameters: --
|
|  Returns: channel name
\----------------------------------------------------------------- */
QString CChanListWidgetItem::name (void) const
{
   return sChan;
}

/* -----------------------------------------------------------------\
|  Method: cid
|  Begin: 17.03.2011 / 15:10
|  Author: Jo2003
|  Description: get channel id
|
|  Parameters: --
|
|  Returns: cid
\----------------------------------------------------------------- */
int CChanListWidgetItem::cid (void)
{
   return id;
}

/* -----------------------------------------------------------------\
|  Method: setTimes
|  Begin: 17.03.2011 / 15:10
|  Author: Jo2003
|  Description: store start and end time
|
|  Parameters: start time, end time
|
|  Returns: --
\----------------------------------------------------------------- */
void CChanListWidgetItem::setTimes (const uint &start, const uint &end)
{
   uiStart = start;
   uiEnd   = end;
}

/* -----------------------------------------------------------------\
|  Method: startTime
|  Begin: 17.03.2011 / 15:10
|  Author: Jo2003
|  Description: get start time
|
|  Parameters: --
|
|  Returns: start time
\----------------------------------------------------------------- */
uint CChanListWidgetItem::startTime ()
{
   return uiStart;
}

/* -----------------------------------------------------------------\
|  Method: endTime
|  Begin: 17.03.2011 / 15:10
|  Author: Jo2003
|  Description: get end time
|
|  Parameters: --
|
|  Returns: end time
\----------------------------------------------------------------- */
uint CChanListWidgetItem::endTime ()
{
   return uiEnd;
}

/* -----------------------------------------------------------------\
|  Method: setProgram
|  Begin: 17.03.2011 / 15:10
|  Author: Jo2003
|  Description: set program name
|
|  Parameters: program name (show name)
|
|  Returns: --
\----------------------------------------------------------------- */
void CChanListWidgetItem::setProgram (const QString &prog)
{
   sProgram = prog;
}

/* -----------------------------------------------------------------\
|  Method: program
|  Begin: 17.03.2011 / 15:10
|  Author: Jo2003
|  Description: get program name
|
|  Parameters: --
|
|  Returns: program name
\----------------------------------------------------------------- */
QString CChanListWidgetItem::program () const
{
   return sProgram;
}

/* -----------------------------------------------------------------\
|  Method: prepareIcon
|  Begin: 17.03.2011 / 15:10
|  Author: Jo2003
|  Description: buffer item for later use
|
|  Parameters: ref. to icon
|
|  Returns: --
\----------------------------------------------------------------- */
void CChanListWidgetItem::prepareIcon(const QIcon &ico)
{
   icon = ico;
}

/* -----------------------------------------------------------------\
|  Method: prepareToolTip
|  Begin: 17.03.2011 / 15:30
|  Author: Jo2003
|  Description: store tooltip for later use
|
|  Parameters: ref. to tool tip
|
|  Returns: --
\----------------------------------------------------------------- */
void CChanListWidgetItem::prepareToolTip(const QString &str)
{
   sToolTip = str;
}

/* -----------------------------------------------------------------\
|  Method: toolTip
|  Begin: 17.03.2011 / 15:30
|  Author: Jo2003
|  Description: get tool tip
|
|  Parameters: --
|
|  Returns: tool tip
\----------------------------------------------------------------- */
QString CChanListWidgetItem::toolTip () const
{
   if (bNested)
   {
      return sToolTip;
   }
   else
   {
      return QListWidgetItem::toolTip();
   }
}

/* -----------------------------------------------------------------\
|  Method: nest
|  Begin: 17.03.2011 / 15:10
|  Author: Jo2003
|  Description: nest extended info widget if all is well
|
|  Parameters: ext. info flag
|
|  Returns: 1 --> QWidget nested
|           0 --> using QListWidgetItem
\----------------------------------------------------------------- */
int CChanListWidgetItem::nest(bool bExtInfo)
{
   if (uiStart && uiEnd && pParent && bExtInfo && (sProgram != ""))
   {
      bNested       = true;
      pNestedWidget = new QChanDetails();
      pNestedWidget->setChannelName(sChan);
      pNestedWidget->setProgram(sProgram);
      pNestedWidget->setTimes(uiStart, uiEnd);
      pNestedWidget->setIcon(icon);
      pParent->setItemWidget(this, pNestedWidget);
      setSizeHint(pNestedWidget->size());
   }
   else
   {
      bNested = false;
      setText(sChan);
      setIcon(icon);
      setToolTip(sToolTip);
   }

   return (bNested) ? 1 : 0;
}

/* -----------------------------------------------------------------\
|  Method: changeFontSize
|  Begin: 18.03.2011 / 9;30
|  Author: Jo2003
|  Description: change font size
|
|  Parameters: change value (delta)
|
|  Returns: --
\----------------------------------------------------------------- */
void CChanListWidgetItem::changeFontSize(int delta)
{
   if (bNested)
   {
      pNestedWidget->changeFontSize(delta);
      setSizeHint(pNestedWidget->size());
   }
   else
   {
      QFont f = font();
      f.setPointSize(f.pointSize() + delta);
      setFont(f);
   }
}
