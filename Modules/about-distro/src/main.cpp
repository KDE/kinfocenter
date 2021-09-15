/*
    SPDX-FileCopyrightText: 2012-2021 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#include <QClipboard>
#include <QGuiApplication>
#include <QIcon>
#include <QLocale>

#include <KAboutData>
#include <KConfig>
#include <KConfigGroup>
#include <KCoreAddons>
#include <KLocalizedString>
#include <KOSRelease>
#include <KSharedConfig>

#include "CPUEntry.h"
#include "GPUEntry.h"
#include "GraphicsPlatformEntry.h"
#include "KernelEntry.h"
#include "MemoryEntry.h"
#include "PlasmaEntry.h"
#include "Version.h"

#include <KAboutData>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KQuickAddons/ConfigModule>

class KCMAboutSystem : public KQuickAddons::ConfigModule
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(KCMAboutSystem) // rule of 5
public:
    explicit KCMAboutSystem(QObject *parent, const QVariantList &args)
        : ConfigModule(parent, args)
    {
        auto aboutData = new KAboutData;
        aboutData->setComponentName(QStringLiteral("kcm_about-distro"));
        aboutData->setDisplayName(i18nc("@title", "About this System"));
        aboutData->setVersion(QByteArrayLiteral(PROJECT_VERSION));
        aboutData->setLicense(KAboutLicense::LicenseKey::GPL_V3);
        aboutData->setCopyrightStatement(i18nc("@info:credit", "Copyright 2012-2020 Harald Sitter"));
        aboutData->addAuthor(i18nc("@info:credit", "Harald Sitter"), i18nc("@info:credit", "Author"), QStringLiteral("sitter@kde.org"));
        setAboutData(aboutData);

        // https://bugs.kde.org/show_bug.cgi?id=366158
        // When a KCM loads fast enough do a blocking load via the constructor.
        // Otherwise there is a noticeable rendering gap where dummy/no data is
        // shown. Makes it look bad.
        load();
    }

    ~KCMAboutSystem() override
    {
        qDeleteAll(m_entries);
    }

    void load() override
    {
        // load is called lazily, but also from the ctor -> prevent double init.
        if (!m_entries.empty()) {
            return;
        }

        loadOSData();
        loadEntries();
    }

    void loadOSData()
    {
        // NOTE: do not include globals, otherwise kdeglobals could provide values
        //       even though we only explicitly want them from our own config.
        KSharedConfig::Ptr config = KSharedConfig::openConfig(QStringLiteral("kcm-about-distrorc"), KConfig::NoGlobals);
        KConfigGroup cg = KConfigGroup(config, "General");

        KOSRelease os;

        QString logoPath = cg.readEntry("LogoPath", os.logo());
        if (logoPath.isEmpty()) {
            logoPath = QStringLiteral("start-here-kde");
        }
        m_distroLogo = logoPath;

        // We allow overriding of the OS name for branding purposes.
        // For example OS Ubuntu may be rebranded as Kubuntu. Also Kubuntu Active
        // as a product brand is different from Kubuntu.
        const QString distroName = cg.readEntry("Name", os.name());
        const QString osrVersion = cg.readEntry("UseOSReleaseVersion", false) ? os.version() : os.versionId();
        const QString versionId = cg.readEntry("Version", osrVersion);
        // This creates a trailing space if versionId is empty, so trimming String
        // to remove possibly trailing spaces
        const QString distroNameVersion = QStringLiteral("%1 %2").arg(distroName, versionId).trimmed();
        m_distroNameVersion = distroNameVersion;

        // Insert a dummy entry for debug info dumps.
        m_entries.push_back(new Entry(ki18n("Operating System:"), distroNameVersion));

        const QString variant = cg.readEntry("Variant", os.variant());
        m_distroVariant = variant;

        const QString url = cg.readEntry("Website", os.homeUrl());
        m_distroUrl = url;

        Q_EMIT changed();
    }

    void loadEntries()
    {
        auto addEntriesToGrid = [this](QList<QObject *> *list, const std::vector<Entry *> &entries) {
            for (auto entry : entries) {
                if (!entry->isValid()) {
                    delete entry; // since we do not keep it around
                    continue;
                }
                list->append(entry);
                m_entries.push_back(entry);
            }
        };

        // software
        addEntriesToGrid(&m_softwareEntries,
                         {new PlasmaEntry(),
                          new Entry(ki18n("KDE Frameworks Version:"), KCoreAddons::versionString()),
                          new Entry(ki18n("Qt Version:"), QString::fromLatin1(qVersion())),
                          new KernelEntry(),
                          new GraphicsPlatformEntry()});

        // hardware
        addEntriesToGrid(&m_hardwareEntries, {new CPUEntry(), new MemoryEntry(), new GPUEntry()});

        Q_EMIT changed();
    }

    Q_SCRIPTABLE void copyToClipboard()
    {
        QString text;
        for (auto entry : m_entries) {
            text += entry->diagnosticLine(Entry::Language::System);
        }
        QGuiApplication::clipboard()->setText(text.trimmed());
    }

    Q_SCRIPTABLE void copyToClipboardInEnglish()
    {
        QString text;
        for (auto entry : m_entries) {
            text += entry->diagnosticLine(Entry::Language::English);
        }
        QGuiApplication::clipboard()->setText(text.trimmed());
    }

private:
    std::vector<const Entry *> m_entries;

    Q_SIGNAL void changed();

    Q_PROPERTY(QList<QObject *> softwareEntries MEMBER m_softwareEntries NOTIFY changed);
    QList<QObject *> m_softwareEntries;
    Q_PROPERTY(QList<QObject *> hardwareEntries MEMBER m_hardwareEntries NOTIFY changed);
    QList<QObject *> m_hardwareEntries;

    Q_PROPERTY(QString distroLogo MEMBER m_distroLogo NOTIFY changed);
    QString m_distroLogo;
    Q_PROPERTY(QString distroNameVersion MEMBER m_distroNameVersion NOTIFY changed);
    QString m_distroNameVersion;
    Q_PROPERTY(QString distroVariant MEMBER m_distroVariant NOTIFY changed);
    QString m_distroVariant;
    Q_PROPERTY(QString distroUrl MEMBER m_distroUrl NOTIFY changed);
    QString m_distroUrl;

    Q_PROPERTY(bool isEnglish MEMBER m_isEnglish CONSTANT);
    const bool m_isEnglish = QLocale::system().language() == QLocale::English || QLocale::system().language() == QLocale::C;
};

K_PLUGIN_CLASS_WITH_JSON(KCMAboutSystem, "about-distro.json")

#include "main.moc"
