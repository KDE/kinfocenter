# TODO: HAVE_COREAUDIO (for OSX)
# TODO: HAVE_LIBDEVINFO_H (for Solaris 7 and later)
#   to be set if both -ldevinfo and libdevinfo.h exist

check_include_files(devinfo.h HAVE_DEVINFO_H)       # info_fbsd.cpp
check_include_files(fstab.h HAVE_FSTAB_H)           # info_linux.cpp
check_include_files(linux/raw.h HAVE_LINUX_RAW_H)   # info_linux.cpp
check_include_files(mntent.h HAVE_MNTENT_H)         # info_linux.cpp
check_include_files(sys/ioctl.h HAVE_SYS_IOCTL_H)   # info_linux.cpp
check_include_files(sys/raw.h HAVE_SYS_RAW_H)       # info_linux.cpp
check_include_files(Alib.h HAVE_ALIB_H)             # info_hpux.cpp

configure_file (../base/config-infocenter.h.cmake ${CMAKE_CURRENT_SOURCE_DIR}/config-infocenter.h )
