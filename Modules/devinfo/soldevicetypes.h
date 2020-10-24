/*
 *  SPDX-FileCopyrightText: 2009 David Hubner <hubnerd@ntlworld.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#ifndef SOLDEVICETYPES
#define SOLDEVICETYPES

#include "infopanel.h"
#include "soldevice.h"

class QVListLayout;

class SolProcessorDevice : public SolDevice
{
public:
    SolProcessorDevice(const Solid::DeviceInterface::Type &);
    SolProcessorDevice(QTreeWidgetItem *, const Solid::Device &);
    QVListLayout *infoPanelLayout() override;

private:
    void setDefaultDeviceText() override;
    void setDefaultListing(const Solid::DeviceInterface::Type &) override;
};

class SolStorageDevice : public SolDevice
{
public:
    enum storageChildren {
        CREATECHILDREN,
        NOCHILDREN,
    };

    SolStorageDevice(const Solid::DeviceInterface::Type &);
    SolStorageDevice(QTreeWidgetItem *, const Solid::Device &, const storageChildren & = CREATECHILDREN);
    QVListLayout *infoPanelLayout() override;

private:
    void setDefaultDeviceText() override;
    void setDefaultListing(const Solid::DeviceInterface::Type &) override;
};

class SolVolumeDevice : public SolDevice
{
public:
    SolVolumeDevice(const Solid::DeviceInterface::Type &);
    SolVolumeDevice(QTreeWidgetItem *, const Solid::Device &);
    QVListLayout *infoPanelLayout() override;

private:
    void setDefaultListing(const Solid::DeviceInterface::Type &) override;
};

class SolMediaPlayerDevice : public SolDevice
{
public:
    SolMediaPlayerDevice(const Solid::DeviceInterface::Type &);
    SolMediaPlayerDevice(QTreeWidgetItem *, const Solid::Device &);
    QVListLayout *infoPanelLayout() override;

private:
    void setDefaultListing(const Solid::DeviceInterface::Type &) override;
};

class SolCameraDevice : public SolDevice
{
public:
    SolCameraDevice(const Solid::DeviceInterface::Type &);
    SolCameraDevice(QTreeWidgetItem *, const Solid::Device &);
    QVListLayout *infoPanelLayout() override;

private:
    void setDefaultListing(const Solid::DeviceInterface::Type &) override;
};

class SolBatteryDevice : public SolDevice
{
public:
    SolBatteryDevice(const Solid::DeviceInterface::Type &);
    SolBatteryDevice(QTreeWidgetItem *, const Solid::Device &);
    QVListLayout *infoPanelLayout() override;

private:
    void setDefaultListing(const Solid::DeviceInterface::Type &) override;
};

#endif //
