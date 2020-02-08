/*
    SPDX-FileCopyrightText: 2020 Harald Sitter <sitter@kde.org>
    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

#ifndef KSAMBASHAREMODEL_H
#define KSAMBASHAREMODEL_H

#include <QAbstractListModel>
#include <KIOCore/KSambaShareData>

/**
 * Model of ksamabasharedata. Implementing properties
 * as columns rather than roles.
 */
class KSambaShareModel : public QAbstractListModel
{
Q_OBJECT
public:
    enum class ColumnRole {
        Name,
        Path,
        Comment,
        ColumnCount, // End marker
    };

    explicit KSambaShareModel(QObject *parent = nullptr);
    ~KSambaShareModel() override;

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool hasChildren(const QModelIndex &parent) const override;

public slots:
    void reloadData();

private:
    QList<KSambaShareData> m_list;
};

#endif // KSAMBASHAREMODEL_H
