/*
 *  infopanel.h
 *
 *  Copyright (C) 2009 David Hubner <hubnerd@ntlworld.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#ifndef INFOPANEL
#define INFOPANEL

// QT
#include <QGroupBox>

//KDE
#include <KLocalizedString>

class QVListLayout;
class QLabel;
class DevInfoPlugin;
class QVBoxLayout;
namespace Solid {
class Device;
}

class InfoPanel : public QGroupBox
{
    Q_OBJECT

public:
    explicit InfoPanel(QWidget *, DevInfoPlugin *);
    ~InfoPanel();

    void setTopInfo(const QIcon &, Solid::Device *);
    void setBottomInfo(QVListLayout *lay);
    static QString friendlyString(const QString &,
                                  const QString & = i18nc("name of something is not known",
                                                          "Unknown"));
    static QString convertTf(bool);

private:
    void setTopWidgetLayout(bool = false);
    void setBottomWidgetLayout(QVListLayout *, bool = false);
    void setInfoPanelLayout();
    void setDefaultText();

    QLabel *setDevicesIcon(const QIcon &);
    QVBoxLayout *setAlignedLayout(QWidget *parent, int = 0);

    QWidget *top;
    QWidget *bottom;
    QVBoxLayout *vLayout;
    Solid::Device *dev;
    DevInfoPlugin *status;
};

#endif //INFOPANEL
