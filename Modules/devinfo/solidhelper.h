/*
 *  SPDX-FileCopyrightText: 2009 David Hubner <hubnerd@ntlworld.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *
 */

#ifndef SOLIDHELPER
#define SOLIDHELPER

// Solid
#include <solid/deviceinterface.h>

class SolidHelper
{
public:
    SolidHelper()
    {
    }

    Solid::DeviceInterface::Type deviceType(const Solid::Device *);
};

#endif // SOLIDHELPER
