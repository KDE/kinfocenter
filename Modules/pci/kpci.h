/* Retrieve information about PCI subsystem through libpci library from
   pciutils package. This should be possible on Linux, BSD and AIX.

   Author: Konrad Rzepecki <hannibal@astral.lodz.pl>
*/

#ifndef KCONTROL_KPCI_H
#define KCONTROL_KPCI_H

#include <QTreeWidget>

bool GetInfo_PCIUtils(QTreeWidget *tree);

#endif // KCONTROL_KPCI_H
