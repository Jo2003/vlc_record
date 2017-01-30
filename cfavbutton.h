/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     cfavbutton.h
 *
 *  @author   Jo2003
 *
 *  @date     30.01.2017
 *
 *  $Id$
 *
 *///------------------------- (c) 2017 by Jo2003  --------------------------
#ifndef __20170130_CFAVBUTTON_H
    #define __20170130_CFAVBUTTON_H

#include <QPushButton>
#include <QString>

///
/// \brief The CFavButton class
///
class CFavButton : public QPushButton
{
    Q_OBJECT

public:
    explicit CFavButton(const QIcon& icon, const QString& text, QWidget* parent, int id);
    virtual ~CFavButton();
    int cid() const;
    QString name() const;

signals:
    void clicked(int);
    void deleteMe(int);
    void reorder();

private slots:
    void showContextMenu(const QPoint & pos);

public slots:
    void slotClicked();
    void slotRemove();
    void slotSort();

private:
    int     mId;
    QString mName;
    QMenu*  mpContext;
};

#endif // __20170130_CFAVBUTTON_H
