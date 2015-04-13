/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/qwatchlistdlg.h $
 *
 *  @file     qwatchlistdlg.h
 *
 *  @author   Jo2003
 *
 *  @date     06.08.2013
 *
 *  $Id: qwatchlistdlg.h 1238 2013-11-27 15:15:56Z Olenka.Joerg $
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#ifndef __20130806_QWATCHLISTDLG_H
   #define __20130806_QWATCHLISTDLG_H

#include <QDialog>
#include <QEvent>
#include <QUrl>
#include <QVector>
#include <QTimer>

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
   void on_pushRecordAll_clicked();
   void slotNextRec();

public slots:
   void slotWLRecEnded();
   void slotStopRecAll();


private:
   Ui::QWatchListDlg *ui;
   bool          _bRecAll;
   QVector<QUrl> _vUrls;
   QTimer        _tGap;

signals:
   void sigClick(QUrl);
   void sigUpdCount();
   void sigRecordArchiveFor (uint uid, int iCid, uint uiStart, int iLength);
};

#endif // __20130806_QWATCHLISTDLG_H
