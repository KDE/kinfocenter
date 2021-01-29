configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/Version.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(kcm_SRCS
    main.cpp
    Module.cpp
    Entry.cpp
    PlasmaEntry.cpp
    KernelEntry.cpp
    SectionLabel.cpp
    MemoryEntry.cpp
    GraphicsPlatformEntry.cpp
    CPUEntry.cpp
    GPUEntry.cpp
    FancyString.cpp
)

ki18n_wrap_ui(kcm_SRCS Module.ui)

add_library(kcm_about_distro MODULE ${kcm_SRCS})

target_link_libraries(kcm_about_distro
    KF5::ConfigWidgets
    KF5::CoreAddons
    KF5::I18n
    KF5::Solid)

install(TARGETS kcm_about_distro DESTINATION ${KDE_INSTALL_PLUGINDIR})
install(FILES about-distro.desktop DESTINATION ${SERVICES_INSTALL_DIR})
