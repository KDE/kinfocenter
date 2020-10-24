/*
 *   SPDX-FileCopyrightText: 2015 Pinak Ahuja <pinak.ahuja@gmail.com>
 *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 */

#ifndef KCM_FILEINDEXER_H
#define KCM_FILEINDEXER_H

#include <KQuickAddons/ConfigModule>

class KCMFileIndexer : public KQuickAddons::ConfigModule
{
    Q_OBJECT

public:
    explicit KCMFileIndexer(QObject *parent, const QVariantList &args);
    virtual ~KCMFileIndexer() = default;
};

Q_DECLARE_METATYPE(QList<QPointF>)

#endif // KCM_FILEINDEXER_H
