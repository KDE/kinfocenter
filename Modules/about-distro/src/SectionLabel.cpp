/*
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "SectionLabel.h"

SectionLabel::SectionLabel(const QString &text, QWidget *parent)
    : QLabel(text, parent)
{
    QFont font;
    font.setBold(true);
    font.setWeight(75);
    setFont(font);
}
