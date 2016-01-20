/*********************** Information *************************\
| $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/qchanlistdelegate.h $
|
| Author: Jo2003
|
| Begin: 22.03.2011 / 12:20
|
| Last edited by: $Author: Olenka.Joerg $
|
| $Id: qchanlistdelegate.h 1044 2013-03-20 14:43:26Z Olenka.Joerg $
\*************************************************************/
#ifndef __032211_QCHANLISTDELEGATE_H
   #define __032211_QCHANLISTDELEGATE_H

#include <QStyledItemDelegate>
#include <QApplication>
#include <QStyleOptionProgressBar>
#include <QPainter>
#include <QDateTime>
#include <QPen>

namespace channellist
{
   enum eRoles
   {
      iconRole     = Qt::UserRole + 101,
      nameRole     = Qt::UserRole + 102,
      progRole     = Qt::UserRole + 103,
      cidRole      = Qt::UserRole + 104,
      startRole    = Qt::UserRole + 105,
      endRole      = Qt::UserRole + 106,
      bgcolorRole  = Qt::UserRole + 107,
      posRole      = Qt::UserRole + 108,
      lastEpgUpd   = Qt::UserRole + 109,
      logoFileRole = Qt::UserRole + 110,
      gidRole      = Qt::UserRole + 111,
      noRole
   };
}

/********************************************************************\
|  Class: QChanListDelegate
|  Date:  22.03.2011 / 12:20
|  Author: Jo2003
|  Description: delegate to render custom channel list item
|
\********************************************************************/
class QChanListDelegate : public QStyledItemDelegate
{
   Q_OBJECT

public:
   QChanListDelegate(QObject *parent = 0);
   virtual ~QChanListDelegate();
   virtual void	paint (QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
   virtual QSize	sizeHint (const QStyleOptionViewItem & option, const QModelIndex & index) const;

protected:
};

#endif // __032211_QCHANLISTDELEGATE_H
