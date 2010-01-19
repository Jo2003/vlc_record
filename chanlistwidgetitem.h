/*********************** Information *************************\
| $HeadURL: svn://172.25.1.40/joergn/trunk/vlc-record/chanlistwidgetitem.h $
| 
| Author: Joerg Neubert
|
| Begin: 18.01.2010 / 16:20:26
| 
| Last edited by: $Author: joergn $
| 
| $Id: chanlistwidgetitem.h 173 2010-01-18 15:43:19Z joergn $
\*************************************************************/
#ifndef __011810__CHANLISTWIDGETITEM_H
   #define __011810__CHANLISTWIDGETITEM_H

#include <QListWidgetItem>
#include <QString>

/********************************************************************\
|  Class: CChanListWidgetItem
|  Date:  18.01.2010 / 16:20:57
|  Author: Joerg Neubert
|  Description: advanced QListWidgetItem to store some more info
|
\********************************************************************/
class CChanListWidgetItem : public QListWidgetItem
{
public:
   // constructors ...
   CChanListWidgetItem (const QString &sText, int iId, const QString &sName, QListWidget * parent)
         : QListWidgetItem (sText, parent)
   {
      sChan  = sName;
      id     = iId;
      sStart = "";
      sEnd   = "";
   }

   CChanListWidgetItem (const QString &sText, int iId, QListWidget * parent)
         : QListWidgetItem (sText, parent)
   {
      sChan  = "";
      id     = iId;
      sStart = "";
      sEnd   = "";
   }

   CChanListWidgetItem (const QString &sText, QListWidget * parent)
         : QListWidgetItem (sText, parent)
   {
      sChan  = "";
      id     = -1;
      sStart = "";
      sEnd   = "";
   }

   CChanListWidgetItem (const QIcon &icon, const QString &sText, int iId, const QString &sName, QListWidget * parent)
         : QListWidgetItem (icon, sText, parent)
   {
      sChan  = sName;
      id     = iId;
      sStart = "";
      sEnd   = "";
   }

   CChanListWidgetItem (const QIcon &icon, const QString &sText, int iId, QListWidget * parent)
         : QListWidgetItem (icon, sText, parent)
   {
      sChan  = "";
      id     = iId;
      sStart = "";
      sEnd   = "";
   }

   void SetName (const QString &sName)
   {
      sChan = sName;
   }

   void SetId (int iId)
   {
      id = iId;
   }

   QString &GetName (void)
   {
      return sChan;
   }

   int GetId (void)
   {
      return id;
   }

   void SetStartEnd (const QString &start, const QString &end)
   {
      sStart = start;
      sEnd   = end;
   }

   QString &GetStartTime ()
   {
      return sStart;
   }

   QString &GetEndTime ()
   {
      return sEnd;
   }

protected:
   QString sChan, sStart, sEnd;
   int     id;
};

#endif /* __011810__CHANLISTWIDGETITEM_H */
/************************* History ***************************\
| $Log$
\*************************************************************/


