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
#ifndef __20170130_CFAVSORTDLG_H
    #define __20170130_CFAVSORTDLG_H

#include <QStandardItemModel>
#include <QDialog>

namespace Ui {
    class CFavSortDlg;
}

///
/// \brief The CFavItemModel class
///
class CFavItemModel : public QStandardItemModel
{
public:
    explicit CFavItemModel(QObject *parent = 0) : QStandardItemModel(parent)
    {
    }

    virtual Qt::ItemFlags flags(const QModelIndex &index) const
    {
        // Qt::ItemFlags flags; //= QStringListModel::flags(index);

        if (index.isValid())
            return Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsEnabled;
        else
            return Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled  | Qt::ItemIsEnabled;

        // return flags;
    }
};

///
/// \brief The CFavSortDlg class
///
class CFavSortDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CFavSortDlg(QWidget *parent, CFavItemModel* model);
    ~CFavSortDlg();

private slots:
    void slotRowsInserted(const QModelIndex& parent, int start, int end);
    void slotRowsRemoved(const QModelIndex& parent, int start, int end);

private:
    Ui::CFavSortDlg   *ui;
    int                mDroppedIdx;
    int                mDragIdx;
};

#endif // __20170130_CFAVSORTDLG_H
