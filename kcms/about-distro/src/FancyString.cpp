/*
    SPDX-FileCopyrightText: 2012-2020 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include "FancyString.h"

namespace FancyString
{
QString fromUgly(const QString &string)
{
    QString ugly(string);
    ugly.replace(QStringLiteral("(TM)"), QChar(8482));
    ugly.replace(QStringLiteral("(R)"), QChar(174));
    return ugly.trimmed();
}

QString fromRenderer(const QString &renderer_)
{
    QString renderer = renderer_;
    renderer = fromUgly(renderer);
    // It seems the renderer value may have excess information in parentheses ->
    // strip that.
    renderer = renderer.mid(0, renderer.indexOf('('));
    // Leads to trailing space in my case, don't know whether that is happening
    // everywhere, though. Thus removing trailing spaces separately.
    renderer = renderer.trimmed();
    return renderer;
}

} // namespace FancyString
