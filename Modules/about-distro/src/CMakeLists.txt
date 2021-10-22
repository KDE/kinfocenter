configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/Version.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(kcm_SRCS
    main.cpp
    Entry.cpp
    PlasmaEntry.cpp
    KernelEntry.cpp
    MemoryEntry.cpp
    GraphicsPlatformEntry.cpp
    CPUEntry.cpp
    GPUEntry.cpp
    FancyString.cpp
)

kcoreaddons_add_plugin(kcm_about-distro SOURCES ${kcm_SRCS} INSTALL_NAMESPACE "plasma/kcms")
kcoreaddons_desktop_to_json(kcm_about-distro "kcm_about-distro.desktop")

target_link_libraries(kcm_about-distro
    KF5::CoreAddons
    KF5::I18n
    KF5::ConfigCore
    KF5::QuickAddons
    KF5::Solid)

install(FILES kcm_about-distro.desktop DESTINATION  ${KDE_INSTALL_KSERVICES5DIR}/kcms)
kpackage_install_package(package kcm_about-distro kcms)
