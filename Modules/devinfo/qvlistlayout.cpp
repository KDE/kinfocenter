/*
 *  soldevicetypes.cpp
 *
 *  Copyright (C) 2009 David Hubner <hubnerd@ntlworld.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include "qvlistlayout.h"

#include <QStringList>
#include <QLabel>

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
