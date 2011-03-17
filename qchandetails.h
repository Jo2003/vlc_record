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
#ifndef __031711_QCHANDETAILS_H
   #define __031711_QCHANDETAILS_H

#include <QWidget>
#include <QDateTime>
#include <QIcon>

#include "defdef.h"

namespace Ui {
    class QChanDetails;
}

/********************************************************************\
|  Class: QChanDetails
|  Date:  17.03.2011 / 9:50
|  Author: Jo2003
|  Description: special widget to insert into channel list
|
\********************************************************************/
class QChanDetails : public QWidget
{
    Q_OBJECT

public:
    QChanDetails(QWidget *parent = 0);
    ~QChanDetails();
    void setIcon(const QIcon &icon);
    void setChannelName(const QString &name);
    void setProgram(const QString &prog);
    void setTimes(uint start, uint end);

private:
    Ui::QChanDetails *ui;
};

#endif // __031711_QCHANDETAILS_H
