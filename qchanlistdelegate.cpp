/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 22.03.2011 / 12:20
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#include "qchanlistdelegate.h"
#include "small_helpers.h"

/* -----------------------------------------------------------------\
|  Method: QChanListDelegate / constructor
|  Begin:  22.03.2011 / 12:20
|  Author: Jo2003
|  Description: construct QChanListDelegate object
|
|  Parameters: pointer to parent object
|
|  Returns: --
\----------------------------------------------------------------- */
QChanListDelegate::QChanListDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
   // nothing to do so far ...
}

/* -----------------------------------------------------------------\
|  Method: ~QChanListDelegate / destructor
|  Begin:  22.03.2011 / 12:20
|  Author: Jo2003
|  Description: destroy QChanListDelegate object
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
QChanListDelegate::~QChanListDelegate()
{
   // nothing to do so far ...
}

/*
 +----------------------------------------------------+
 | +------+  Channel Name                             |
 | | Icon |  Program                                  |
 | |      |  From ... to                              |
 | +------+ |====================-------------------| |
 +----------------------------------------------------+
*/

/* -----------------------------------------------------------------\
|  Method: sizeHint
|  Begin:  22.03.2011 / 12:20
|  Author: Jo2003
|  Description: get size hint for custom list item
|
|  Parameters: view item, model index
|
|  Returns: size
\----------------------------------------------------------------- */
QSize QChanListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
   int     iHeight, iWidth = 0, iTextHeight, iTextWidth[3];
   uint    uiStart  = qvariant_cast<uint>(index.data(channellist::startRole));
   uint    uiEnd    = qvariant_cast<uint>(index.data(channellist::endRole));
   QIcon   icon     = qvariant_cast<QIcon>(index.data(channellist::iconRole));
   QSize   iconsize = icon.actualSize(option.decorationSize);
   QString sChan    = qvariant_cast<QString>(index.data(channellist::nameRole));
   QString sProg    = qvariant_cast<QString>(index.data(channellist::progRole));
   QString fromTo   = QString("%1 - %2")
                      .arg(QDateTime::fromTime_t(uiStart).toString("h:mm"))
                      .arg(QDateTime::fromTime_t(uiEnd).toString("h:mm"));

   iHeight = iconsize.height() + 4; // space on top and bottom ...

   QFont chanFont = option.font;
   chanFont.setPointSize(chanFont.pointSize() + 1);
   chanFont.setBold(true);

   QFontMetrics fmChan(chanFont);

   // check if additional info is available ...
   if ((sProg == "") || !uiStart || !uiEnd)
   {
      // no additional info --> simple item only ...
      iWidth       = fmChan.size(Qt::TextSingleLine, sChan).width();

      // take care of text height ...
      iTextHeight  = fmChan.height();

      if (iTextHeight > iHeight)
      {
         iHeight = iTextHeight;
      }
   }
   else
   {
      QFont progFont = option.font;
      progFont.setPointSize(progFont.pointSize() - 1);
      progFont.setWeight(progFont.weight() - 2);

      QFontMetrics fmProg(progFont);

      // "channel name" + "program" + "from ... to" ...
      iTextHeight  = fmChan.height() + fmProg.height() + fmProg.height();

      // add progress bar height ...
      iTextHeight += 10;

      if (iTextHeight > iHeight)
      {
         iHeight = iTextHeight;
      }

      // width ...
      CSmallHelpers::cutProgString(sProg, fmProg, option.rect.width() - (2 + iconsize.width() + 4 + 2));
      CSmallHelpers::cutProgString(sChan, fmChan, option.rect.width() - (2 + iconsize.width() + 4 + 2));
      iTextWidth[0] = fmChan.size(Qt::TextSingleLine, sChan).width();
      iTextWidth[1] = fmProg.size(Qt::TextSingleLine, sProg).width();
      iTextWidth[2] = fmProg.size(Qt::TextSingleLine, fromTo).width();

      for (int i = 0; i < 3; i++)
      {
         if (iTextWidth[i] > iWidth)
         {
            iWidth = iTextWidth[i];
         }
      }
   }

   iWidth += iconsize.width();
   iWidth += 4; // space between icon and text ...

   return QSize(iWidth, iHeight);
}

/* -----------------------------------------------------------------\
|  Method: paint
|  Begin:  22.03.2011 / 12:20
|  Author: Jo2003
|  Description: paint custom view item
|
|  Parameters: painter, view item, model index
|
|  Returns: --
\----------------------------------------------------------------- */
void QChanListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
   // paint any selection / hover effect ...
   QStyledItemDelegate::paint(painter, option, index);
   painter->save();

   /////////////////////////////////////////////////////////////////////////////
   int     iPos     = qvariant_cast<int>(index.data(channellist::posRole));
   int     iCid     = qvariant_cast<int>(index.data(channellist::cidRole));
   uint    uiStart  = qvariant_cast<uint>(index.data(channellist::startRole));
   uint    uiEnd    = qvariant_cast<uint>(index.data(channellist::endRole));
   QIcon   icon     = qvariant_cast<QIcon>(index.data(channellist::iconRole));
   QSize   iconsize = icon.actualSize(option.decorationSize);
   QString sChan    = qvariant_cast<QString>(index.data(channellist::nameRole));
   QString sProg    = qvariant_cast<QString>(index.data(channellist::progRole));
   QString sColor   = qvariant_cast<QString>(index.data(channellist::bgcolorRole));
   QString fromTo   = QString("%1 - %2")
                      .arg(QDateTime::fromTime_t(uiStart).toString("h:mm"))
                      .arg(QDateTime::fromTime_t(uiEnd).toString("h:mm"));
   /////////////////////////////////////////////////////////////////////////////

   // colorize background if this is a channel group ...
   if ((iCid == -1) && (sColor != "")
      && !(option.state & (QStyle::State_Selected | QStyle::State_MouseOver)))
   {
      // channel group ...
      painter->fillRect(option.rect, QColor(sColor));
   }

   // draw icon ...
   int   x = option.rect.x() + 2;
   int   y = option.rect.y() + ((option.rect.height() - iconsize.height()) / 2);
   QRect iconRect(x, y, iconsize.width(), iconsize.height());
   painter->drawPixmap(iconRect, icon.pixmap(iconsize));

   x = iconRect.right() + 4;
   int rightWidth = option.rect.right() - x - 2;

   // font stuff for channel line ...
   QFont chanFont = option.font;
   chanFont.setPointSize(chanFont.pointSize() + 1);
   chanFont.setBold(true);
   QFontMetrics fmChan(chanFont);

   // check if additional info is available ...
   if ((sProg == "") || !uiStart || !uiEnd)
   {
      // draw simple item only ...

      // channel line ...
      y = option.rect.y() + ((option.rect.height() - fmChan.height()) / 2);
      QRect chanRect(x, y, rightWidth, fmChan.height());

      painter->setFont(chanFont);
      painter->drawText(chanRect, Qt::TextSingleLine, sChan);
   }
   else
   {
      // draw advanced item ...
      QRect bounting;
      QFont progFont = option.font;
      progFont.setPointSize(progFont.pointSize() - 1);
      progFont.setWeight(progFont.weight() - 2);
      QFontMetrics fmProg(progFont);

      // channel line ...
      y = option.rect.y() + 2;
      QRect chanRect(x, y, rightWidth, fmChan.height());

      CSmallHelpers::cutProgString(sChan, fmChan, rightWidth);
      painter->setFont(chanFont);
      painter->drawText(chanRect, Qt::TextSingleLine, sChan, &bounting);

      // program line ...
      y = bounting.bottom();
      QRect progRect(x, y, rightWidth, fmProg.height());

      CSmallHelpers::cutProgString(sProg, fmProg, rightWidth);
      painter->setFont(progFont);
      painter->drawText(progRect, Qt::TextSingleLine, sProg.section('\n', 0, 0), &bounting);

      // from ... to line ...
      y = bounting.bottom();
      QRect timesRect(x, y, rightWidth, fmProg.height());
      painter->drawText(timesRect, Qt::TextSingleLine, fromTo, &bounting);

      // progress bar ...
      y = bounting.bottom() + 2;
      QRect progressRect(x, y, rightWidth, 5);
      iPos = (iPos > (int)(uiEnd - uiStart)) ? (int)(uiEnd - uiStart) : iPos;

#ifndef Q_OS_MAC
      QStyleOptionProgressBar progressBar;
      progressBar.rect        = progressRect;
      progressBar.minimum     = 0;
      progressBar.maximum     = (int)(uiEnd - uiStart);
      progressBar.progress    = iPos;
      progressBar.textVisible = false;

      QApplication::style()->drawControl(QStyle::CE_ProgressBar,
                                         &progressBar, painter);
#else
      // Drawing the progress bar on mac doesn't look nice
      // because the height can't be shrinked as needed.
      // So we try to draw our own progress bar here ...

      // make sure background is white ...
      painter->fillRect(progressRect, QColor("white"));

      // prepare pen for border line ...
      QPen pen(Qt::SolidLine);
      pen.setColor(QColor("silver"));
      pen.setWidth(1);
      painter->setPen(pen);

      // draw outline ...
      painter->drawRect(progressRect);

      // compute size of inner rect (take care of border) ...
      x = progressRect.width()  - pen.width();
      y = progressRect.height() - pen.width();
      x = (uiEnd - uiStart) ? ((x * iPos) / (int)(uiEnd - uiStart)) : 0; ///< avoid division with 0 !!!

      // inner rectangle (progess) ...
      QRect macRect(progressRect.x() + pen.width(), progressRect.y() + pen.width(), x, y);

      // paint progress ...
      painter->fillRect(macRect, QColor("#036"));
#endif // Q_OS_MAC

   }

   painter->restore();
}
