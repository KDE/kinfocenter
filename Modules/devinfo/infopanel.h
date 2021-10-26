/*
 *  SPDX-FileCopyrightText: 2009 David Hubner <hubnerd@ntlworld.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#ifndef INFOPANEL
#define INFOPANEL

// QT
#include <QGroupBox>

// KDE
#include <KLocalizedString>

class QVListLayout;
class QLabel;
class DevInfoPlugin;
class QVBoxLayout;
namespace Solid
{
class Device;
}

class InfoPanel : public QGroupBox
{
    Q_OBJECT

public:
    explicit InfoPanel(QWidget *, DevInfoPlugin *);
    ~InfoPanel() override;

    void setTopInfo(const QIcon &, Solid::Device *);
    void setBottomInfo(QVListLayout *lay);
    static QString friendlyString(const QString &, const QString & = i18nc("name of something is not known", "Unknown"));
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

#endif // INFOPANEL
