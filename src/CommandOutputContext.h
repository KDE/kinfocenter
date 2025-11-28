/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2021-2022 Harald Sitter <sitter@kde.org>
    SPDX-FileCopyrightText: 2025 Thomas Duckworth <tduck@filotimoproject.org>
*/

#pragma once

#include <QMap>
#include <QObject>
#include <QTimer>
#include <QUrl>
#include <QtQmlIntegration>

// Somewhat general-purpose command executor. This class runs the executable with arguments, collecting all its output
// and potentially filtering it to limit the lines to only ones matching.
class CommandOutputContext : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_UNCREATABLE("Only ever passed in from C++")
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
    // Extra explanatory text for error conditions. Empty when no explanatory text has been specified.
    Q_PROPERTY(QString explanation MEMBER m_explanation NOTIFY explanationChanged)
    // URL where the user can report a bug when there is an error. Empty when there is no error, or no applicable place to report a bug
    Q_PROPERTY(QUrl bugReportUrl MEMBER m_bugReportUrl CONSTANT)
    // Text styling, which matters for filtering purposes.
    Q_PROPERTY(Qt::TextFormat textFormat MEMBER m_format CONSTANT)
    // Interval in milliseconds at which to automatically refresh the output. 0 means no refreshing.
    Q_PROPERTY(int autoRefreshMs MEMBER m_autoRefreshMs WRITE setAutoRefreshMs NOTIFY autoRefreshMsChanged)
    // If automatic refreshing is enabled. Setting an interval will automatically enable this.
    Q_PROPERTY(bool autoRefresh READ autoRefresh WRITE setAutoRefresh NOTIFY autoRefreshChanged)
public:
    CommandOutputContext(const QStringList &findExecutables,
                         const QString &executable,
                         const QStringList &arguments,
                         Qt::TextFormat format,
                         QObject *parent = nullptr);
    CommandOutputContext(const QStringList &findExecutables, const QString &executable, const QStringList &arguments, QObject *parent = nullptr);
    CommandOutputContext(const QString &executable, const QStringList &arguments, QObject *parent = nullptr);

    QString executableName() const;
    QStringList arguments() const;

    QString filter() const;
    void setFilter(const QString &filter);
    Q_SIGNAL void filterChanged();

    bool autoRefresh() const;
    void setAutoRefresh(bool autoRefresh);
    Q_SIGNAL void autoRefreshChanged();

    void setAutoRefreshMs(int ms);
    Q_SIGNAL void autoRefreshMsChanged();

    Q_SLOT void refresh();

    void setTrimAllowed(bool allow);

Q_SIGNALS:
    void textChanged();
    void readyChanged();
    void errorChanged();
    void explanationChanged();

private:
    void reset();
    void load();
    void runProcess();
    void setError(const QString &message, const QString &explanation);
    void setReady();

    const QString m_executableName;
    QString m_executablePath;
    QMap<QString, QString> m_foundExecutablePaths;
    const QStringList m_arguments;
    const QUrl m_bugReportUrl;

    QStringList m_originalLines;

    bool m_trimAllowed = true;
    bool m_ready = false;
    QString m_error;
    QString m_explanation;

    QString m_text; // possibly filtered
    QString m_filter;

    QTimer *m_autoRefreshTimer = nullptr;
    int m_autoRefreshMs = 0;

    Qt::TextFormat m_format;
    QString m_newlineIdentifier;
};
