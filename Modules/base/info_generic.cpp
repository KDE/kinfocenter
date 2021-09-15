/*
    SPDX-FileCopyrightText: 1998 Helge Deller <deller@gmx.de>

    SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
*/

/*  all following functions should return true, when the Information
 was filled into the lBox-Widget.
 returning false indicates, that information was not available.
 */

bool GetInfo_DMA(QTreeWidget *)
{
    return false;
}

bool GetInfo_IO_Ports(QTreeWidget *)
{
    return false;
}
