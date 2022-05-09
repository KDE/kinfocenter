configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/Version.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(dmidecode-helper)

set(kcm_SRCS
    CPUEntry.cpp
    CPUEntry.h
    Entry.cpp
    Entry.h
    FancyString.cpp
    FancyString.h
    GPUEntry.cpp
    GPUEntry.h
    GraphicsPlatformEntry.cpp
    GraphicsPlatformEntry.h
    KernelEntry.cpp
    KernelEntry.h
    main.cpp
    MemoryEntry.cpp
    MemoryEntry.h
    PlasmaEntry.cpp
    PlasmaEntry.h
    ServiceRunner.cpp
)

kcoreaddons_add_plugin(kcm_about-distro SOURCES ${kcm_SRCS} INSTALL_NAMESPACE "plasma/kcms")

target_link_libraries(kcm_about-distro
    KF5::CoreAddons
    KF5::I18n
    KF5::ConfigCore
    KF5::QuickAddons
    KF5::Solid
    KF5::Service
    KF5::KIOGui
    KF5::AuthCore)

kpackage_install_package(package kcm_about-distro kcms)
install(FILES kcm_about-distro.desktop DESTINATION ${KDE_INSTALL_APPDIR})
