/*------------------------------ Infor mation ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qstringfilterwidgetaction.h
 *
 *  @author   Jo2003
 *
 *  @date     29.07.2013
 *
 *  $Id$
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#ifndef __20130729_QSTRINGFILTERWIDGETACTION_H
   #define __20130729_QSTRINGFILTERWIDGETACTION_H

#include <QWidgetAction>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QEvent>

//---------------------------------------------------------------------------
//! \class   QStringFilterWidgetAction
//! \date    29.07.2013
//! \author  Jo2003
//! \brief   widget for channel filter
//---------------------------------------------------------------------------
class QStringFilterWidgetAction : public QWidgetAction
{
   Q_OBJECT

public:
   //---------------------------------------------------------------------------
   //
   //! \brief   constructs QStringFilterWidgetAction object
   //
   //! \author  Jo2003
   //! \date    29.07.2013
   //
   //! \param   parent (QObject *) pointer to parent object
   //
   //! \return  --
   //---------------------------------------------------------------------------
   QStringFilterWidgetAction(QObject *parent = 0)
      : QWidgetAction(parent), _lab(NULL), _line(NULL), _go(NULL)
   {

   }

private:
   QLabel      *_lab;
   QLineEdit   *_line;
   QPushButton *_go;
   QPushButton *_cancel;

protected:
   //---------------------------------------------------------------------------
   //
   //! \brief   create channel filter widget
   //
   //! \author  Jo2003
   //! \date    29.07.2013
   //
   //! \param   parent (QWidget *) pointer to parent widget
   //
   //! \return  --
   //---------------------------------------------------------------------------
   virtual QWidget* createWidget (QWidget * parent)
   {
      QWidget     *w = new QWidget(parent);
      QHBoxLayout *l = new QHBoxLayout();

      _lab           = new QLabel(tr("Filter Channels:"), w);
      _line          = new QLineEdit(w);
      _cancel        = new QPushButton(QIcon(":/app/del"), "", w);
      _go            = new QPushButton(QIcon(":/app/set"), "", w);

      _go->setIconSize(QSize(20, 20));
      _go->setFlat(true);
      _go->setDefault(true);
      _go->setToolTip(tr("set filter"));

      _cancel->setIconSize(QSize(20, 20));
      _cancel->setFlat(true);
      _cancel->setToolTip(tr("delete filter"));

      l->setSpacing(3);
      l->setMargin(1);

      l->addWidget(_lab);
      l->addWidget(_line, 10);
      l->addWidget(_cancel);
      l->addWidget(_go);

      w->setLayout(l);

      connect(_go    , SIGNAL(clicked())      , this, SLOT(slotGo()));
      connect(_cancel, SIGNAL(clicked())      , this, SLOT(slotCancel()));
      connect(_line  , SIGNAL(returnPressed()), this, SLOT(slotGo()));

      return w;
   }

   //---------------------------------------------------------------------------
   //
   //! \brief   handle language switch event
   //
   //! \author  Jo2003
   //! \date    29.07.2013
   //
   //! \param   e (QEvent *) pointer to event
   //
   //! \return  true -> handled; false -> not handled
   //---------------------------------------------------------------------------
   virtual bool event(QEvent *e)
   {
      bool rv = false;

      if (e->type() == QEvent::LanguageChange)
      {
         // make sure we already created
         // control elements ...
         if (_lab && _go && _cancel)
         {
            _lab->setText(tr("Filter Channels:"));
            _go->setToolTip(tr("set filter"));
            _cancel->setToolTip(tr("delete filter"));

            rv = true;
         }
      }
      else
      {
         rv = QWidgetAction::event(e);
      }

      return rv;
   }

private slots:
   //---------------------------------------------------------------------------
   //
   //! \brief   go button was pressed, trigger sigFilter
   //
   //! \author  Jo2003
   //! \date    29.07.2013
   //
   //! \param   --
   //
   //! \return  --
   //---------------------------------------------------------------------------
   void slotGo()
   {
      // slot is reached only if we've created
      // control elements already ...
      emit sigFilter(_line->text());
   }

   //---------------------------------------------------------------------------
   //
   //! \brief   cancel button was pressed, clear filter, trigger sigFilter
   //
   //! \author  Jo2003
   //! \date    29.07.2013
   //
   //! \param   --
   //
   //! \return  --
   //---------------------------------------------------------------------------
   void slotCancel()
   {
      _line->setText("");
      emit sigFilter(_line->text());
   }

signals:
   //---------------------------------------------------------------------------
   //
   //! \brief   signal filter string
   //
   //! \author  Jo2003
   //! \date    29.07.2013
   //
   //! \param   QString filter string
   //
   //! \return  --
   //---------------------------------------------------------------------------
   void sigFilter(QString);
};

#endif // __20130729_QSTRINGFILTERWIDGETACTION_H
