import org.kde.kcm 1.2 as KCM
import QtQuick.Layouts 1.2
import QtQuick 2.7
import QtQuick.Controls 2.2 as Controls
import org.kde.kirigami 2.10 as Kirigami

KCM.SimpleKCM {
    title: i18n("System Information")

    Kirigami.Theme.colorSet: Kirigami.Theme.Window

    // This only works in plasma-settings
    actions.contextualActions: [
        Kirigami.Action {
            text: i18n("Copy to clipboard")
            icon.name: "edit-copy"
            onTriggered: copyInfoToClipboard()
        }
    ]

    ColumnLayout {
        width: parent.width
        spacing: Kirigami.Units.largeSpacing

        Kirigami.Icon {
            Layout.alignment: Qt.AlignHCenter
            width: Kirigami.Units.iconSizes.huge
            height: width
            source: kcm.distroInfo.logo ? kcm.distroInfo.logo : "kde"
        }
        Kirigami.Heading {
            Layout.alignment: Qt.AlignHCenter
            text: kcm.distroInfo.name + " " + kcm.distroInfo.versionId
        }

        Kirigami.UrlButton {
            Layout.alignment: Qt.AlignHCenter
            text: kcm.distroInfo.homeUrl
            url: kcm.distroInfo.homeUrl
        }

        Kirigami.Heading {
            level: 2
            Layout.alignment: Qt.AlignHCenter
            text: i18n("Software")
        }
        Kirigami.FormLayout {
            id: softwareLayout
            twinFormLayouts: [hardwareLayout]

            Controls.Label {
                Kirigami.FormData.label: i18n("KDE Plasma Version:")
                text: kcm.softwareInfo.plasmaVersion
                visible: kcm.softwareInfo.plasmaVersion
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("KDE Frameworks Version:")
                text: kcm.softwareInfo.frameworksVersion
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("Qt Version:")
                text: kcm.softwareInfo.qtVersion
                visible: kcm.softwareInfo.qtVersion
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("Kernel Version:")
                text: kcm.softwareInfo.kernelRelease
                visible: kcm.softwareInfo.kernelRelease
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("OS Type:")
                text: i18nc("@label %1 is the CPU bit width (e.g. 32 or 64)", "%1-bit", kcm.softwareInfo.osType)
                visible: kcm.softwareInfo.osType
            }
        }
        Kirigami.Heading {
            level: 2
            Layout.alignment: Qt.AlignHCenter
            text: i18n("Hardware")
        }
        Kirigami.FormLayout {
            id: hardwareLayout
            twinFormLayouts: [softwareLayout]

            Controls.Label {
                Kirigami.FormData.label: i18np("Processor:", "Processors:", kcm.hardwareInfo.processorCount);
                text: kcm.hardwareInfo.processors
            }
            Controls.Label {
                Kirigami.FormData.label: i18nc("@label", "Graphics Processor:")
                text: kcm.hardwareInfo.gpu
            }
            Controls.Label {
                Kirigami.FormData.label: i18n("Memory:")
                text: {
                    if (kcm.hardwareInfo.memory !== "0 B") {
                        return i18nc("@label %1 is the formatted amount of system memory (e.g. 7,7 GiB)",
                              "%1 of RAM", kcm.hardwareInfo.memory)
                    } else {
                        return i18nc("Unknown amount of RAM", "Unknown")
                    }
                }
            }
        }
    }
    // Only display when we can't use Kirigami Actions
    footer: Controls.Button {
        visible: !Kirigami.Settings.isMobile
        implicitHeight: visible ? Kirigami.Units.gridUnit * 2 : 0
        Layout.alignment: Qt.AlignLeft | Qt.AlignBottom

        text: i18n("Copy to clipboard")
        icon.name: "edit-copy"
        onClicked: kcm.copyToClipboard()
    }
}
