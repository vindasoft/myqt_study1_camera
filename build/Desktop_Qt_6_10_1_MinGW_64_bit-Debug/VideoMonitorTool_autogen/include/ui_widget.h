/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 6.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QComboBox *cameraSel;
    QLabel *cameraTypeLabel;
    QComboBox *storageModeSel;
    QLabel *storageModeLabel;
    QPushButton *startVideoRecordBut;
    QPushButton *pauseBut;
    QPushButton *continueBut;
    QPushButton *screenShotBut;
    QComboBox *resolutionSel;
    QLabel *resolutionLabel;
    QPushButton *stopBut;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName("Widget");
        Widget->resize(700, 511);
        cameraSel = new QComboBox(Widget);
        cameraSel->setObjectName("cameraSel");
        cameraSel->setGeometry(QRect(60, 430, 121, 22));
        cameraTypeLabel = new QLabel(Widget);
        cameraTypeLabel->setObjectName("cameraTypeLabel");
        cameraTypeLabel->setGeometry(QRect(30, 430, 53, 15));
        storageModeSel = new QComboBox(Widget);
        storageModeSel->setObjectName("storageModeSel");
        storageModeSel->setGeometry(QRect(450, 430, 101, 22));
        storageModeLabel = new QLabel(Widget);
        storageModeLabel->setObjectName("storageModeLabel");
        storageModeLabel->setGeometry(QRect(390, 430, 53, 15));
        startVideoRecordBut = new QPushButton(Widget);
        startVideoRecordBut->setObjectName("startVideoRecordBut");
        startVideoRecordBut->setGeometry(QRect(30, 470, 75, 23));
        pauseBut = new QPushButton(Widget);
        pauseBut->setObjectName("pauseBut");
        pauseBut->setGeometry(QRect(140, 470, 75, 23));
        continueBut = new QPushButton(Widget);
        continueBut->setObjectName("continueBut");
        continueBut->setGeometry(QRect(250, 470, 75, 23));
        screenShotBut = new QPushButton(Widget);
        screenShotBut->setObjectName("screenShotBut");
        screenShotBut->setGeometry(QRect(480, 470, 75, 23));
        resolutionSel = new QComboBox(Widget);
        resolutionSel->setObjectName("resolutionSel");
        resolutionSel->setGeometry(QRect(260, 430, 101, 22));
        resolutionLabel = new QLabel(Widget);
        resolutionLabel->setObjectName("resolutionLabel");
        resolutionLabel->setGeometry(QRect(210, 430, 53, 15));
        stopBut = new QPushButton(Widget);
        stopBut->setObjectName("stopBut");
        stopBut->setGeometry(QRect(360, 470, 75, 23));

        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
        cameraTypeLabel->setText(QCoreApplication::translate("Widget", "\347\233\270\346\234\272", nullptr));
        storageModeLabel->setText(QCoreApplication::translate("Widget", "\345\255\230\345\202\250\346\250\241\345\274\217", nullptr));
        startVideoRecordBut->setText(QCoreApplication::translate("Widget", "\345\274\200\345\247\213\345\275\225\345\203\217", nullptr));
        pauseBut->setText(QCoreApplication::translate("Widget", "\346\232\202\345\201\234", nullptr));
        continueBut->setText(QCoreApplication::translate("Widget", "\347\273\247\347\273\255", nullptr));
        screenShotBut->setText(QCoreApplication::translate("Widget", "\346\210\252\345\233\276", nullptr));
        resolutionLabel->setText(QCoreApplication::translate("Widget", "\345\210\206\350\276\250\347\216\207", nullptr));
        stopBut->setText(QCoreApplication::translate("Widget", "\345\201\234\346\255\242", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
