/*********************** Information *************************\
| $HeadURL$
|
| Author: Jo2003
|
| Begin: 17.03.2011 / 9:50
|
| Last edited by: $Author$
|
| $Id$
\*************************************************************/
#include "qchandetails.h"
#include "ui_qchandetails.h"

/* -----------------------------------------------------------------\
|  Method: QChanDetails / constructor
|  Begin:  17.03.2011 / 9:50
|  Author: Jo2003
|  Description: construct QChanDetails object
|
|  Parameters: pointer to parent widget
|
|  Returns: --
\----------------------------------------------------------------- */
QChanDetails::QChanDetails(QWidget *parent) :
      QWidget(parent), ui(new Ui::QChanDetails)
{
    ui->setupUi(this);
}

/* -----------------------------------------------------------------\
|  Method: ~QChanDetails / destructor
|  Begin:  17.03.2011 / 9:50
|  Author: Jo2003
|  Description: cleanly destroy QChanDetails object
|
|  Parameters: --
|
|  Returns: --
\----------------------------------------------------------------- */
QChanDetails::~QChanDetails()
{
    delete ui;
}

/* -----------------------------------------------------------------\
|  Method: setChannelName
|  Begin:  17.03.2011 / 9:50
|  Author: Jo2003
|  Description: set and store channel name
|
|  Parameters: channel name
|
|  Returns: --
\----------------------------------------------------------------- */
void QChanDetails::setChannelName(const QString &name)
{
   ui->lChan->setText(name);
}

/* -----------------------------------------------------------------\
|  Method: setProgram
|  Begin:  17.03.2011 / 9:50
|  Author: Jo2003
|  Description: set and store show name
|
|  Parameters: show (program) name
|
|  Returns: --
\----------------------------------------------------------------- */
void QChanDetails::setProgram(const QString &prog)
{
   ui->lProgram->setText(prog.section('\n', 0, 0));
}

/* -----------------------------------------------------------------\
|  Method: setTimes
|  Begin:  17.03.2011 / 9:50
|  Author: Jo2003
|  Description: set start and end time of running show,
|               set progress
|
|  Parameters: start time, end time
|
|  Returns: --
\----------------------------------------------------------------- */
void QChanDetails::setTimes(uint start, uint end)
{
   if (start && end)
   {
      // set from ... to label ...
      QString fromTo = QString("%1 - %2")
                       .arg(QDateTime::fromTime_t(start).toString("hh:mm"))
                       .arg(QDateTime::fromTime_t(end).toString("hh:mm"));

      ui->lFromTo->setText(fromTo);

      // set progess bar ...
      uint now = QDateTime::currentDateTime().toTime_t();
      ui->timeProgess->setRange(0, (int)(end - start));
      ui->timeProgess->setValue((int)(now - start));
   }
   else
   {
      ui->lFromTo->hide();
      ui->timeProgess->hide();
   }
}

/* -----------------------------------------------------------------\
|  Method: setIcon
|  Begin:  17.03.2011 / 10:00
|  Author: Jo2003
|  Description: set icon
|
|  Parameters: ref. to icon
|
|  Returns: --
\----------------------------------------------------------------- */
void QChanDetails::setIcon (const QIcon &icon)
{
   ui->lIcon->setPixmap(icon.pixmap(24, 24));
}
