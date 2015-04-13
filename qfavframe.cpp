/*------------------------------ Information ---------------------------*//**
 *
 *  $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/qfavframe.cpp $
 *
 *  @file     qfavframe.cpp
 *
 *  @author   Jo2003
 *
 *  @date     14.06.2012
 *
 *  $Id: qfavframe.cpp 759 2012-06-14 13:36:38Z Olenka.Joerg $
 *
 *///------------------------- (c) 2012 by Jo2003  --------------------------
#include "qfavframe.h"

// log file functions ...
extern CLogFile VlcLog;

//---------------------------------------------------------------------------
//! \fn      sigAddFav
//! \brief   signal will be emitted if we have to add a favourite
//
//! \author  Jo2003
//! \date    14.06.2012
//
//! \param   f favourit to add
//
//! \return  --
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//
//! \brief   constructs QFavFrame object
//
//! \author  Jo2003
//! \date    14.06.2012
//
//! \param   parent pointer to parent widget
//! \param   f window flags
//
//! \return  --
//---------------------------------------------------------------------------
QFavFrame::QFavFrame(QWidget *parent, Qt::WindowFlags f)
   : QFrame(parent, f)
{
   // nothing special to do so far ...
}

//---------------------------------------------------------------------------
//
//! \brief   destroys QFavFrame object
//
//! \author  Jo2003
//! \date    14.06.2012
//
//! \return  --
//---------------------------------------------------------------------------
QFavFrame::~QFavFrame()
{
   // nothing to do so far ...
}

//---------------------------------------------------------------------------
//
//! \brief   drag enters widget
//
//! \author  Jo2003
//! \date    14.06.2012
//
//! \param   event pointer to QDragEnterEvent
//
//! \return  --
//---------------------------------------------------------------------------
void QFavFrame::dragEnterEvent(QDragEnterEvent *event)
{
   // we accept only values from our channel list (list view) ...
   if ((event->mimeData()->hasFormat("application/x-qstandarditemmodeldatalist"))
      && (getCidFromMime(event->mimeData()) > -1))
   {
      event->acceptProposedAction();
   }
}

//---------------------------------------------------------------------------
//
//! \brief   drop on widget
//
//! \author  Jo2003
//! \date    14.06.2012
//
//! \param   event pointer to QDropEvent
//
//! \return  --
//---------------------------------------------------------------------------
void QFavFrame::dropEvent(QDropEvent *event)
{
   int cid;
   if ((cid = getCidFromMime(event->mimeData())) > -1)
   {
      event->acceptProposedAction();

      // tell about ...
      emit sigAddFav(cid);
   }
}

//---------------------------------------------------------------------------
//
//! \brief   decode mime data to get cid
//
//! \author  Jo2003
//! \date    14.06.2012
//
//! \param   pData pointer to encoded mime data
//
//! \return  --
//---------------------------------------------------------------------------
int QFavFrame::getCidFromMime(const QMimeData *pData)
{
   int row, col, cid = -2;
   QModelItemMap dataMap;
   QByteArray    encData = pData->data("application/x-qabstractitemmodeldatalist");
   QDataStream   dataStream(&encData, QIODevice::ReadOnly);

   // there should only be one entry at all ...
   while (!dataStream.atEnd())
   {
      dataStream >> row >> col >> dataMap;

      if (dataMap.contains(channellist::cidRole))
      {
         cid = qvariant_cast<int>(dataMap[channellist::cidRole]);
      }
   }

   return cid;
}
