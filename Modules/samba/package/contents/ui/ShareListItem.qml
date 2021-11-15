// SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
// SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>

import org.kde.kcm 1.4 as KCM
import QtQuick 2.14
import org.kde.kirigami 2.12 as Kirigami
import QtQuick.Controls 2.14 as QQC2
import QtQuick.Layouts 1.14
import org.kde.kinfocenter.samba 1.0 as Samba

// This is a slightly bespoke variant of a BasicListItem. It features equally sized labels for the share information.
Kirigami.AbstractListItem {
    id: listItem

    contentItem: RowLayout {
        spacing: LayoutMirroring.enabled ? listItem.rightPadding : listItem.leftPadding

        ColumnLayout {
            spacing: 0
            Layout.fillWidth: true

            RowLayout {
                QQC2.Label {
                    elide: Text.ElideMiddle
                    horizontalAlignment: Text.AlignLeft
                    font: Kirigami.Theme.smallFont
                    text: i18nc("@label local file system path", 'Path:')
                }

                Kirigami.UrlButton {
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignLeft
                    font: Kirigami.Theme.smallFont
                    url: ROLE_Path
                }
            }

            RowLayout {
                QQC2.Label {
                    elide: Text.ElideMiddle
                    horizontalAlignment: Text.AlignLeft
                    font: Kirigami.Theme.smallFont
                    text: i18nc("@label labels a samba url or path", 'Shared at:')
                }

                // either fully qualified url
                Kirigami.UrlButton {
                    id: link
                    visible: ROLE_ShareUrl !== undefined
                    Layout.fillWidth: true
                    horizontalAlignment: Text.AlignLeft
                    font: Kirigami.Theme.smallFont
                    url: ROLE_ShareUrl
                }

                // ... or name when we couldn't resolve a fully qualified url
                QQC2.Label {
                    visible: !link.visible
                    Layout.fillWidth: true
                    elide: Text.ElideMiddle
                    horizontalAlignment: Text.AlignLeft
                    font: Kirigami.Theme.smallFont
                    text: "/" + ROLE_Name
                }
            }
        }

        QQC2.ToolButton {
            action: Kirigami.Action {
                iconName: "document-properties"
                tooltip: xi18nc("@info:tooltip", "Open folder properties to change share settings")
                displayHint: Kirigami.Action.DisplayHint.IconOnly
                onTriggered: view.model.showPropertiesDialog(model.row)
            }

            QQC2.ToolTip {
                text: parent.action.tooltip
            }
        }
    }
}
