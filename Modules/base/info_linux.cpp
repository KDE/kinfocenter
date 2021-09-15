/*
    SPDX-FileCopyrightText: 1998 Helge Deller <deller@gmx.de>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "config-infocenter.h"
#include <ctype.h>
#include <linux/kernel.h>
#include <stdio.h>
#include <sys/stat.h>
#include <syscall.h>
#include <unistd.h>

#include <QFile>
#include <QTextStream>
#include <QTreeWidget>

#include <KLocalizedString>
#include <QFontDatabase>

#include <algorithm>

#define INFO_DMA "/proc/dma"

#define INFO_IOPORTS "/proc/ioports"

#define INFO_MISC "/proc/misc"

#define MAXCOLUMNWIDTH 600

bool GetInfo_ReadfromFile(QTreeWidget *tree, const char *FileName, const QChar &splitChar)
{
    bool added = false;
    QFile file(QString::fromLatin1(FileName));

    if (!file.exists()) {
        return false;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    QTextStream stream(&file);

    QString line = stream.readLine();

    while (!line.isNull()) {
        QString s1, s2;
        if (!line.isEmpty()) {
            if (!splitChar.isNull()) {
                int pos = line.indexOf(splitChar);
                s1 = line.left(pos - 1).trimmed();
                s2 = line.mid(pos + 1).trimmed();
            } else
                s1 = line;
        }
        QStringList list;
        list << s1 << s2;
        new QTreeWidgetItem(tree, list);
        added = true;
        line = stream.readLine();
    }

    file.close();

    return added;
}

bool GetInfo_DMA(QTreeWidget *tree)
{
    QFile file(QLatin1String(INFO_DMA));

    QStringList headers;
    headers << i18n("DMA-Channel") << i18n("Used By");
    tree->setHeaderLabels(headers);

    if (file.exists() && file.open(QIODevice::ReadOnly)) {
        QTextStream stream(&file);
        QString line;

        line = stream.readLine();
        while (!line.isNull()) {
            if (!line.isEmpty()) {
                // line is e.g. " 4: cascade"
                QStringList list = line.split(QLatin1Char(':'), Qt::KeepEmptyParts, Qt::CaseInsensitive);

                if (list.size() != 2) {
                    continue;
                }

                std::for_each(list.begin(), list.end(), [](QString &str) {
                    str = str.trimmed();
                });

                new QTreeWidgetItem(tree, list);
            }
            line = stream.readLine();
        }
        file.close();
    } else {
        return false;
    }

    return true;
}

bool GetInfo_IO_Ports(QTreeWidget *tree)
{
    QStringList headers;
    headers << i18n("I/O-Range") << i18n("Used By");
    tree->setHeaderLabels(headers);
    return GetInfo_ReadfromFile(tree, INFO_IOPORTS, QLatin1Char(':'));
}
