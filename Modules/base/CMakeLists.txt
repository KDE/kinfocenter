# TODO: HAVE_COREAUDIO (for OSX)
# TODO: HAVE_LIBDEVINFO_H (for Solaris 7 and later)
#   to be set if both -ldevinfo and libdevinfo.h exist

check_include_files(devinfo.h HAVE_DEVINFO_H)       # info_fbsd.cpp
check_include_files(Alib.h HAVE_ALIB_H)             # info_hpux.cpp

configure_file (../base/config-infocenter.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-infocenter.h )
