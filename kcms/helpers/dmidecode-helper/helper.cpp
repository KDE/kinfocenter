// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2021-2022 Harald Sitter <sitter@kde.org>
// SPDX-FileCopyrightText: 2024 Kristen McWilliam <kmcwilliampublic@gmail.com>

#include "helper.h"

#include <QDebug>
#include <QProcess>
#include <QStandardPaths>
#include <array>

#include <KAuth/HelperSupport>

template<typename Output, typename... Input>
auto make_array(Input &&...args) -> std::array<Output, sizeof...(args)> // NB: we need suffix notation here so args is defined
{
    return {std::forward<Input>(args)...};
}

DMIDecodeHelper::DMIDecodeHelper(QObject *parent)
    : QObject(parent)
{
    // PATH is super minimal when invoked through dbus
    setenv("PATH", "/usr/sbin:/sbin:/usr/local/sbin", 1);

    m_dmidecodePath = QStandardPaths::findExecutable("dmidecode");
}

KAuth::ActionReply DMIDecodeHelper::memoryinformation(const QVariantMap &args)
{
    Q_UNUSED(args);

    KAuth::ActionReply reply;
    auto result = executeDmidecode({QStringLiteral("--type"), QStringLiteral("17")});

    if (result.failed()) {
        qWarning() << "DMIDecodeHelper: Unable to get memory information";
        return KAuth::ActionReply::HelperErrorReply();
    }

    const QString output = result.data().value("result").toString();
    reply.addData("memory", output);

    return reply;
}

KAuth::ActionReply DMIDecodeHelper::systeminformation(const QVariantMap &args)
{
    Q_UNUSED(args);

    // NB: Microsoft also outlines a limited set of DMI values to be required for IOT OEM licensing, as such we
    //   can rely on the same fields to have sound content . Since this only applies to OEMs we still need to filter
    //   out dummy values though and because of that we can grab more fields, since we'll filter them anyway.
    // https://docs.microsoft.com/en-us/windows-hardware/manufacture/iot/license-requirements?view=windows-11#smbios-support

    KAuth::ActionReply reply;

    const auto keys = {QStringLiteral("system-manufacturer"),
                       QStringLiteral("system-product-name"),
                       QStringLiteral("system-version"),
                       QStringLiteral("system-serial-number")};

    for (const auto &key : keys) {
        auto result = executeDmidecode({QStringLiteral("--string"), key});
        if (result.failed()) {
            qWarning() << "DMIDecodeHelper: Unable to get system information for " << key;
            // We don't want to fail the entire action if we can't get a single piece of information.
            continue;
        }

        const auto output = result.data().value("result").toString();

        // Fairly exhaustive filter list based on a dozen or so samples gathered from reddit and other places.
        // These are values that may appear in the DMI system information but aren't really useful.
        static const auto dummyData = make_array<QString>(QStringLiteral("system version"),
                                                          QStringLiteral("system product name"),
                                                          QStringLiteral("system serial number"),
                                                          QStringLiteral("system manufacturer"),
                                                          QStringLiteral("to be filled by o.e.m."),
                                                          QStringLiteral("standard"), /* sometimes the version is useless */
                                                          QStringLiteral("sku"),
                                                          QStringLiteral("default string"),
                                                          QStringLiteral("not specified"),
                                                          QStringLiteral("not applicable")
                                                          /* may also be empty, but that is filtered above already */);

        if (std::find(dummyData.cbegin(), dummyData.cend(), output.toLower()) != dummyData.cend()) {
            continue;
        }

        reply.addData(key, output);
    }

    if (reply.data().isEmpty()) {
        qWarning() << "DMIDecodeHelper: Unable to get system information";
        // If we didn't get any data, we should fail the action.
        return KAuth::ActionReply::HelperErrorReply();
    }

    return reply;
}

KAuth::ActionReply DMIDecodeHelper::executeDmidecode(const QStringList &arguments)
{
    if (m_dmidecodePath.isEmpty()) {
        return KAuth::ActionReply::HelperErrorReply();
    }

    QProcess proc;
    proc.start(m_dmidecodePath, arguments);
    proc.waitForFinished();
    const QByteArray output = proc.readAllStandardOutput().trimmed();

    if (output.isEmpty() || proc.error() != QProcess::UnknownError || proc.exitStatus() != QProcess::NormalExit) {
        return KAuth::ActionReply::HelperErrorReply();
    }

    KAuth::ActionReply reply;
    reply.addData("result", output);
    return reply;
}

KAUTH_HELPER_MAIN("org.kde.kinfocenter.dmidecode", DMIDecodeHelper)

#include "moc_helper.cpp"
