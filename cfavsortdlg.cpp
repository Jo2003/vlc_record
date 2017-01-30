/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     cfavsortdlg.h
 *
 *  @author   Jo2003
 *
 *  @date     30.01.2017
 *
 *  $Id$
 *
 *///------------------------- (c) 2017 by Jo2003  --------------------------
#include "cfavsortdlg.h"
#include "ui_cfavsortdlg.h"

#include "clogfile.h"

// for logging ...
extern CLogFile VlcLog;

// ------------------------------------------------------------------
/// \brief create object
/// \param parent parent widet
/// \param model model pointer
// ------------------------------------------------------------------
CFavSortDlg::CFavSortDlg(QWidget *parent, CFavItemModel *model) :
    QDialog(parent),
    ui(new Ui::CFavSortDlg)
{
    ui->setupUi(this);
    ui->listView->setModel(model);

    connect (model, SIGNAL(rowsInserted(QModelIndex, int, int)), this, SLOT(slotRowsInserted(QModelIndex,int,int)));
    connect (model, SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SLOT(slotRowsRemoved(QModelIndex,int,int)));
}

// ------------------------------------------------------------------
/// \brief destroy object
// ------------------------------------------------------------------
CFavSortDlg::~CFavSortDlg()
{
    delete ui;
}

// ------------------------------------------------------------------
/// \brief CFavSortDlg::slotRowsInserted
/// \param parent
/// \param start inserted index start
/// \param end inserted index end
// ------------------------------------------------------------------
void CFavSortDlg::slotRowsInserted(const QModelIndex &parent, int start, int end)
{
#ifdef __TRACE
    mInfo(tr("parent: %1, start: %2, end: %3").arg(parent.row()).arg(start).arg(end));
#endif
    mDroppedIdx = start;
}

// ------------------------------------------------------------------
/// \brief CFavSortDlg::slotRowsRemoved
/// \param parent
/// \param start removed index start
/// \param end removed index end
// ------------------------------------------------------------------
void CFavSortDlg::slotRowsRemoved(const QModelIndex &parent, int start, int end)
{
#ifdef __TRACE
    mInfo(tr("parent: %1, start: %2, end: %3").arg(parent.row()).arg(start).arg(end));
#endif
    mDragIdx = start;

    // time to make the right selection ...
    if (mDroppedIdx > mDragIdx)
    {
        // when moving down the channel, we have to reduce
        // the new row count by 1 since channel was removed
        // from above
        mDroppedIdx --;
    }

    QModelIndex midx = ui->listView->model()->index(mDroppedIdx, 0);
    ui->listView->setCurrentIndex(midx);
}
