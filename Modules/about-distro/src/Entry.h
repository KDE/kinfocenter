/*
    SPDX-FileCopyrightText: 2012-2020 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef ENTRY_H
#define ENTRY_H

#include <QString>
#include <QLocale>
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
        English,
    };

    // value may be empty if localizedValue is overridden
    Entry(const KLocalizedString &label_, const QString &value_);
    virtual ~Entry();

    // When false this entry is garbage (e.g. incomplete data) and shouldn't be rendered.
    bool isValid() const;

    // Returns textual representation of entry.
    QString diagnosticLine(Language language = Language::System) const;

    QString localizedLabel(Language language = Language::System) const;

    // Returns the value by default. Needs to be overridden in subclasses if localization
    // is needed for the value.
    virtual QString localizedValue(Language language = Language::System) const;

protected:
    // Returns localized QString for the given language.
    QString localize(const KLocalizedString &string, Language language) const;

    // Returns a QLocale for the given language.
    QLocale localeForLanguage(Language language) const;

    // Descriptive label
    KLocalizedString m_label;
    // Value of the entry (e.g. the version of plasma)
    QString m_value;
};

#endif // ENTRY_H
