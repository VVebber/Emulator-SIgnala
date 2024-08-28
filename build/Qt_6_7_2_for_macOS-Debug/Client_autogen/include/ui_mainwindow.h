/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.7.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QLabel *labelPort;
    QSpinBox *spinPort;
    QPushButton *ConnectToServer;
    QComboBox *TypeSignal;
    QLineEdit *lineAddres;
    QLabel *label_Ip;
    QPushButton *SendRequest;
    QLabel *Spam;
    QLabel *label;
    QGraphicsView *WeveDisplay;
    QLineEdit *lineFileLinck;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(456, 291);
        MainWindow->setStyleSheet(QString::fromUtf8("#lineAddres[state = \"0\"]{\n"
"	color: rgb(255, 74, 51);\n"
"}\n"
"#lineAddres[state = \"1\"]{\n"
"	color:rgb(102, 255, 51);\n"
"}\n"
"#WeveDisplay{\n"
"	background-color: rgb(239, 255, 172);\n"
"}"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName("groupBox");
        groupBox->setGeometry(QRect(0, 0, 201, 182));
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setObjectName("gridLayout");
        labelPort = new QLabel(groupBox);
        labelPort->setObjectName("labelPort");

        gridLayout->addWidget(labelPort, 1, 0, 1, 1);

        spinPort = new QSpinBox(groupBox);
        spinPort->setObjectName("spinPort");
        spinPort->setMinimum(1024);
        spinPort->setMaximum(65000);

        gridLayout->addWidget(spinPort, 1, 1, 1, 1);

        ConnectToServer = new QPushButton(groupBox);
        ConnectToServer->setObjectName("ConnectToServer");

        gridLayout->addWidget(ConnectToServer, 2, 1, 1, 1);

        TypeSignal = new QComboBox(groupBox);
        TypeSignal->addItem(QString());
        TypeSignal->addItem(QString());
        TypeSignal->addItem(QString());
        TypeSignal->addItem(QString());
        TypeSignal->addItem(QString());
        TypeSignal->addItem(QString());
        TypeSignal->setObjectName("TypeSignal");

        gridLayout->addWidget(TypeSignal, 4, 1, 1, 1);

        lineAddres = new QLineEdit(groupBox);
        lineAddres->setObjectName("lineAddres");

        gridLayout->addWidget(lineAddres, 0, 1, 1, 1);

        label_Ip = new QLabel(groupBox);
        label_Ip->setObjectName("label_Ip");

        gridLayout->addWidget(label_Ip, 0, 0, 1, 1);

        SendRequest = new QPushButton(groupBox);
        SendRequest->setObjectName("SendRequest");
        QFont font;
        font.setPointSize(12);
        SendRequest->setFont(font);
        SendRequest->setInputMethodHints(Qt::InputMethodHint::ImhNone);

        gridLayout->addWidget(SendRequest, 4, 0, 1, 1);

        Spam = new QLabel(groupBox);
        Spam->setObjectName("Spam");

        gridLayout->addWidget(Spam, 2, 0, 1, 1);

        label = new QLabel(groupBox);
        label->setObjectName("label");

        gridLayout->addWidget(label, 5, 0, 1, 2);

        WeveDisplay = new QGraphicsView(centralwidget);
        WeveDisplay->setObjectName("WeveDisplay");
        WeveDisplay->setEnabled(true);
        WeveDisplay->setGeometry(QRect(240, 0, 210, 210));
        lineFileLinck = new QLineEdit(centralwidget);
        lineFileLinck->setObjectName("lineFileLinck");
        lineFileLinck->setGeometry(QRect(10, 220, 421, 21));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 456, 24));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        groupBox->setTitle(QString());
        labelPort->setText(QCoreApplication::translate("MainWindow", "\320\237\320\276\321\200\321\202", nullptr));
        ConnectToServer->setText(QCoreApplication::translate("MainWindow", "Connect", nullptr));
        TypeSignal->setItemText(0, QCoreApplication::translate("MainWindow", "cos", nullptr));
        TypeSignal->setItemText(1, QCoreApplication::translate("MainWindow", "sin", nullptr));
        TypeSignal->setItemText(2, QCoreApplication::translate("MainWindow", "tan", nullptr));
        TypeSignal->setItemText(3, QCoreApplication::translate("MainWindow", "atan", nullptr));
        TypeSignal->setItemText(4, QCoreApplication::translate("MainWindow", "acos", nullptr));
        TypeSignal->setItemText(5, QCoreApplication::translate("MainWindow", "asin", nullptr));

        lineAddres->setInputMask(QCoreApplication::translate("MainWindow", "000.000.000.000;_", nullptr));
        lineAddres->setProperty("state", QVariant(QString()));
        label_Ip->setText(QCoreApplication::translate("MainWindow", "Ip", nullptr));
        SendRequest->setText(QCoreApplication::translate("MainWindow", "\320\236\320\272", nullptr));
        Spam->setText(QCoreApplication::translate("MainWindow", " ------", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "127.0.0.1:1300", nullptr));
        lineFileLinck->setText(QCoreApplication::translate("MainWindow", "/Users/malysevdanil/Documents/Projects/P2/Client/sifgnal_data2.txt", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
