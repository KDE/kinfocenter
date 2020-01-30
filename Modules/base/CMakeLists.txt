include(CheckIncludeFiles)
check_include_files(devinfo.h HAVE_DEVINFO_H)       # info_fbsd.cpp

set(HAVE_WAYLAND ${KF5Wayland_FOUND})

configure_file (../base/config-infocenter.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-infocenter.h )
