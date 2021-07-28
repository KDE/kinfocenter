/*
    SPDX-FileCopyrightText: 1998 Helge Deller <deller@gmx.de>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef _INFO_H_
#define _INFO_H_

#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QTreeWidget>
#include <QWidget>

#include <KAboutData>
#include <KCModule>

#include "os_current.h"

#include <KPluginFactory>
#include <KLocalizedString>

#define CREATE_FACTORY(type, name, json)                                                                                                                       \
    class K##type##InfoWidget : public KInfoListWidget                                                                                                         \
    {                                                                                                                                                          \
    public:                                                                                                                                                    \
        K##type##InfoWidget(QWidget *parent, const QVariantList &)                                                                                             \
            : KInfoListWidget(name, parent, GetInfo_##type)                                                                                                    \
        {                                                                                                                                                      \
        }                                                                                                                                                      \
    };                                                                                                                                                         \
    K_PLUGIN_CLASS_WITH_JSON(K##type##InfoWidget, json)

class KInfoListWidget : public KCModule
{
public:
    KInfoListWidget(const QString &_title, QWidget *parent, bool _getlistbox(QTreeWidget *) = nullptr);

    void load() override;
    QString quickHelp() const override;

private:
    QTreeWidget *tree;
    bool (*getlistbox)(QTreeWidget *);
    QString title;

    QLabel *noInfoText;
    QString errorString;
    QStackedWidget *widgetStack;
};

#endif
