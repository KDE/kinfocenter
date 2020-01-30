/*
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef SMBMOUNTMODEL_H
#define SMBMOUNTMODEL_H

#include <QAbstractListModel>
#include <QList>
#include <Solid/Device>

/**
 * Model of cifs mount entires. Implementing properties
 * as columns rather than roles.
 */
class SmbMountModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum class ColumnRole {
        Share,
        Path,
        Accessible,
        ColumnCount, // End marker
    };

    explicit SmbMountModel(QObject *parent = nullptr);
    ~SmbMountModel() override;

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool hasChildren(const QModelIndex &parent) const override;

private slots:
    void addDevice(const QString &udi);
    void removeDevice(const QString &udi);
    void reloadData();

private:
    inline QList<Solid::Device>::const_iterator deviceForUdi(const QString &udi) const
    {
        return std::find_if(m_devices.constBegin(), m_devices.constEnd(),
                            [&udi](const Solid::Device &device) {
            return device.udi() == udi;
        });
    }

    QList<Solid::Device> m_devices;
};

#endif // SMBMOUNTMODEL_H
