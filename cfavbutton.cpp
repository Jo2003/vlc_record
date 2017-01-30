/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     cfavbutton.cpp
 *
 *  @author   Jo2003
 *
 *  @date     30.01.2017
 *
 *  $Id$
 *
 *///------------------------- (c) 2017 by Jo2003  --------------------------
#include "cfavbutton.h"
#include <QAction>
#include <QMenu>

#include "clogfile.h"

// for logging ...
extern CLogFile VlcLog;

//-------------------------------------------------------------------
/// \brief construct object
/// \param icon   button icon
/// \param text   button text
/// \param parent parent widget
/// \param id     channel id
//-------------------------------------------------------------------
CFavButton::CFavButton(const QIcon &icon, const QString &text, QWidget *parent, int id)
    : QPushButton(icon, QString(), parent), mId(id), mName(text)
{
    QAction *pAct;
    mpContext = new QMenu(this);
    pAct = mpContext->addAction(QIcon(":/app/sort"), tr("Sort favourites"));
    connect(pAct, SIGNAL(triggered()), this, SLOT(slotSort()));

    pAct = mpContext->addAction(icon, tr("Remove \"%1\" from favourites").arg(mName));
    connect(pAct, SIGNAL(triggered()), this, SLOT(slotRemove()));

    connect(this, SIGNAL(clicked()), this, SLOT(slotClicked()));

    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
}

//-------------------------------------------------------------------
/// \brief destroy object
//-------------------------------------------------------------------
CFavButton::~CFavButton()
{
#ifdef __TRACE
    mInfo(tr("Delete CFavButton button %1 ...").arg(mName));
#endif
    if (mpContext)
    {
        delete mpContext;
    }
}

//-------------------------------------------------------------------
/// \brief get channel id
/// \return channel id
//-------------------------------------------------------------------
int CFavButton::cid() const
{
    return mId;
}

//-------------------------------------------------------------------
/// \brief get channel name
/// \return channel name
//-------------------------------------------------------------------
QString CFavButton::name() const
{
    return mName;
}

//-------------------------------------------------------------------
/// \brief show context menu
/// \param pos where to show menu
//-------------------------------------------------------------------
void CFavButton::showContextMenu(const QPoint &pos)
{
    mpContext->exec(mapToGlobal(pos));
}

//-------------------------------------------------------------------
/// \brief button was clicked, send with id
//-------------------------------------------------------------------
void CFavButton::slotClicked()
{
    emit clicked(mId);
}

//-------------------------------------------------------------------
/// \brief remove action from button context
//-------------------------------------------------------------------
void CFavButton::slotRemove()
{
    emit deleteMe(mId);
}

//-------------------------------------------------------------------
/// \brief sort action from button context
//-------------------------------------------------------------------
void CFavButton::slotSort()
{
    emit reorder();
}


