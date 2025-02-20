/*
    SPDX-FileCopyrightText: 2012-2022 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef ENTRY_H
#define ENTRY_H

#include <KLocalizedString>
#include <QLocale>
#include <QObject>
#include <QString>

// A hint is short additional information for an Entry. It may be color coded as well.
// Hints annotate the Entry with extra info. Notable example are GPUEntry which may
// have a hint about the type of GPU (integrated vs. discrete).
class Hint
{
public:
    enum class Color {
        One,
        Two,
        Three
    };
    Q_ENUM(Color)

private:
    Q_GADGET
    Q_PROPERTY(QString text MEMBER m_text CONSTANT)
    Q_PROPERTY(Color color MEMBER m_color CONSTANT)
public:
    QString m_text;
    Color m_color = Hint::Color::One;
};

// Generic dumpable info entry.
// This encapsulates a table entry so that it may be put into the UI
// and also serialized into textual form for copy to clipboard.
// All entries that are meant to be serializable should derive from this!
// This class may either be subclassed or used as-is if label/value are trivial
// to obtain.
class Entry : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(Entry)
public:
    enum class Language {
        System,
        English,
    };
    Q_ENUM(Language)

    enum class Hidden {
        No = false,
        Yes = true,
    };
    Q_ENUM(Hidden)

    // value may be empty if localizedValue is overridden
    Entry(const KLocalizedString &label_, const QString &value_, Hidden hidden = Hidden::No);
    ~Entry() override;

    // When false this entry is garbage (e.g. incomplete data) and shouldn't be rendered.
    bool isValid() const;

    // Returns textual representation of entry.
    QString diagnosticLine(Language language = Language::System) const;

    Q_SCRIPTABLE virtual QString localizedLabel(Language language = Language::System) const;

    // Returns the value by default. Needs to be overridden in subclasses if localization
    // is needed for the value.
    Q_SCRIPTABLE virtual QString localizedValue(Language language = Language::System) const;

    // Returns whether this Entry should be hidden by default (i.e. only shown upon user request)
    Q_INVOKABLE virtual bool isHidden() const;

    // Returns a hint for the user to consider when interpreting the value.
    Q_INVOKABLE [[nodiscard]] virtual Hint localizedHint(Language language = Language::System) const;

    // Returns a help string for the entry, shown with a ContextualHelpButton
    Q_SCRIPTABLE [[nodiscard]] virtual QString localizedHelp(Language language = Language::System) const;

protected:
    // Returns localized QString for the given language.
    QString localize(const KLocalizedString &string, Language language) const;

    // Returns a QLocale for the given language.
    QLocale localeForLanguage(Language language) const;

    // Descriptive label
    KLocalizedString m_label;
    // Value of the entry (e.g. the version of plasma)
    QString m_value;
    // Entries may be hidden behind a button so they aren't visible by default.
    Hidden m_hidden;
};

#endif // ENTRY_H
