/*------------------------------ Infor mation ---------------------------*//**
 *
 *  $HeadURL$
 *
 *  @file     qlangfilterwidgetaction.h
 *
 *  @author   Jo2003
 *
 *  @date     29.07.2013
 *
 *  $Id$
 *
 *///------------------------- (c) 2013 by Jo2003  --------------------------
#ifndef __20130729_QLANGFILTERWIDGETACTION_H
   #define __20130729_QLANGFILTERWIDGETACTION_H

#include <QWidgetAction>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QEvent>

//---------------------------------------------------------------------------
//! \class   QLangFilterWidgetAction
//! \date    29.07.2013
//! \author  Jo2003
//! \brief   widget for language channel filter
//---------------------------------------------------------------------------
class QLangFilterWidgetAction : public QWidgetAction
{
   Q_OBJECT

public:
   //---------------------------------------------------------------------------
   //
   //! \brief   constructs QLangFilterWidgetAction object
   //
   //! \author  Jo2003
   //! \date    29.07.2013
   //
   //! \param   parent (QObject *) pointer to parent object
   //
   //! \return  --
   //---------------------------------------------------------------------------
   QLangFilterWidgetAction(QObject *parent = 0)
      : QWidgetAction(parent), _lab(NULL), _cbx(NULL), _go(NULL), _cancel(NULL)
   {

   }

   //---------------------------------------------------------------------------
   //
   //! \brief   fill language combo box
   //
   //! \author  Jo2003
   //! \date    29.07.2013
   //
   //! \param   sl (const QStringList&) list with language codes
   //
   //! \return  --
   //---------------------------------------------------------------------------
   void fillLangCbx(QStringList sl)
   {
      int i;

      if(_cbx)
      {
         _cbx->clear();
         _cbx->addItem(tr("All"));

         for (i = 0; i < sl.count(); i++)
         {
            _cbx->addItem(sl.at(i));
         }
      }
   }

   //---------------------------------------------------------------------------
   //
   //! \brief   is language box filles already?
   //
   //! \author  Jo2003
   //! \date    29.07.2013
   //
   //! \param   --
   //
   //! \return  true -> filled; false -> not yet filled
   //---------------------------------------------------------------------------
   bool langBoxFilled()
   {
      return (_cbx->count() > 1) ? true : false;
   }

private:
   QLabel      *_lab;
   QComboBox   *_cbx;
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

      _lab           = new QLabel(tr("Language Filter:"), w);
      _cbx           = new QComboBox(w);
      _cancel        = new QPushButton(QIcon(":/app/del"), "", w);
      _go            = new QPushButton(QIcon(":/app/set"), "", w);

      _go->setIconSize(QSize(20, 20));
      _go->setFlat(true);
      _go->setDefault(true);
      _go->setToolTip(tr("set filter"));

      _cancel->setIconSize(QSize(20, 20));
      _cancel->setFlat(true);
      _cancel->setToolTip(tr("delete filter"));

      _cbx->addItem(tr("All"));

      l->setSpacing(3);
      l->setMargin(1);

      l->addWidget(_lab);
      l->addWidget(_cbx, 10);
      l->addWidget(_cancel);
      l->addWidget(_go);

      w->setLayout(l);

      connect(_go    , SIGNAL(clicked()), this, SLOT(slotGo()));
      connect(_cancel, SIGNAL(clicked()), this, SLOT(slotCancel()));

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
         if (_lab && _go && _cancel && _cbx)
         {
            _lab->setText(tr("Language Filter:"));
            _go->setToolTip(tr("set filter"));
            _cancel->setToolTip(tr("delete filter"));
            _cbx->setItemText(0, tr("All"));

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
      if (_cbx->currentIndex() <= 0)
      {
         emit sigFilter(QString());
      }
      else
      {
         emit sigFilter(_cbx->currentText());
      }
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
      _cbx->setCurrentIndex(0);
      emit sigFilter(QString());
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

#endif // __20130729_QLANGFILTERWIDGETACTION_H
