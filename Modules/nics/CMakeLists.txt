# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcmnic\")

include(CheckSymbolExists)
include(CheckStructHasMember)
include(CheckFunctionExists)
include(CheckIncludeFiles)

check_include_files(sys/sockio.h HAVE_SYS_SOCKIO_H) # networkmodel.cpp
check_symbol_exists(getnameinfo   "sys/socket.h;netdb.h" HAVE_GETNAMEINFO) # networkmodel.cpp
check_struct_has_member("struct sockaddr" "sa_len" "sys/socket.h" HAVE_STRUCT_SOCKADDR_SA_LEN) # networkmodel.cpp
check_function_exists(getifaddrs  HAVE_GETIFADDRS)  # networkmodel.cpp

configure_file(config-nic.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-nic.h)

########### next target ###############
set(kcm_nic_PART_SRCS
    nic.cpp
    networkmodel.cpp)

kcoreaddons_add_plugin(kcm_nic SOURCES ${kcm_nic_PART_SRCS} INSTALL_NAMESPACE "plasma/kcms/kinfocenter")
kcoreaddons_desktop_to_json(kcm_nic "kcm_nic.desktop")

target_link_libraries(kcm_nic
    KF5::I18n
    KF5::CoreAddons
    KF5::KCMUtils
    KF5::QuickAddons)

target_compile_features(kcm_nic PUBLIC cxx_std_14)

kpackage_install_package(package kcm_nic kcms)
