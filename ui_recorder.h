/********************************************************************************
** Form generated from reading ui file 'recorder.ui'
**
** Created: Tue 19. Jan 16:21:19 2010
**      by: Qt User Interface Compiler version 4.5.3
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef UI_RECORDER_H
#define UI_RECORDER_H

#include <QtCore/QLocale>
#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QFrame>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include "cepgbrowser.h"

QT_BEGIN_NAMESPACE

class Ui_Recorder
{
public:
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *vLayoutChannels;
    QGroupBox *groupChannels;
    QVBoxLayout *verticalLayout;
    QComboBox *cbxChannelGroup;
    QListWidget *listWidget;
    QGroupBox *groupEpgShort;
    QVBoxLayout *verticalLayout_3;
    CEpgBrowser *textEpgShort;
    QProgressBar *progressBar;
    QGroupBox *groupEPG;
    QVBoxLayout *verticalLayout_2;
    QFrame *frameEpgCtrl;
    QHBoxLayout *horizontalLayout_5;
    QFrame *frameChan;
    QHBoxLayout *horizontalLayout_3;
    QLabel *labChanIcon;
    QLabel *labChanName;
    QSpacerItem *horizontalSpacer_2;
    QFrame *frameNavi;
    QHBoxLayout *horizontalLayout_4;
    QToolButton *btnBack;
    QLabel *labCurrDay;
    QToolButton *btnNext;
    CEpgBrowser *textEpg;
    QFrame *frame;
    QHBoxLayout *horizontalLayout_2;
    QComboBox *cbxTimeShift;
    QFrame *line;
    QPushButton *pushPlay;
    QPushButton *pushRecord;
    QFrame *line_2;
    QSpacerItem *horizontalSpacer;
    QFrame *line_3;
    QPushButton *pushSettings;
    QPushButton *pushAbout;
    QPushButton *pushQuit;

    void setupUi(QDialog *Recorder)
    {
        if (Recorder->objectName().isEmpty())
            Recorder->setObjectName(QString::fromUtf8("Recorder"));
        Recorder->resize(700, 436);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/app/tv"), QSize(), QIcon::Normal, QIcon::Off);
        Recorder->setWindowIcon(icon);
        Recorder->setStyleSheet(QString::fromUtf8("QDialog#Recorder {\n"
"background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 #eee, 	stop:0.7 grey, stop:1 #dedede);\n"
"}"));
        Recorder->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        verticalLayout_4 = new QVBoxLayout(Recorder);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setMargin(11);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        vLayoutChannels = new QVBoxLayout();
        vLayoutChannels->setSpacing(6);
        vLayoutChannels->setObjectName(QString::fromUtf8("vLayoutChannels"));
        groupChannels = new QGroupBox(Recorder);
        groupChannels->setObjectName(QString::fromUtf8("groupChannels"));
        verticalLayout = new QVBoxLayout(groupChannels);
        verticalLayout->setSpacing(6);
        verticalLayout->setMargin(11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        cbxChannelGroup = new QComboBox(groupChannels);
        cbxChannelGroup->setObjectName(QString::fromUtf8("cbxChannelGroup"));

        verticalLayout->addWidget(cbxChannelGroup);

        listWidget = new QListWidget(groupChannels);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        listWidget->setIconSize(QSize(24, 24));

        verticalLayout->addWidget(listWidget);

        verticalLayout->setStretch(1, 60);

        vLayoutChannels->addWidget(groupChannels);

        groupEpgShort = new QGroupBox(Recorder);
        groupEpgShort->setObjectName(QString::fromUtf8("groupEpgShort"));
        verticalLayout_3 = new QVBoxLayout(groupEpgShort);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setMargin(11);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        textEpgShort = new CEpgBrowser(groupEpgShort);
        textEpgShort->setObjectName(QString::fromUtf8("textEpgShort"));

        verticalLayout_3->addWidget(textEpgShort);

        progressBar = new QProgressBar(groupEpgShort);
        progressBar->setObjectName(QString::fromUtf8("progressBar"));
        progressBar->setMinimumSize(QSize(0, 10));
        progressBar->setMaximumSize(QSize(16777215, 10));
        progressBar->setValue(0);
        progressBar->setTextVisible(false);

        verticalLayout_3->addWidget(progressBar);


        vLayoutChannels->addWidget(groupEpgShort);

        vLayoutChannels->setStretch(0, 6);
        vLayoutChannels->setStretch(1, 4);

        horizontalLayout->addLayout(vLayoutChannels);

        groupEPG = new QGroupBox(Recorder);
        groupEPG->setObjectName(QString::fromUtf8("groupEPG"));
        verticalLayout_2 = new QVBoxLayout(groupEPG);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setMargin(11);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        frameEpgCtrl = new QFrame(groupEPG);
        frameEpgCtrl->setObjectName(QString::fromUtf8("frameEpgCtrl"));
        frameEpgCtrl->setStyleSheet(QString::fromUtf8("QFrame#frameEpgCtrl {\n"
"background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 #eee, 	stop:0.7 grey, stop:1 #dedede);\n"
"};"));
        frameEpgCtrl->setFrameShape(QFrame::Panel);
        frameEpgCtrl->setFrameShadow(QFrame::Raised);
        horizontalLayout_5 = new QHBoxLayout(frameEpgCtrl);
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setMargin(4);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        frameChan = new QFrame(frameEpgCtrl);
        frameChan->setObjectName(QString::fromUtf8("frameChan"));
        frameChan->setStyleSheet(QString::fromUtf8(""));
        frameChan->setFrameShape(QFrame::Panel);
        frameChan->setFrameShadow(QFrame::Sunken);
        horizontalLayout_3 = new QHBoxLayout(frameChan);
        horizontalLayout_3->setSpacing(4);
        horizontalLayout_3->setMargin(11);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        horizontalLayout_3->setContentsMargins(4, 2, 4, 2);
        labChanIcon = new QLabel(frameChan);
        labChanIcon->setObjectName(QString::fromUtf8("labChanIcon"));
        labChanIcon->setMinimumSize(QSize(24, 24));

        horizontalLayout_3->addWidget(labChanIcon);

        labChanName = new QLabel(frameChan);
        labChanName->setObjectName(QString::fromUtf8("labChanName"));
        labChanName->setStyleSheet(QString::fromUtf8("QLabel#labChanName {\n"
"font-weight: bold;\n"
"color: white;\n"
"}"));

        horizontalLayout_3->addWidget(labChanName);


        horizontalLayout_5->addWidget(frameChan);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_2);

        frameNavi = new QFrame(frameEpgCtrl);
        frameNavi->setObjectName(QString::fromUtf8("frameNavi"));
        frameNavi->setFrameShape(QFrame::Panel);
        frameNavi->setFrameShadow(QFrame::Sunken);
        horizontalLayout_4 = new QHBoxLayout(frameNavi);
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setMargin(2);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        btnBack = new QToolButton(frameNavi);
        btnBack->setObjectName(QString::fromUtf8("btnBack"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/png/back"), QSize(), QIcon::Normal, QIcon::Off);
        btnBack->setIcon(icon1);
        btnBack->setAutoRaise(true);
        btnBack->setArrowType(Qt::NoArrow);

        horizontalLayout_4->addWidget(btnBack);

        labCurrDay = new QLabel(frameNavi);
        labCurrDay->setObjectName(QString::fromUtf8("labCurrDay"));
        labCurrDay->setStyleSheet(QString::fromUtf8("QLabel#labCurrDay {\n"
"font-weight: bold;\n"
"color: white;\n"
"}"));

        horizontalLayout_4->addWidget(labCurrDay);

        btnNext = new QToolButton(frameNavi);
        btnNext->setObjectName(QString::fromUtf8("btnNext"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/png/next"), QSize(), QIcon::Normal, QIcon::Off);
        btnNext->setIcon(icon2);
        btnNext->setAutoRaise(true);
        btnNext->setArrowType(Qt::NoArrow);

        horizontalLayout_4->addWidget(btnNext);


        horizontalLayout_5->addWidget(frameNavi);


        verticalLayout_2->addWidget(frameEpgCtrl);

        textEpg = new CEpgBrowser(groupEPG);
        textEpg->setObjectName(QString::fromUtf8("textEpg"));
        textEpg->setStyleSheet(QString::fromUtf8(""));
        textEpg->setLocale(QLocale(QLocale::English, QLocale::UnitedStates));
        textEpg->setOpenLinks(false);

        verticalLayout_2->addWidget(textEpg);

        verticalLayout_2->setStretch(1, 10);

        horizontalLayout->addWidget(groupEPG);

        horizontalLayout->setStretch(0, 40);
        horizontalLayout->setStretch(1, 60);

        verticalLayout_4->addLayout(horizontalLayout);

        frame = new QFrame(Recorder);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setStyleSheet(QString::fromUtf8("QFrame#frame {\n"
"background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0, y2:1, stop:0 #eee, 	stop:0.7 grey, stop:1 #dedede);\n"
"background-image: url(:/app/time);\n"
"background-position: left center;\n"
"background-repeat: no;\n"
"background-attachment: fixed;\n"
"};"));
        frame->setFrameShape(QFrame::Panel);
        frame->setFrameShadow(QFrame::Raised);
        frame->setLineWidth(1);
        horizontalLayout_2 = new QHBoxLayout(frame);
        horizontalLayout_2->setSpacing(3);
        horizontalLayout_2->setMargin(11);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(42, 5, 5, 5);
        cbxTimeShift = new QComboBox(frame);
        cbxTimeShift->setObjectName(QString::fromUtf8("cbxTimeShift"));
        cbxTimeShift->setMinimumSize(QSize(40, 0));
        cbxTimeShift->setMaximumSize(QSize(40, 16777215));

        horizontalLayout_2->addWidget(cbxTimeShift);

        line = new QFrame(frame);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Sunken);

        horizontalLayout_2->addWidget(line);

        pushPlay = new QPushButton(frame);
        pushPlay->setObjectName(QString::fromUtf8("pushPlay"));
        pushPlay->setMaximumSize(QSize(34, 34));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/app/play"), QSize(), QIcon::Normal, QIcon::Off);
        pushPlay->setIcon(icon3);
        pushPlay->setIconSize(QSize(32, 32));
        pushPlay->setDefault(true);
        pushPlay->setFlat(true);

        horizontalLayout_2->addWidget(pushPlay);

        pushRecord = new QPushButton(frame);
        pushRecord->setObjectName(QString::fromUtf8("pushRecord"));
        pushRecord->setMaximumSize(QSize(34, 34));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/app/record"), QSize(), QIcon::Normal, QIcon::Off);
        pushRecord->setIcon(icon4);
        pushRecord->setIconSize(QSize(32, 32));
        pushRecord->setFlat(true);

        horizontalLayout_2->addWidget(pushRecord);

        line_2 = new QFrame(frame);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setFrameShape(QFrame::VLine);
        line_2->setFrameShadow(QFrame::Sunken);

        horizontalLayout_2->addWidget(line_2);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        line_3 = new QFrame(frame);
        line_3->setObjectName(QString::fromUtf8("line_3"));
        line_3->setFrameShape(QFrame::VLine);
        line_3->setFrameShadow(QFrame::Sunken);

        horizontalLayout_2->addWidget(line_3);

        pushSettings = new QPushButton(frame);
        pushSettings->setObjectName(QString::fromUtf8("pushSettings"));
        pushSettings->setMinimumSize(QSize(28, 28));
        pushSettings->setMaximumSize(QSize(28, 28));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/app/settings"), QSize(), QIcon::Normal, QIcon::Off);
        pushSettings->setIcon(icon5);
        pushSettings->setIconSize(QSize(24, 24));
        pushSettings->setFlat(true);

        horizontalLayout_2->addWidget(pushSettings);

        pushAbout = new QPushButton(frame);
        pushAbout->setObjectName(QString::fromUtf8("pushAbout"));
        pushAbout->setMinimumSize(QSize(28, 28));
        pushAbout->setMaximumSize(QSize(28, 28));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/app/info"), QSize(), QIcon::Normal, QIcon::Off);
        pushAbout->setIcon(icon6);
        pushAbout->setIconSize(QSize(24, 24));
        pushAbout->setFlat(true);

        horizontalLayout_2->addWidget(pushAbout);

        pushQuit = new QPushButton(frame);
        pushQuit->setObjectName(QString::fromUtf8("pushQuit"));
        pushQuit->setMinimumSize(QSize(28, 28));
        pushQuit->setMaximumSize(QSize(28, 28));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/app/quit"), QSize(), QIcon::Normal, QIcon::Off);
        pushQuit->setIcon(icon7);
        pushQuit->setIconSize(QSize(24, 24));
        pushQuit->setFlat(true);

        horizontalLayout_2->addWidget(pushQuit);


        verticalLayout_4->addWidget(frame);

        QWidget::setTabOrder(cbxChannelGroup, listWidget);
        QWidget::setTabOrder(listWidget, textEpgShort);
        QWidget::setTabOrder(textEpgShort, cbxTimeShift);
        QWidget::setTabOrder(cbxTimeShift, pushPlay);
        QWidget::setTabOrder(pushPlay, pushRecord);
        QWidget::setTabOrder(pushRecord, textEpg);
        QWidget::setTabOrder(textEpg, pushSettings);
        QWidget::setTabOrder(pushSettings, pushQuit);

        retranslateUi(Recorder);
        QObject::connect(pushQuit, SIGNAL(clicked()), Recorder, SLOT(accept()));

        QMetaObject::connectSlotsByName(Recorder);
    } // setupUi

    void retranslateUi(QDialog *Recorder)
    {
        Recorder->setWindowTitle(QApplication::translate("Recorder", "VLC Recorder", 0, QApplication::UnicodeUTF8));
        groupChannels->setTitle(QApplication::translate("Recorder", " Channels ", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        cbxChannelGroup->setWhatsThis(QApplication::translate("Recorder", "Channel Group", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_TOOLTIP
        listWidget->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        listWidget->setWhatsThis(QApplication::translate("Recorder", "Channel List", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        groupEpgShort->setTitle(QApplication::translate("Recorder", " Now Playing ", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_WHATSTHIS
        textEpgShort->setWhatsThis(QApplication::translate("Recorder", "Short EPG", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_WHATSTHIS
        progressBar->setWhatsThis(QApplication::translate("Recorder", "Time Progress for played programm", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        groupEPG->setTitle(QApplication::translate("Recorder", " EPG - Electronic Program Guide ", 0, QApplication::UnicodeUTF8));
        labChanIcon->setText(QString());
        labChanName->setText(QString());
        btnBack->setText(QApplication::translate("Recorder", "...", 0, QApplication::UnicodeUTF8));
        labCurrDay->setText(QString());
        btnNext->setText(QApplication::translate("Recorder", "...", 0, QApplication::UnicodeUTF8));
        cbxTimeShift->clear();
        cbxTimeShift->insertItems(0, QStringList()
         << QApplication::translate("Recorder", "0", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Recorder", "1", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Recorder", "2", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Recorder", "3", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Recorder", "4", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Recorder", "8", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Recorder", "9", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Recorder", "10", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("Recorder", "11", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        cbxTimeShift->setToolTip(QApplication::translate("Recorder", "Set TimeShift in hours.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        cbxTimeShift->setWhatsThis(QApplication::translate("Recorder", "TimeShift Selector", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
#ifndef QT_NO_TOOLTIP
        pushPlay->setToolTip(QApplication::translate("Recorder", "Play selected Channel", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushPlay->setText(QString());
        pushPlay->setShortcut(QApplication::translate("Recorder", "Alt+P", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushRecord->setToolTip(QApplication::translate("Recorder", "Record selected Channel", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushRecord->setText(QString());
        pushRecord->setShortcut(QApplication::translate("Recorder", "Alt+R", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushSettings->setToolTip(QApplication::translate("Recorder", "Settings", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushSettings->setText(QString());
        pushSettings->setShortcut(QApplication::translate("Recorder", "Alt+S", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushAbout->setToolTip(QApplication::translate("Recorder", "About ...", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushAbout->setText(QString());
        pushAbout->setShortcut(QApplication::translate("Recorder", "Alt+I", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        pushQuit->setToolTip(QApplication::translate("Recorder", "Quit", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        pushQuit->setText(QString());
        pushQuit->setShortcut(QApplication::translate("Recorder", "Alt+Q", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(Recorder);
    } // retranslateUi

};

namespace Ui {
    class Recorder: public Ui_Recorder {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RECORDER_H
