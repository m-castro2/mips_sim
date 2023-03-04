/********************************************************************************
** Form generated from reading UI file 'mips_sim_settings.ui'
**
** Created by: Qt User Interface Compiler version 5.15.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MIPS_SIM_SETTINGS_H
#define UI_MIPS_SIM_SETTINGS_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_MipsSimSettings
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *frame;
    QFormLayout *formLayout;
    QLabel *lblPipelinedProcessor;
    QFrame *framePipelinedProcessor;
    QVBoxLayout *verticalLayout_3;
    QCheckBox *cbForwarding;
    QCheckBox *cbHazardDetection;
    QLabel *lblBranchProcessing;
    QFrame *frameBranch;
    QHBoxLayout *horizontalLayout;
    QFrame *frameBType;
    QVBoxLayout *verticalLayout_4;
    QRadioButton *radBTypeNonTaken;
    QRadioButton *radBTypeFlush;
    QRadioButton *radBTypeDelayed;
    QFrame *frameBStage;
    QVBoxLayout *verticalLayout_5;
    QRadioButton *radBStageID;
    QRadioButton *radBStageMEM;
    QSpacerItem *verticalSpacer;
    QLabel *lblMain;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *MipsSimSettings)
    {
        if (MipsSimSettings->objectName().isEmpty())
            MipsSimSettings->setObjectName(QString::fromUtf8("MipsSimSettings"));
        MipsSimSettings->resize(662, 337);
        verticalLayout = new QVBoxLayout(MipsSimSettings);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        frame = new QFrame(MipsSimSettings);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        formLayout = new QFormLayout(frame);
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        lblPipelinedProcessor = new QLabel(frame);
        lblPipelinedProcessor->setObjectName(QString::fromUtf8("lblPipelinedProcessor"));

        formLayout->setWidget(1, QFormLayout::LabelRole, lblPipelinedProcessor);

        framePipelinedProcessor = new QFrame(frame);
        framePipelinedProcessor->setObjectName(QString::fromUtf8("framePipelinedProcessor"));
        framePipelinedProcessor->setFrameShape(QFrame::StyledPanel);
        framePipelinedProcessor->setFrameShadow(QFrame::Raised);
        verticalLayout_3 = new QVBoxLayout(framePipelinedProcessor);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        cbForwarding = new QCheckBox(framePipelinedProcessor);
        cbForwarding->setObjectName(QString::fromUtf8("cbForwarding"));

        verticalLayout_3->addWidget(cbForwarding);

        cbHazardDetection = new QCheckBox(framePipelinedProcessor);
        cbHazardDetection->setObjectName(QString::fromUtf8("cbHazardDetection"));

        verticalLayout_3->addWidget(cbHazardDetection);

        lblBranchProcessing = new QLabel(framePipelinedProcessor);
        lblBranchProcessing->setObjectName(QString::fromUtf8("lblBranchProcessing"));

        verticalLayout_3->addWidget(lblBranchProcessing);

        frameBranch = new QFrame(framePipelinedProcessor);
        frameBranch->setObjectName(QString::fromUtf8("frameBranch"));
        frameBranch->setFrameShape(QFrame::StyledPanel);
        frameBranch->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(frameBranch);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        frameBType = new QFrame(frameBranch);
        frameBType->setObjectName(QString::fromUtf8("frameBType"));
        frameBType->setFrameShape(QFrame::StyledPanel);
        frameBType->setFrameShadow(QFrame::Raised);
        verticalLayout_4 = new QVBoxLayout(frameBType);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        radBTypeNonTaken = new QRadioButton(frameBType);
        radBTypeNonTaken->setObjectName(QString::fromUtf8("radBTypeNonTaken"));

        verticalLayout_4->addWidget(radBTypeNonTaken);

        radBTypeFlush = new QRadioButton(frameBType);
        radBTypeFlush->setObjectName(QString::fromUtf8("radBTypeFlush"));

        verticalLayout_4->addWidget(radBTypeFlush);

        radBTypeDelayed = new QRadioButton(frameBType);
        radBTypeDelayed->setObjectName(QString::fromUtf8("radBTypeDelayed"));

        verticalLayout_4->addWidget(radBTypeDelayed);


        horizontalLayout->addWidget(frameBType);

        frameBStage = new QFrame(frameBranch);
        frameBStage->setObjectName(QString::fromUtf8("frameBStage"));
        frameBStage->setFrameShape(QFrame::StyledPanel);
        frameBStage->setFrameShadow(QFrame::Raised);
        verticalLayout_5 = new QVBoxLayout(frameBStage);
        verticalLayout_5->setObjectName(QString::fromUtf8("verticalLayout_5"));
        radBStageID = new QRadioButton(frameBStage);
        radBStageID->setObjectName(QString::fromUtf8("radBStageID"));

        verticalLayout_5->addWidget(radBStageID);

        radBStageMEM = new QRadioButton(frameBStage);
        radBStageMEM->setObjectName(QString::fromUtf8("radBStageMEM"));

        verticalLayout_5->addWidget(radBStageMEM);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_5->addItem(verticalSpacer);


        horizontalLayout->addWidget(frameBStage);


        verticalLayout_3->addWidget(frameBranch);


        formLayout->setWidget(2, QFormLayout::LabelRole, framePipelinedProcessor);

        lblMain = new QLabel(frame);
        lblMain->setObjectName(QString::fromUtf8("lblMain"));
        QFont font;
        font.setBold(true);
        font.setUnderline(true);
        font.setWeight(75);
        lblMain->setFont(font);

        formLayout->setWidget(0, QFormLayout::LabelRole, lblMain);


        verticalLayout->addWidget(frame);

        buttonBox = new QDialogButtonBox(MipsSimSettings);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(MipsSimSettings);
        QObject::connect(buttonBox, SIGNAL(accepted()), MipsSimSettings, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), MipsSimSettings, SLOT(reject()));

        QMetaObject::connectSlotsByName(MipsSimSettings);
    } // setupUi

    void retranslateUi(QDialog *MipsSimSettings)
    {
        MipsSimSettings->setWindowTitle(QCoreApplication::translate("MipsSimSettings", "Dialog", nullptr));
        lblPipelinedProcessor->setText(QCoreApplication::translate("MipsSimSettings", "Pipelined processor", nullptr));
        cbForwarding->setText(QCoreApplication::translate("MipsSimSettings", "Forwarding unit", nullptr));
        cbHazardDetection->setText(QCoreApplication::translate("MipsSimSettings", "Hazard detection unit", nullptr));
        lblBranchProcessing->setText(QCoreApplication::translate("MipsSimSettings", "Branch processing:", nullptr));
        radBTypeNonTaken->setText(QCoreApplication::translate("MipsSimSettings", "Fixed non-taken branch", nullptr));
        radBTypeFlush->setText(QCoreApplication::translate("MipsSimSettings", "Flush pipeline on branch", nullptr));
        radBTypeDelayed->setText(QCoreApplication::translate("MipsSimSettings", "Delayed branch", nullptr));
        radBStageID->setText(QCoreApplication::translate("MipsSimSettings", "Branch decided at ID", nullptr));
        radBStageMEM->setText(QCoreApplication::translate("MipsSimSettings", "Branch decided at MEM", nullptr));
        lblMain->setText(QCoreApplication::translate("MipsSimSettings", "MIPS Simulator Settings", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MipsSimSettings: public Ui_MipsSimSettings {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MIPS_SIM_SETTINGS_H
