/*
    SPDX-FileCopyrightText: 2012-2020 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef MODULE_H
#define MODULE_H

#include <KCModule>

namespace Ui {
    class Module;
}

class Entry;

class Module : public KCModule
{
    Q_OBJECT
public:
    /**
     * Constructor.
     *
     * @param parent Parent widget of the module
     * @param args Arguments for the module
     */
    explicit Module(QWidget *parent, const QVariantList &args = QVariantList());

    /**
     * Destructor.
     */
    ~Module() override;

    /**
     * Overloading the KCModule load() function.
     */
    void load() override;

    /**
     * Overloading the KCModule save() function.
     */
    void save() override;

    /**
     * Overloading the KCModule defaults() function.
     */
    void defaults() override;

private:
    // Load os-release data into UI.
    void loadOSData();
    // Load generic entries into UI.
    void loadEntries();

    // Copy data dump to clipboard
    void copyToClipboard();
    // Same as copyToClipboard but in en_US when the system language
    // is something else
    void copyToClipboardInEnglish();

    /**
     * UI
     */
    Ui::Module *ui = nullptr;

    /*** Description entries for dumping into textual form. Already excludes invalids. */
    std::vector<const Entry *> m_entries;
};

#endif // MODULE_H
