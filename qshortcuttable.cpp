/*------------------------------ Information --------------------------*/ /**
*
*  $HeadURL$
*
*  @file   qshortcuttable.cpp
*
*  @author Jo2003
*
*  @date   06.04.2011 / 9:25
*
*  $Id$
*
*/ //------------------------------------------------------------------------
#include "qshortcuttable.h"

// storage db ...
extern CVlcRecDB *pDb;

//---------------------------------------------------------------------------
//
//! \brief   constructs QShortCutTable object
//
//! \author  Jo2003
//! \date    06.04.2011 / 9:25
//
//! \param   parent pointer to parent widget
//
//! \return  --
//---------------------------------------------------------------------------
QShortCutTable::QShortCutTable(QWidget * parent) :
   QTableWidget(parent)
{

}

//---------------------------------------------------------------------------
//
//! \brief   constructs QShortCutTable object
//
//! \author  Jo2003
//! \date    06.04.2011 / 9:25
//
//! \param   rows initial number of table rows
//! \param   columns inition number of table columns
//! \param   parent pointer to parent widget
//
//! \return  --
//---------------------------------------------------------------------------
QShortCutTable::QShortCutTable(int rows, int columns, QWidget *parent) :
   QTableWidget(rows, columns, parent)
{

}

//---------------------------------------------------------------------------
//
//! \brief   destroys QShortCutTable object
//
//! \author  Jo2003
//! \date    06.04.2011 / 9:50
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
QShortCutTable::~QShortCutTable()
{

}

//---------------------------------------------------------------------------
//
//! \brief   add a shortcut to shortcut table
//
//! \author  Jo2003
//! \date    15.03.2011 / 15:45
//
//! \param   descr description text
//! \param   target target object name
//! \param   slot slot for action to trigger
//! \param   keys shortcut string
//
//! \return  --
//---------------------------------------------------------------------------
void QShortCutTable::addShortCut(const QString &descr, const QString &target,
                              const QString &slot, const QString &keys)
{
   QString           shortCut;
   int               iRow     = rowCount();
   QTableWidgetItem *pItem    = new QTableWidgetItem (descr);
   CShortCutGrabber *pGrab    = new CShortCutGrabber (this, iRow);

   if((shortCut = pDb->getShortCut(target, slot)) == "")
   {
      shortCut = keys;
   }

   pGrab->setTarget(target);
   pGrab->setSlot(slot);
   pGrab->setKeySequence(QKeySequence(shortCut), QKeySequence(keys));

   connect (pGrab, SIGNAL(keySequenceChanged(QKeySequence,int)),
      this, SLOT(slotShortCutChanged(QKeySequence,int)));

   setRowCount(iRow + 1);
   setItem(iRow, 0, pItem);
   setCellWidget(iRow, 1, pGrab);

   resizeColumnsToContents();
}

//---------------------------------------------------------------------------
//
//! \brief   get matching shortcut
//
//! \author  Jo2003
//! \date    15.03.2011 / 15:45
//
//! \param   target target object name
//! \param   slot slot for action to trigger
//
//! \return  shortcut string
//---------------------------------------------------------------------------
QString QShortCutTable::shortCut(const QString &target, const QString &slot) const
{
   CShortCutGrabber *pGrab;
   int     i;
   QString key;

   for (i = 0; i < rowCount(); i++)
   {
      pGrab = (CShortCutGrabber *)cellWidget(i, 1);

      if ((pGrab->target() == target) && (pGrab->slot() == slot))
      {
         key = pGrab->shortCutString();
         break;
      }
   }

   return key;
}

//---------------------------------------------------------------------------
//
//! \brief   delete matching shortcut from shortcut table only
//
//! \author  Jo2003
//! \date    06.04.2011 / 9:25
//
//! \param   target target object name
//! \param   slot slot for action to trigger
//
//! \return  --
//---------------------------------------------------------------------------
void QShortCutTable::delShortCut(const QString &target, const QString &slot)
{
   CShortCutGrabber *pGrab;
   int     i;

   for (i = 0; i < rowCount(); i++)
   {
      pGrab = (CShortCutGrabber *)cellWidget(i, 1);

      if ((pGrab->target() == target) && (pGrab->slot() == slot))
      {
         disconnect (pGrab, SIGNAL(keySequenceChanged(QKeySequence,int)),
            this, SLOT(slotShortCutChanged(QKeySequence,int)));

         removeRow(i);
         break;
      }
   }
}

//---------------------------------------------------------------------------
//
//! \brief   update description text
//
//! \author  Jo2003
//! \date    06.04.2011 / 9:25
//
//! \param   descr description text
//! \param   target target object name
//! \param   slot slot for action to trigger
//
//! \return  --
//---------------------------------------------------------------------------
void QShortCutTable::updateShortcutDescr (const QString &descr, const QString &target, const QString &slot)
{
   CShortCutGrabber *pGrab;
   int     i;

   for (i = 0; i < rowCount(); i++)
   {
      pGrab = (CShortCutGrabber *)cellWidget(i, 1);

      if ((pGrab->target() == target) && (pGrab->slot() == slot))
      {
         item (i, 0)->setText(descr);
         break;
      }
   }
}

//---------------------------------------------------------------------------
//
//! \brief   shortcut in any row was changed [slot]
//
//! \author  Jo2003
//! \date    06.04.2011 / 9:50
//
//! \param   shortCut changed shortcut
//! \param   iRow index of row in which shortcut was changed
//
//! \return  --
//---------------------------------------------------------------------------
void QShortCutTable::slotShortCutChanged(const QKeySequence &shortCut, int iRow)
{
   QVector<int> matchingRows;
   int i;

   for (i = 0; i < rowCount(); i++)
   {
      if (i != iRow)
      {
         if (((CShortCutGrabber *)cellWidget(i, 1))->keySequence() == shortCut)
         {
            matchingRows.push_back(i);
         }
      }
   }

   if (matchingRows.count())
   {
      matchingRows.push_back(iRow);
   }

   // mark duplicated rows, unmark unique rows ...
   for (i = 0; i < rowCount(); i++)
   {
      if (matchingRows.contains(i))
      {
         ((CShortCutGrabber *)cellWidget(i, 1))->markRed();
      }
      else
      {
         ((CShortCutGrabber *)cellWidget(i, 1))->unMark();
      }
   }
}

//---------------------------------------------------------------------------
//
//! \brief   revert all changed shortcuts
//
//! \author  Jo2003
//! \date    06.04.2011 / 11:05
//
//! \param   --
//
//! \return  --
//---------------------------------------------------------------------------
void QShortCutTable::revert ()
{
   for (int i = 0; i < rowCount(); i++)
   {
      ((CShortCutGrabber *)cellWidget(i, 1))->revert();
   }
}
