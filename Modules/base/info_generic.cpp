
/*
Copyright 1998  Helge Deller deller@gmx.de

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of
the License or (at your option) version 3 or any later version
accepted by the membership of KDE e.V. (or its successor approved
by the membership of KDE e.V.), which shall act as a proxy 
defined in Section 14 of version 3 of the license.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*  all following functions should return true, when the Information 
 was filled into the lBox-Widget.
 returning false indicates, that information was not available.
 */

bool GetInfo_CPU(QTreeWidget*) {
	return false;
}

bool GetInfo_IRQ(QTreeWidget*) {
	return false;
}

bool GetInfo_DMA(QTreeWidget*) {
	return false;
}

bool GetInfo_PCI(QTreeWidget*) {
	return false;
}

bool GetInfo_IO_Ports(QTreeWidget*) {
	return false;
}

bool GetInfo_Sound(QTreeWidget*) {
	return false;
}

bool GetInfo_Devices(QTreeWidget*) {
	return false;
}

bool GetInfo_SCSI(QTreeWidget*) {
	return false;
}

bool GetInfo_Partitions(QTreeWidget*) {
	return false;
}

bool GetInfo_XServer_and_Video(QTreeWidget* tree) {
	return GetInfo_XServer_Generic(tree);
}
