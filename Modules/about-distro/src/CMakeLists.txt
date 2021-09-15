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

add_library(kcm_about-distro MODULE ${kcm_SRCS})

target_link_libraries(kcm_about-distro
    KF5::CoreAddons
    KF5::I18n
    KF5::ConfigCore
    KF5::QuickAddons
    KF5::Solid)

kcoreaddons_desktop_to_json(kcm_about-distro "about-distro.desktop")
kpackage_install_package(package kcm_about-distro kcms)

install(TARGETS kcm_about-distro DESTINATION ${KDE_INSTALL_PLUGINDIR}/kcms)
install(FILES about-distro.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR})
