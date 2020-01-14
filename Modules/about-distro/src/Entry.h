/*
    SPDX-FileCopyrightText: 2012-2020 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef ENTRY_H
#define ENTRY_H

#include <QString>
#include <KLocalizedString>

// Generic dumpable info entry.
// This encapsulates a table entry so that it may be put into the UI
// and also serialized into textual form for copy to clipboard.
// All entries that are meant to be serializable should derive from this!
// This class may either be subclassed or used as-is if label/value are trivial
// to obtain.
class Entry
{
public:
    enum class Language {
        System,
        English
    };

    Entry(const KLocalizedString &label_, const QString &value_);
    virtual ~Entry();

    // When false this entry is garbage (e.g. incomplete data) and shouldn't be rendered.
    bool isValid() const;

    // Returns textual representation of entry.
    QString diagnosticLine(Language language = Language::System) const;

    // Descriptive label
    KLocalizedString label;
    // Value of the entry (e.g. the version of plasma)
    QString value;

private:
    QString localizedLabel(Language language) const;
};

#endif // ENTRY_H
