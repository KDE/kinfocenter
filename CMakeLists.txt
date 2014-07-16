project(kcm-about-distro)

cmake_minimum_required(VERSION 2.8.12)

find_package(ECM 0.0.14 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(FeatureSummary)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)

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

find_package(KF5ConfigWidgets 5.0.0)
find_package(KF5Service 5.0.0)
find_package(KF5Solid 5.0.0)

add_subdirectory(src)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
