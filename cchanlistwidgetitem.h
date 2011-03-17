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
#ifndef __011810__CHANLISTWIDGETITEM_H
   #define __011810__CHANLISTWIDGETITEM_H

#include <QListWidgetItem>
#include <QString>
#include "qchandetails.h"

/********************************************************************\
|  Class: CChanListWidgetItem
|  Date:  18.01.2010 / 16:20:57
|  Author: Jo2003
|  Description: advanced QListWidgetItem to store some more info
|
\********************************************************************/
class CChanListWidgetItem : public QListWidgetItem
{
public:
   // constructors ...
   CChanListWidgetItem (QListWidget * parent = 0, int type = Type);
   CChanListWidgetItem (const QString & text, QListWidget * parent = 0, int type = Type);
   CChanListWidgetItem (const QIcon & icon, const QString & text, QListWidget * parent = 0, int type = Type);
   virtual ~CChanListWidgetItem();
   virtual QString toolTip () const;

   void    setName (const QString &sName);
   void    setCid (int iId);
   QString name (void) const;
   int     cid (void);
   void    setTimes (const uint &start, const uint &end);
   uint    startTime ();
   uint    endTime ();
   void    setProgram (const QString &prog);
   QString program () const;
   void    prepareIcon(const QIcon &ico);
   void    prepareToolTip(const QString &str);
   void    init (QListWidget *parent);
   int     nest(bool bExtInfo = true);

private:
   QString sChan, sProgram, sToolTip;
   int     id;
   uint    uiStart, uiEnd;
   QChanDetails *pNestedWidget;
   QListWidget  *pParent;
   QIcon         icon;
   bool          bNested;
};

#endif /* __011810__CHANLISTWIDGETITEM_H */
/************************* History ***************************\
| $Log$
\*************************************************************/


