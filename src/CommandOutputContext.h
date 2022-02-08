/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>
*/

#pragma once

#include <QObject>

// Somewhat general-purpose command executor. This class runs the executable with arguments, collecting all its output
// and potentially filtering it to limit the lines to only ones matching.
class CommandOutputContext : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString executable READ executableName CONSTANT)
    Q_PROPERTY(QStringList arguments READ arguments CONSTANT)
    // Output. With filter applied.
    Q_PROPERTY(QString text MEMBER m_text NOTIFY textChanged)
    // Filter string. Case in-sensitive. If lines do not contain the filter string they'll be removed from text.
    Q_PROPERTY(QString filter READ filter WRITE setFilter NOTIFY filterChanged)
    // Ready when the underlying process has terminated
    Q_PROPERTY(bool ready MEMBER m_ready NOTIFY readyChanged)
    // Potential error description. Empty when there is no error to report.
    Q_PROPERTY(QString error MEMBER m_error NOTIFY errorChanged)
public:
    explicit CommandOutputContext(const QString &executable, const QStringList &arguments, QObject *parent = nullptr);

    QString executableName() const;
    QStringList arguments() const;

    QString filter() const;
    void setFilter(const QString &filter);
    Q_SIGNAL void filterChanged();

Q_SIGNALS:
    void textChanged();
    void readyChanged();
    void errorChanged();

private:
    void reset();
    void load();
    void setError(const QString &message);
    void setReady();

    const QString m_executableName;
    QString m_executablePath;
    const QStringList m_arguments;

    QStringList m_originalLines;

    bool m_ready = false;
    QString m_error;

    QString m_text; // possibly filtered
    QString m_filter;
};
