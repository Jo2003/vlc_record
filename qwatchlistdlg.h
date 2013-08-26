/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qwatchlistdlg.h
 *
 *  @author   Jo2003
 *
 *  @date     06.08.2013
 *
 *  $Id$
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#ifndef __20130806_QWATCHLISTDLG_H
   #define __20130806_QWATCHLISTDLG_H

#include <QDialog>
#include <QEvent>
#include <QUrl>

#include "cvlcrecdb.h"
#include "defdef.h"

namespace Ui {
   class QWatchListDlg;
}

//---------------------------------------------------------------------------
//! \class   QWatchListDlg
//! \date    06.08.2013
//! \author  Jo2003
//! \brief   dialog for watch list
//---------------------------------------------------------------------------
class QWatchListDlg : public QDialog
{
   Q_OBJECT
   
public:
   explicit QWatchListDlg(QWidget *parent = 0);
   ~QWatchListDlg();
   void buildWatchTab();
   int  cleanWatchList ();
   int  count();

protected:
   virtual void changeEvent(QEvent *e);

private slots:
   void slotListAnchor(QUrl url);

private:
   Ui::QWatchListDlg *ui;

signals:
   void sigClick(QUrl);
   void sigUpdCount();
};

#endif // __20130806_QWATCHLISTDLG_H
