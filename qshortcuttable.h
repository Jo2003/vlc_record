/*------------------------------ Information --------------------------*/ /**
*
*  $HeadURL: https://vlc-record.googlecode.com/svn/branches/sunduk.tv/qshortcuttable.h $
*
*  @file   qshortcuttable.h
*
*  @author Jo2003
*
*  @date   06.04.2011 / 9:14
*
*  $Id: qshortcuttable.h 497 2011-04-06 09:48:07Z Olenka.Joerg $
*
*/ //------------------------------------------------------------------------
#ifndef __060411__QSHORTCUTTABLE_H
   #define __060411__QSHORTCUTTABLE_H

#include <QTableWidget>
#include <QVector>

#include "cvlcrecdb.h"
#include "cshortcutgrabber.h"

//---------------------------------------------------------------------------
//! \class   QShortCutTable
//! \date    06.04.2011 / 9:14
//! \author  Jo2003
//! \brief   a table widget to handle short cut stuff
//---------------------------------------------------------------------------
class QShortCutTable : public QTableWidget
{
   Q_OBJECT

public:
   QShortCutTable(QWidget * parent = 0);
   QShortCutTable(int rows, int columns, QWidget * parent = 0);
   virtual ~QShortCutTable();

   void    addShortCut (const QString& descr, const QString& target, const QString& slot, const QString& keys);
   void    delShortCut (const QString& target, const QString& slot);
   void    updateShortcutDescr(const QString& descr, const QString& target, const QString& slot);
   void    revert ();
   QString shortCut (const QString& target, const QString& slot) const;

protected:

signals:

public slots:

private slots:
   void slotShortCutChanged (const QKeySequence & shortCut, int iRow);
};

#endif // __060411__QSHORTCUTTABLE_H
