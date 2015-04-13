/*********************** Information *************************\
| $HeadURL: https://vlc-record.googlecode.com/svn/branches/rodnoe.tv/cshortcutgrabber.h $
|
| Author: Jo2003
|
| Begin: 11.02.2011 / 15:00
|
| Last edited by: $Author: Olenka.Joerg $
|
| $Id: cshortcutgrabber.h 496 2011-04-06 09:33:25Z Olenka.Joerg $
\*************************************************************/
#ifndef __11022001_CSHORTCUTGRABBER_H
   #define __11022001_CSHORTCUTGRABBER_H

#include <QLineEdit>
#include <QKeyEvent>
#include <QKeySequence>
#include <QHBoxLayout>
#include <QMenu>

/********************************************************************\
|  Class: CShortCutGrabber
|  Date:  11.02.2011 / 15:00
|  Author: Jo2003 / mostly taken from QDesigner
|  Description: a widget to grab key sequences
|
\********************************************************************/
class CShortCutGrabber : public QWidget
{
   Q_OBJECT

public:
   CShortCutGrabber(QWidget *parent = 0, int row = -1);

   QKeySequence keySequence() const;
   QString shortCutString() const;
   bool eventFilter(QObject *o, QEvent *e);
   void setTarget(const QString &str);
   void setSlot(const QString &str);
   void markRed ();
   void unMark ();
   QString slot() const;
   QString target() const;

public slots:
   void setKeySequence(const QKeySequence &costSeq, const QKeySequence &orgSeq = QKeySequence());
   void revert();

signals:
   void keySequenceChanged(const QKeySequence &sequence, int row);

protected:
   void focusInEvent(QFocusEvent *e);
   void focusOutEvent(QFocusEvent *e);
   void keyPressEvent(QKeyEvent *e);
   void keyReleaseEvent(QKeyEvent *e);
   bool event(QEvent *e);

private slots:
   void slotClearShortcut();

private:
   void handleKeyEvent(QKeyEvent *e);
   int translateModifiers(Qt::KeyboardModifiers state, const QString &text) const;

   int          m_num;
   int          iRow;
   QKeySequence m_keySequence;
   QKeySequence m_orgkeySequence;
   QLineEdit   *m_lineEdit;
   QString      sTarget;
   QString      sSlot;
   bool         bMarked;
};

#endif // __11022001_CSHORTCUTGRABBER_H
