/********************************************************************************
** Form generated from reading UI file 'appendmoduledialog.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_APPENDMODULEDIALOG_H
#define UI_APPENDMODULEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_AppendModuleDialog
{
public:
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_3;
    QListWidget *listModules;
    QListWidget *listFunctions;
    QHBoxLayout *horizontalLayout_2;
    QPushButton *buttonSearch;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *buttonSelectAll;
    QPushButton *buttonDeselect;
    QPushButton *buttonSelectInvert;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *buttonAdd;
    QPushButton *buttonCancel;

    void setupUi(QDialog *AppendModuleDialog)
    {
        if (AppendModuleDialog->objectName().isEmpty())
            AppendModuleDialog->setObjectName(QStringLiteral("AppendModuleDialog"));
        AppendModuleDialog->resize(602, 431);
        verticalLayoutWidget = new QWidget(AppendModuleDialog);
        verticalLayoutWidget->setObjectName(QStringLiteral("verticalLayoutWidget"));
        verticalLayoutWidget->setGeometry(QRect(30, 20, 511, 291));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        listModules = new QListWidget(verticalLayoutWidget);
        listModules->setObjectName(QStringLiteral("listModules"));

        horizontalLayout_3->addWidget(listModules);

        listFunctions = new QListWidget(verticalLayoutWidget);
        listFunctions->setObjectName(QStringLiteral("listFunctions"));

        horizontalLayout_3->addWidget(listFunctions);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        buttonSearch = new QPushButton(verticalLayoutWidget);
        buttonSearch->setObjectName(QStringLiteral("buttonSearch"));

        horizontalLayout_2->addWidget(buttonSearch);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        buttonSelectAll = new QPushButton(verticalLayoutWidget);
        buttonSelectAll->setObjectName(QStringLiteral("buttonSelectAll"));

        horizontalLayout_2->addWidget(buttonSelectAll);

        buttonDeselect = new QPushButton(verticalLayoutWidget);
        buttonDeselect->setObjectName(QStringLiteral("buttonDeselect"));

        horizontalLayout_2->addWidget(buttonDeselect);

        buttonSelectInvert = new QPushButton(verticalLayoutWidget);
        buttonSelectInvert->setObjectName(QStringLiteral("buttonSelectInvert"));

        horizontalLayout_2->addWidget(buttonSelectInvert);


        verticalLayout->addLayout(horizontalLayout_2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        buttonAdd = new QPushButton(verticalLayoutWidget);
        buttonAdd->setObjectName(QStringLiteral("buttonAdd"));

        horizontalLayout->addWidget(buttonAdd);

        buttonCancel = new QPushButton(verticalLayoutWidget);
        buttonCancel->setObjectName(QStringLiteral("buttonCancel"));

        horizontalLayout->addWidget(buttonCancel);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(AppendModuleDialog);

        QMetaObject::connectSlotsByName(AppendModuleDialog);
    } // setupUi

    void retranslateUi(QDialog *AppendModuleDialog)
    {
        AppendModuleDialog->setWindowTitle(QApplication::translate("AppendModuleDialog", "\320\224\320\276\320\261\320\260\320\262\320\270\321\202\321\214 \320\274\320\276\320\264\321\203\320\273\321\214", Q_NULLPTR));
        buttonSearch->setText(QApplication::translate("AppendModuleDialog", "\320\237\320\276\320\270\321\201\320\272", Q_NULLPTR));
        buttonSelectAll->setText(QApplication::translate("AppendModuleDialog", "\320\222\321\213\320\264\320\265\320\273\320\270\321\202\321\214 \320\262\321\201\320\265", Q_NULLPTR));
        buttonDeselect->setText(QApplication::translate("AppendModuleDialog", "\320\241\320\275\321\217\321\202\321\214 \320\262\321\213\320\264\320\265\320\273\320\265\320\275\320\270\320\265", Q_NULLPTR));
        buttonSelectInvert->setText(QApplication::translate("AppendModuleDialog", "\320\230\320\275\320\262\320\265\321\200\321\202\320\270\321\200\320\276\320\262\320\260\321\202\321\214 \320\262\321\213\320\261\320\276\321\200", Q_NULLPTR));
        buttonAdd->setText(QApplication::translate("AppendModuleDialog", "\320\224\320\276\320\261\320\260\320\262\320\270\321\202\321\214", Q_NULLPTR));
        buttonCancel->setText(QApplication::translate("AppendModuleDialog", "\320\236\321\202\320\274\320\265\320\275\320\270\321\202\321\214", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class GSAppendModuleDialog: public Ui_AppendModuleDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_APPENDMODULEDIALOG_H
