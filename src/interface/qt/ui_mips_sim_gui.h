/********************************************************************************
** Form generated from reading UI file 'mips_sim_gui.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MIPS_SIM_GUI_H
#define UI_MIPS_SIM_GUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MipsSimGui
{
public:
    QAction *actionExit;
    QAction *actionLoad_file;
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_3;
    QFrame *mainFrame;
    QHBoxLayout *mainLayout;
    QVBoxLayout *boxRegisters;
    QFrame *frameSRegs;
    QGridLayout *pcGridLayout;
    QLabel *lblStatusid;
    QLabel *lblHIid;
    QLabel *lblStatusValue;
    QLabel *lblLOid;
    QLabel *lblEPCValue;
    QLabel *lblEPCid;
    QLabel *lblBadVAddrid;
    QLabel *lblLOValue;
    QLabel *lblPCid;
    QLabel *lblCauseid;
    QLabel *lblPCValue;
    QLabel *lblCauseValue;
    QLabel *lblHIValue;
    QLabel *lblBadVAddrValue;
    QScrollArea *scrollArea;
    QWidget *scrollAreaWidgetContents_2;
    QVBoxLayout *verticalLayout_4;
    QFrame *frameRegs;
    QGridLayout *iregsLayout;
    QTabWidget *tabWidget;
    QWidget *tab;
    QVBoxLayout *verticalLayout_5;
    QFrame *frameFRegs;
    QGridLayout *fregsLayout;
    QWidget *tab_2;
    QVBoxLayout *verticalLayout_6;
    QFrame *frameDRegs;
    QGridLayout *dregsLayout;
    QLabel *label_3;
    QScrollArea *scrollMem;
    QWidget *scrollMemData;
    QVBoxLayout *verticalLayout_7;
    QFrame *frameProcessor;
    QVBoxLayout *verticalLayout;
    QFrame *frame;
    QVBoxLayout *verticalLayout_8;
    QFrame *frameDiagram;
    QGridLayout *gridLayout_2;
    QSpacerItem *verticalSpacer;
    QLabel *label_4;
    QScrollArea *scrollDiagram;
    QWidget *scrollAreaWidgetContents;
    QVBoxLayout *verticalLayout_2;
    QFrame *frameTMem;
    QGridLayout *gridLayout;
    QSpacerItem *verticalSpacer_2;
    QFrame *frameButtons;
    QHBoxLayout *buttonsLayout;
    QPushButton *btnReset;
    QPushButton *btnPrev;
    QPushButton *btnNext;
    QPushButton *btnRun;
    QLabel *lblCycles;
    QMenuBar *menubar;
    QMenu *menuFile;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MipsSimGui)
    {
        if (MipsSimGui->objectName().isEmpty())
            MipsSimGui->setObjectName(QString::fromUtf8("MipsSimGui"));
        MipsSimGui->resize(1068, 750);
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MipsSimGui->sizePolicy().hasHeightForWidth());
        MipsSimGui->setSizePolicy(sizePolicy);
        MipsSimGui->setMinimumSize(QSize(0, 750));
        actionExit = new QAction(MipsSimGui);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionLoad_file = new QAction(MipsSimGui);
        actionLoad_file->setObjectName(QString::fromUtf8("actionLoad_file"));
        centralwidget = new QWidget(MipsSimGui);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        centralwidget->setEnabled(true);
        verticalLayout_3 = new QVBoxLayout(centralwidget);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        mainFrame = new QFrame(centralwidget);
        mainFrame->setObjectName(QString::fromUtf8("mainFrame"));
        mainFrame->setMinimumSize(QSize(0, 50));
        mainLayout = new QHBoxLayout(mainFrame);
        mainLayout->setObjectName(QString::fromUtf8("mainLayout"));
        boxRegisters = new QVBoxLayout();
        boxRegisters->setObjectName(QString::fromUtf8("boxRegisters"));
        frameSRegs = new QFrame(mainFrame);
        frameSRegs->setObjectName(QString::fromUtf8("frameSRegs"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(frameSRegs->sizePolicy().hasHeightForWidth());
        frameSRegs->setSizePolicy(sizePolicy1);
        frameSRegs->setMinimumSize(QSize(550, 80));
        frameSRegs->setMaximumSize(QSize(550, 80));
        frameSRegs->setFrameShape(QFrame::StyledPanel);
        pcGridLayout = new QGridLayout(frameSRegs);
        pcGridLayout->setObjectName(QString::fromUtf8("pcGridLayout"));
        lblStatusid = new QLabel(frameSRegs);
        lblStatusid->setObjectName(QString::fromUtf8("lblStatusid"));
        QFont font;
        font.setFamily(QString::fromUtf8("Monospace"));
        font.setPointSize(10);
        lblStatusid->setFont(font);

        pcGridLayout->addWidget(lblStatusid, 1, 0, 1, 1);

        lblHIid = new QLabel(frameSRegs);
        lblHIid->setObjectName(QString::fromUtf8("lblHIid"));
        lblHIid->setFont(font);

        pcGridLayout->addWidget(lblHIid, 3, 0, 1, 1);

        lblStatusValue = new QLabel(frameSRegs);
        lblStatusValue->setObjectName(QString::fromUtf8("lblStatusValue"));
        lblStatusValue->setFont(font);
        lblStatusValue->setFrameShape(QFrame::StyledPanel);

        pcGridLayout->addWidget(lblStatusValue, 1, 1, 1, 1);

        lblLOid = new QLabel(frameSRegs);
        lblLOid->setObjectName(QString::fromUtf8("lblLOid"));
        lblLOid->setFont(font);

        pcGridLayout->addWidget(lblLOid, 3, 2, 1, 1);

        lblEPCValue = new QLabel(frameSRegs);
        lblEPCValue->setObjectName(QString::fromUtf8("lblEPCValue"));
        lblEPCValue->setFont(font);
        lblEPCValue->setFrameShape(QFrame::StyledPanel);

        pcGridLayout->addWidget(lblEPCValue, 0, 3, 1, 1);

        lblEPCid = new QLabel(frameSRegs);
        lblEPCid->setObjectName(QString::fromUtf8("lblEPCid"));
        lblEPCid->setFont(font);

        pcGridLayout->addWidget(lblEPCid, 0, 2, 1, 1);

        lblBadVAddrid = new QLabel(frameSRegs);
        lblBadVAddrid->setObjectName(QString::fromUtf8("lblBadVAddrid"));
        lblBadVAddrid->setFont(font);

        pcGridLayout->addWidget(lblBadVAddrid, 1, 2, 1, 1);

        lblLOValue = new QLabel(frameSRegs);
        lblLOValue->setObjectName(QString::fromUtf8("lblLOValue"));
        lblLOValue->setFont(font);
        lblLOValue->setFrameShape(QFrame::StyledPanel);

        pcGridLayout->addWidget(lblLOValue, 3, 3, 1, 1);

        lblPCid = new QLabel(frameSRegs);
        lblPCid->setObjectName(QString::fromUtf8("lblPCid"));
        lblPCid->setFont(font);

        pcGridLayout->addWidget(lblPCid, 0, 0, 1, 1);

        lblCauseid = new QLabel(frameSRegs);
        lblCauseid->setObjectName(QString::fromUtf8("lblCauseid"));
        lblCauseid->setFont(font);

        pcGridLayout->addWidget(lblCauseid, 0, 4, 1, 1);

        lblPCValue = new QLabel(frameSRegs);
        lblPCValue->setObjectName(QString::fromUtf8("lblPCValue"));
        lblPCValue->setFont(font);
        lblPCValue->setFrameShape(QFrame::StyledPanel);

        pcGridLayout->addWidget(lblPCValue, 0, 1, 1, 1);

        lblCauseValue = new QLabel(frameSRegs);
        lblCauseValue->setObjectName(QString::fromUtf8("lblCauseValue"));
        lblCauseValue->setFont(font);
        lblCauseValue->setFrameShape(QFrame::StyledPanel);

        pcGridLayout->addWidget(lblCauseValue, 0, 5, 1, 1);

        lblHIValue = new QLabel(frameSRegs);
        lblHIValue->setObjectName(QString::fromUtf8("lblHIValue"));
        lblHIValue->setFont(font);
        lblHIValue->setFrameShape(QFrame::StyledPanel);

        pcGridLayout->addWidget(lblHIValue, 3, 1, 1, 1);

        lblBadVAddrValue = new QLabel(frameSRegs);
        lblBadVAddrValue->setObjectName(QString::fromUtf8("lblBadVAddrValue"));
        lblBadVAddrValue->setFont(font);
        lblBadVAddrValue->setFrameShape(QFrame::StyledPanel);

        pcGridLayout->addWidget(lblBadVAddrValue, 1, 3, 1, 1);


        boxRegisters->addWidget(frameSRegs);

        scrollArea = new QScrollArea(mainFrame);
        scrollArea->setObjectName(QString::fromUtf8("scrollArea"));
        scrollArea->setMinimumSize(QSize(550, 400));
        scrollArea->setMaximumSize(QSize(550, 800));
        scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scrollArea->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        scrollArea->setWidgetResizable(true);
        scrollAreaWidgetContents_2 = new QWidget();
        scrollAreaWidgetContents_2->setObjectName(QString::fromUtf8("scrollAreaWidgetContents_2"));
        scrollAreaWidgetContents_2->setGeometry(QRect(0, 0, 548, 514));
        verticalLayout_4 = new QVBoxLayout(scrollAreaWidgetContents_2);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        frameRegs = new QFrame(scrollAreaWidgetContents_2);
        frameRegs->setObjectName(QString::fromUtf8("frameRegs"));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(frameRegs->sizePolicy().hasHeightForWidth());
        frameRegs->setSizePolicy(sizePolicy2);
        frameRegs->setMinimumSize(QSize(0, 160));
        frameRegs->setMaximumSize(QSize(16777215, 160));
        frameRegs->setFrameShape(QFrame::NoFrame);
        iregsLayout = new QGridLayout(frameRegs);
        iregsLayout->setObjectName(QString::fromUtf8("iregsLayout"));
        iregsLayout->setContentsMargins(0, -1, 0, -1);

        verticalLayout_4->addWidget(frameRegs);

        tabWidget = new QTabWidget(scrollAreaWidgetContents_2);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setFocusPolicy(Qt::NoFocus);
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        tab->setMinimumSize(QSize(0, 165));
        verticalLayout_5 = new QVBoxLayout(tab);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        frameFRegs = new QFrame(tab);
        frameFRegs->setObjectName(QString::fromUtf8("frameFRegs"));
        sizePolicy2.setHeightForWidth(frameFRegs->sizePolicy().hasHeightForWidth());
        frameFRegs->setSizePolicy(sizePolicy2);
        frameFRegs->setMinimumSize(QSize(0, 160));
        frameFRegs->setMaximumSize(QSize(16777215, 160));
        frameFRegs->setFrameShape(QFrame::NoFrame);
        fregsLayout = new QGridLayout(frameFRegs);
        fregsLayout->setObjectName(QString::fromUtf8("fregsLayout"));
        fregsLayout->setContentsMargins(0, -1, 0, -1);

        verticalLayout_5->addWidget(frameFRegs);

        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        tab_2->setMinimumSize(QSize(0, 85));
        verticalLayout_6 = new QVBoxLayout(tab_2);
        verticalLayout_6->setObjectName(QString::fromUtf8("verticalLayout_6"));
        frameDRegs = new QFrame(tab_2);
        frameDRegs->setObjectName(QString::fromUtf8("frameDRegs"));
        sizePolicy2.setHeightForWidth(frameDRegs->sizePolicy().hasHeightForWidth());
        frameDRegs->setSizePolicy(sizePolicy2);
        frameDRegs->setMinimumSize(QSize(0, 80));
        frameDRegs->setMaximumSize(QSize(16777215, 80));
        frameDRegs->setFrameShape(QFrame::NoFrame);
        dregsLayout = new QGridLayout(frameDRegs);
        dregsLayout->setObjectName(QString::fromUtf8("dregsLayout"));
        dregsLayout->setContentsMargins(0, -1, 0, -1);

        verticalLayout_6->addWidget(frameDRegs);

        tabWidget->addTab(tab_2, QString());

        verticalLayout_4->addWidget(tabWidget);

        label_3 = new QLabel(scrollAreaWidgetContents_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        verticalLayout_4->addWidget(label_3);

        scrollMem = new QScrollArea(scrollAreaWidgetContents_2);
        scrollMem->setObjectName(QString::fromUtf8("scrollMem"));
        scrollMem->setWidgetResizable(true);
        scrollMemData = new QWidget();
        scrollMemData->setObjectName(QString::fromUtf8("scrollMemData"));
        scrollMemData->setGeometry(QRect(0, 0, 528, 88));
        verticalLayout_7 = new QVBoxLayout(scrollMemData);
        verticalLayout_7->setSpacing(0);
        verticalLayout_7->setObjectName(QString::fromUtf8("verticalLayout_7"));
        scrollMem->setWidget(scrollMemData);

        verticalLayout_4->addWidget(scrollMem);

        scrollArea->setWidget(scrollAreaWidgetContents_2);

        boxRegisters->addWidget(scrollArea);


        mainLayout->addLayout(boxRegisters);

        frameProcessor = new QFrame(mainFrame);
        frameProcessor->setObjectName(QString::fromUtf8("frameProcessor"));
        frameProcessor->setFrameShape(QFrame::StyledPanel);
        frameProcessor->setFrameShadow(QFrame::Raised);
        verticalLayout = new QVBoxLayout(frameProcessor);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        frame = new QFrame(frameProcessor);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setMinimumSize(QSize(0, 250));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        verticalLayout_8 = new QVBoxLayout(frame);
        verticalLayout_8->setSpacing(0);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        verticalLayout_8->setContentsMargins(0, 0, 0, 0);
        frameDiagram = new QFrame(frame);
        frameDiagram->setObjectName(QString::fromUtf8("frameDiagram"));
        frameDiagram->setMinimumSize(QSize(0, 0));
        frameDiagram->setMaximumSize(QSize(550, 16777215));
        frameDiagram->setFrameShape(QFrame::NoFrame);
        gridLayout_2 = new QGridLayout(frameDiagram);
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));

        verticalLayout_8->addWidget(frameDiagram);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_8->addItem(verticalSpacer);


        verticalLayout->addWidget(frame);

        label_4 = new QLabel(frameProcessor);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        verticalLayout->addWidget(label_4);

        scrollDiagram = new QScrollArea(frameProcessor);
        scrollDiagram->setObjectName(QString::fromUtf8("scrollDiagram"));
        scrollDiagram->setMinimumSize(QSize(400, 0));
        scrollDiagram->setWidgetResizable(true);
        scrollAreaWidgetContents = new QWidget();
        scrollAreaWidgetContents->setObjectName(QString::fromUtf8("scrollAreaWidgetContents"));
        scrollAreaWidgetContents->setGeometry(QRect(0, 0, 452, 275));
        verticalLayout_2 = new QVBoxLayout(scrollAreaWidgetContents);
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout_2->setContentsMargins(0, 0, 0, 0);
        frameTMem = new QFrame(scrollAreaWidgetContents);
        frameTMem->setObjectName(QString::fromUtf8("frameTMem"));
        frameTMem->setMinimumSize(QSize(0, 200));
        gridLayout = new QGridLayout(frameTMem);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));

        verticalLayout_2->addWidget(frameTMem);

        verticalSpacer_2 = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_2);

        scrollDiagram->setWidget(scrollAreaWidgetContents);

        verticalLayout->addWidget(scrollDiagram);


        mainLayout->addWidget(frameProcessor);


        verticalLayout_3->addWidget(mainFrame);

        frameButtons = new QFrame(centralwidget);
        frameButtons->setObjectName(QString::fromUtf8("frameButtons"));
        frameButtons->setMaximumSize(QSize(16777215, 60));
        buttonsLayout = new QHBoxLayout(frameButtons);
        buttonsLayout->setObjectName(QString::fromUtf8("buttonsLayout"));
        btnReset = new QPushButton(frameButtons);
        btnReset->setObjectName(QString::fromUtf8("btnReset"));

        buttonsLayout->addWidget(btnReset);

        btnPrev = new QPushButton(frameButtons);
        btnPrev->setObjectName(QString::fromUtf8("btnPrev"));

        buttonsLayout->addWidget(btnPrev);

        btnNext = new QPushButton(frameButtons);
        btnNext->setObjectName(QString::fromUtf8("btnNext"));

        buttonsLayout->addWidget(btnNext);

        btnRun = new QPushButton(frameButtons);
        btnRun->setObjectName(QString::fromUtf8("btnRun"));

        buttonsLayout->addWidget(btnRun);

        lblCycles = new QLabel(frameButtons);
        lblCycles->setObjectName(QString::fromUtf8("lblCycles"));
        lblCycles->setMaximumSize(QSize(100, 16777215));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Monospace"));
        font1.setPointSize(40);
        font1.setBold(false);
        font1.setWeight(50);
        font1.setKerning(true);
        lblCycles->setFont(font1);

        buttonsLayout->addWidget(lblCycles);


        verticalLayout_3->addWidget(frameButtons);

        MipsSimGui->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MipsSimGui);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1068, 22));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        MipsSimGui->setMenuBar(menubar);
        statusbar = new QStatusBar(MipsSimGui);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MipsSimGui->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());
        menuFile->addAction(actionLoad_file);
        menuFile->addAction(actionExit);

        retranslateUi(MipsSimGui);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(MipsSimGui);
    } // setupUi

    void retranslateUi(QMainWindow *MipsSimGui)
    {
        MipsSimGui->setWindowTitle(QApplication::translate("MipsSimGui", "MipsSimGui", nullptr));
        actionExit->setText(QApplication::translate("MipsSimGui", "Exit", nullptr));
#ifndef QT_NO_TOOLTIP
        actionExit->setToolTip(QApplication::translate("MipsSimGui", "Exit Mips Simulator", nullptr));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_SHORTCUT
        actionExit->setShortcut(QApplication::translate("MipsSimGui", "Ctrl+Q", nullptr));
#endif // QT_NO_SHORTCUT
        actionLoad_file->setText(QApplication::translate("MipsSimGui", "Load file", nullptr));
#ifndef QT_NO_SHORTCUT
        actionLoad_file->setShortcut(QApplication::translate("MipsSimGui", "Ctrl+O", nullptr));
#endif // QT_NO_SHORTCUT
        lblStatusid->setText(QApplication::translate("MipsSimGui", "Status =", nullptr));
        lblHIid->setText(QApplication::translate("MipsSimGui", "HI     =", nullptr));
        lblStatusValue->setText(QApplication::translate("MipsSimGui", "00000000", nullptr));
        lblLOid->setText(QApplication::translate("MipsSimGui", "LO       =", nullptr));
        lblEPCValue->setText(QApplication::translate("MipsSimGui", "00000000", nullptr));
        lblEPCid->setText(QApplication::translate("MipsSimGui", "EPC      =", nullptr));
        lblBadVAddrid->setText(QApplication::translate("MipsSimGui", "BadVAddr =", nullptr));
        lblLOValue->setText(QApplication::translate("MipsSimGui", "00000000", nullptr));
        lblPCid->setText(QApplication::translate("MipsSimGui", "PC     =", nullptr));
        lblCauseid->setText(QApplication::translate("MipsSimGui", "Cause =", nullptr));
        lblPCValue->setText(QApplication::translate("MipsSimGui", "00400000", nullptr));
        lblCauseValue->setText(QApplication::translate("MipsSimGui", "00000000", nullptr));
        lblHIValue->setText(QApplication::translate("MipsSimGui", "00000000", nullptr));
        lblBadVAddrValue->setText(QApplication::translate("MipsSimGui", "00000000", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("MipsSimGui", "Single precision FP", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("MipsSimGui", "Double precision FP", nullptr));
        label_3->setText(QApplication::translate("MipsSimGui", "Data memory", nullptr));
        label_4->setText(QApplication::translate("MipsSimGui", "Instructions memory", nullptr));
        btnReset->setText(QApplication::translate("MipsSimGui", "Reset", nullptr));
        btnPrev->setText(QApplication::translate("MipsSimGui", "Prev Cycle", nullptr));
        btnNext->setText(QApplication::translate("MipsSimGui", "Next Cycle", nullptr));
        btnRun->setText(QApplication::translate("MipsSimGui", "Run", nullptr));
        lblCycles->setText(QApplication::translate("MipsSimGui", "0", nullptr));
        menuFile->setTitle(QApplication::translate("MipsSimGui", "File", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MipsSimGui: public Ui_MipsSimGui {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MIPS_SIM_GUI_H
