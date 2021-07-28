# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcminfo\")

# OS Base includes
include(../base/CMakeLists.txt)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/../base
)

ecm_setup_qtplugin_macro_names(JSON_ARG3 "CREATE_FACTORY"
    CONFIG_CODE_VARIABLE
    PACKAGE_SETUP_AUTOMOC_VARIABLES
)

########### next target ###############

set(kcm_info_PART_SRCS info.cpp ../base/os_current.cpp )

if(KF5Wayland_FOUND)
    set(kcm_info_PART_SRCS ${kcm_info_PART_SRCS} ../base/info_wayland.cpp)
endif()

add_library(kcm_info_static MODULE STATIC ${kcm_info_PART_SRCS})

target_link_libraries(kcm_info_static
    KF5::CoreAddons
    KF5::KCMUtils
    KF5::I18n
    KF5::IconThemes
    ${X11_X11_LIB})

if(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
    target_link_libraries(kcm_info_staticdevinfo)
endif()

if(KF5Wayland_FOUND)
    target_link_libraries(kcm_info_static KF5::WaylandClient)
    kcoreaddons_add_plugin(kcm_wayland SOURCES kcm_wayland.cpp INSTALL_NAMESPACE "plasma/kcms/kinfocenter")
    kcoreaddons_desktop_to_json(kcm_wayland "wayland.desktop")
    target_link_libraries(kcm_wayland kcm_info_static)
endif()

kcoreaddons_add_plugin(kcm_dma SOURCES kcm_dma.cpp INSTALL_NAMESPACE "plasma/kcms/kinfocenter")
kcoreaddons_desktop_to_json(kcm_dma "dma.desktop")
target_link_libraries(kcm_dma kcm_info_static)

kcoreaddons_add_plugin(kcm_ioports SOURCES kcm_ioports.cpp INSTALL_NAMESPACE "plasma/kcms/kinfocenter")
kcoreaddons_desktop_to_json(kcm_ioports "ioports.desktop")
target_link_libraries(kcm_ioports kcm_info_static)
