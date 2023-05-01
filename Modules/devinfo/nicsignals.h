/*
 *  SPDX-FileCopyrightText: 2009 David Hubner <hubnerd@ntlworld.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#ifndef NICSIGNALS
#define NICSIGNALS

// Qt
#include <QObject>

class NicSignals : public QObject
{
    Q_OBJECT

public:
    NicSignals();

private:
    void connectToNicSignals();

Q_SIGNALS:
    void nicActivatedOrDisconnected();

public slots:
    void nicChangedSignal(int, int, int);
};

#endif // NICSIGNALS
