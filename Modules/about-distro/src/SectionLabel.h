/*
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef SECTIONLABEL_H
#define SECTIONLABEL_H

#include <QLabel>

// Label with font styling for section heading such as 'Software'
class SectionLabel : public QLabel
{
public:
    explicit SectionLabel(const QString &text, QWidget *parent = nullptr);
};

#endif // SECTIONLABEL_H
