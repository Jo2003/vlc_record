/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qsigscrollarea.h
 *
 *  @author   Jo2003
 *
 *  @date     22.02.2017
 *
 *  $Id$
 *
 *///------------------------- (c) 2017 by Jo2003  --------------------------
#ifndef __20170222_QSIGSCROLLAREA_H
    #define __20170222_QSIGSCROLLAREA_H

#include <QScrollArea>
#include <QResizeEvent>

//-----------------------------------------------------
/// \brief a scroll area class with resize signalling
//-----------------------------------------------------
class QSigScrollArea : public QScrollArea
{
    Q_OBJECT

public:
    //-----------------------------------------------------
    /// \brief construct object
    /// \param parent (QWidget*) parent widget pointer
    //-----------------------------------------------------
    QSigScrollArea(QWidget * parent = 0) : QScrollArea(parent)
    {
    }

signals:
    //-----------------------------------------------------
    /// \brief is emmited when resized
    /// \param oldSz (QSize) old size
    /// \param sz (QSize) new size
    //-----------------------------------------------------
    void resized(QSize oldSz, QSize sz);

protected:
    //-----------------------------------------------------
    /// \brief resizeEvent
    /// \param e (QResizeEvent*) event pointer
    //-----------------------------------------------------
    virtual void resizeEvent(QResizeEvent *e)
    {
        emit resized(e->oldSize(), e->size());
        QScrollArea::resizeEvent(e);
    }
};

#endif // __20170222_QSIGSCROLLAREA_H

