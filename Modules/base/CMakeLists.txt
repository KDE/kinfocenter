# TODO: HAVE_LIBDEVINFO_H (for Solaris 7 and later)
#   to be set if both -ldevinfo and libdevinfo.h exist

include(CheckIncludeFiles)
check_include_files(devinfo.h HAVE_DEVINFO_H)       # info_fbsd.cpp

set(HAVE_WAYLAND ${Wayland_Client_FOUND})

configure_file (../base/config-infocenter.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-infocenter.h )
