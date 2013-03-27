project(kcm-about-distro)

set(MAJOR_VERSION "0")
set(MINOR_VERSION "0")
set(PATCH_VERSION "0")
set(SUFFIX_VERSION "GIT")

set(VERSION_STRING "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")
if(NOT SUFFIX_VERSION)
    set(VERSION_STRING_FULL "${VERSION_STRING}")
else(NOT SUFFIX_VERSION)
    set(VERSION_STRING_FULL "${VERSION_STRING}-${SUFFIX_VERSION}")
endif(NOT SUFFIX_VERSION)

find_package(Qt4 4.7.0 REQUIRED)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
add_definitions(-std=c++0x)
include_directories(${KDE4_INCLUDES} ${QT_INCLUDES})

add_subdirectory(src)
