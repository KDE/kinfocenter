/*
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
    SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>
*/

#include "CommandOutputContext.h"

#include <QDebug>
#include <QProcess>
#include <QStandardPaths>

#include <KLocalizedString>

CommandOutputContext::CommandOutputContext(const QString &executable, const QStringList &arguments, QObject *parent)
    : QObject(parent)
    , m_executableName(executable)
    , m_executablePath(QStandardPaths::findExecutable(m_executableName))
    , m_arguments(arguments)
{
    metaObject()->invokeMethod(this, &CommandOutputContext::load);
}

QString CommandOutputContext::executableName() const
{
    return m_executableName;
}

QStringList CommandOutputContext::arguments() const
{
    return m_arguments;
}

QString CommandOutputContext::filter() const
{
    return m_filter;
}

void CommandOutputContext::setFilter(const QString &filter)
{
    m_filter = filter;
    if (m_filter.isEmpty()) {
        m_text = m_originalLines.join('\n');
    } else {
        m_text.clear();
        for (const QString &line : m_originalLines) {
            if (line.contains(filter, Qt::CaseInsensitive)) {
                m_text += line + '\n';
            }
        }
    }
    Q_EMIT textChanged();
    Q_EMIT filterChanged();
}

void CommandOutputContext::classBegin()
{
    m_componentComplete = false;
}

void CommandOutputContext::componentComplete()
{
    m_componentComplete = true;
    load();
}

void CommandOutputContext::reset()
{
    m_ready = false;
    m_error.clear();
    m_text.clear();
    m_filter.clear();
    Q_EMIT readyChanged();
    Q_EMIT errorChanged();
    Q_EMIT textChanged();
    Q_EMIT filterChanged();

    // Not exposed as properties
    m_originalLines.clear();
}

void CommandOutputContext::load()
{
    if (!m_componentComplete) {
        return;
    }

    reset();

    if (m_executablePath.isEmpty()) {
        setError(xi18nc("@info", "The executable <command>%1</command> could not be found in $PATH.", m_executableName));
        return;
    }

    auto proc = new QProcess;
    proc->setProcessChannelMode(QProcess::MergedChannels);
    connect(proc, &QProcess::finished, this, [this, proc](int /* exitCode */, QProcess::ExitStatus exitStatus) {
        proc->deleteLater();

        switch (exitStatus) {
        case QProcess::CrashExit:
            return setError(xi18nc("@info", "The subprocess <command>%1</command> crashed unexpectedly. No data could be obtained.", m_executableName));
        case QProcess::NormalExit:
            break;
        }

        m_text = QString::fromLocal8Bit(proc->readAllStandardOutput());
        m_originalLines = m_text.split('\n');
        if (!m_filter.isEmpty()) {
            // re-apply filter on new text
            setFilter(m_filter);
        }

        Q_EMIT textChanged();
        setReady();
    });
    proc->start(m_executablePath, m_arguments);
}

void CommandOutputContext::setError(const QString &message)
{
    m_error = message;
    Q_EMIT errorChanged();
    setReady();
}

void CommandOutputContext::setReady()
{
    m_ready = true;
    Q_EMIT readyChanged();
}
