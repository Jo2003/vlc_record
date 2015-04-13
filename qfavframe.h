/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/qfavframe.h $
 *
 *  @file     qfavframe.h
 *
 *  @author   Jo2003
 *
 *  @date     14.06.2012
 *
 *  $Id: qfavframe.h 759 2012-06-14 13:36:38Z Olenka.Joerg $
 *
 *///------------------------- (c) 2012 by Jo2003  --------------------------
#ifndef __20120614_QFAVFRAME_H
   #define __20120614_QFAVFRAME_H

#include <QFrame>
#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QDataStream>
#include <QMap>

#include "clogfile.h"
#include "qchanlistdelegate.h"

typedef QMap<int, QVariant> QModelItemMap;

//---------------------------------------------------------------------------
//! \class   QFavFrame
//! \date    14.06.2012
//! \author  Jo2003
//! \brief   a drop enabled QFrame class for favourite handling
//---------------------------------------------------------------------------
class QFavFrame : public QFrame
{
   Q_OBJECT

public:
   QFavFrame(QWidget *parent = NULL, Qt::WindowFlags f = 0);
   virtual ~QFavFrame();

protected:
   virtual void dragEnterEvent (QDragEnterEvent * event);
   virtual void dropEvent (QDropEvent *event);
   int getCidFromMime (const QMimeData *pData);

signals:
   void sigAddFav(int f);

public slots:

};

#endif // __20120614_QFAVFRAME_H
