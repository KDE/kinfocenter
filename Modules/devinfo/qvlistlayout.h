/*
 *  SPDX-FileCopyrightText: 2009 David Hubner <hubnerd@ntlworld.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#ifndef QVLISTLAYOUT
#define QVLISTLAYOUT

#include <QVBoxLayout>

class QVListLayout : public QVBoxLayout
{
public:
    QVListLayout();
    void applyQListToLayout(const QStringList &);
};

#endif // QVLISTLAYOUT
