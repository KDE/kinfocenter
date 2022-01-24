# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2021 Harald Sitter <sitter@kde.org>

find_package(lspci)
set_package_properties(lspci PROPERTIES TYPE RUNTIME)

find_package(pciconf)
set_package_properties(pciconf PROPERTIES TYPE RUNTIME)

if(pciconf_FOUND)
    set(PCI_BACKEND pciconf)
    set(PCI_BACKEND_ARGUMENTS "-lv")
else()
    set(PCI_BACKEND lspci)
    set(PCI_BACKEND_ARGUMENTS "-v")
endif()

configure_file(kcm_pci.json.cmake kcm_pci.json)
configure_file(backend.h.cmake backend.h)

add_definitions(-DTRANSLATION_DOMAIN=\"kcm_pci\")

add_library(kcm_pci MODULE main.cpp)
target_link_libraries(kcm_pci KF5::CoreAddons KF5::QuickAddons KF5::I18n KInfoCenterInternal)

install(TARGETS kcm_pci DESTINATION ${KDE_INSTALL_PLUGINDIR}/plasma/kcms/kinfocenter)

kpackage_install_package(package kcm_pci kcms)
