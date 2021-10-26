/*
 *  SPDX-FileCopyrightText: 2009 David Hubner <hubnerd@ntlworld.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#ifndef DEVINFO
#define DEVINFO

// kde
#include <KCModule>

class QLabel;
class QGridLayout;

class DevInfoPlugin : public KCModule
{
    Q_OBJECT

public:
    explicit DevInfoPlugin(QWidget *parent, const QVariantList &);
    ~DevInfoPlugin() override;

    void updateStatus(const QString &uid);

private:
    QGridLayout *layout;
    QLabel *udiStatus;
};

#endif // DEVINFO
