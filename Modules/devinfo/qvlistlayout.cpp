/*
 *  SPDX-FileCopyrightText: 2009 David Hubner <hubnerd@ntlworld.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#include "qvlistlayout.h"

#include <QLabel>
#include <QStringList>

QVListLayout::QVListLayout()
    : QVBoxLayout()
{
}

void QVListLayout::applyQListToLayout(const QStringList &list)
{
    bool toggle = true;

    QLabel *bLabel;
    QFont labelFont;
    labelFont.setBold(true);

    for (const QString &item : list) {
        if (!item.isEmpty()) {
            bLabel = new QLabel(item);
            bLabel->setWordWrap(true);
            if (bLabel->text() != QLatin1String("--")) {
                if (toggle) {
                    toggle = false;
                    bLabel->setFont(labelFont);
                } else {
                    bLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
                    bLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
                    bLabel->setAlignment(Qt::AlignTop);
                    toggle = true;
                }
            } else {
                bLabel->setText(QLatin1String(""));
            }
            addWidget(bLabel);
        }
    }
}
