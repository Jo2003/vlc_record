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
   CChanListWidgetItem (const QString &sText, int iId, const QString &sName, QListWidget * parent)
         : QListWidgetItem (sText, parent)
   {
      sChan  = sName;
      id     = iId;
   }

   CChanListWidgetItem (const QString &sText, int iId, QListWidget * parent)
         : QListWidgetItem (sText, parent)
   {
      id     = iId;
   }

   CChanListWidgetItem (const QString &sText, QListWidget * parent)
         : QListWidgetItem (sText, parent)
   {
      id     = -1;
   }

   CChanListWidgetItem (const QIcon &icon, const QString &sText, int iId, const QString &sName, QListWidget * parent)
         : QListWidgetItem (icon, sText, parent)
   {
      sChan  = sName;
      id     = iId;
   }

   CChanListWidgetItem (const QIcon &icon, const QString &sText, int iId, QListWidget * parent)
         : QListWidgetItem (icon, sText, parent)
   {
      id     = iId;
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

   void SetStartEnd (const uint &start, const uint &end)
   {
      uiStart = start;
      uiEnd   = end;
   }

   uint& GetStartTime ()
   {
      return uiStart;
   }

   uint& GetEndTime ()
   {
      return uiEnd;
   }

   void SetProgram (const QString &prog)
   {
      sProgram = prog;
   }

   QString &GetProgram ()
   {
      return sProgram;
   }

protected:
   QString sChan, sProgram;
   int     id;
   uint    uiStart, uiEnd;
};

#endif /* __011810__CHANLISTWIDGETITEM_H */
/************************* History ***************************\
| $Log$
\*************************************************************/


