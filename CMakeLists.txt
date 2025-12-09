cmake_minimum_required(VERSION 3.16)

project(kinfocenter)
set(PROJECT_VERSION "6.5.80")

set(QT_MIN_VERSION "6.9.0")

set(KF6_MIN_VERSION "6.18.0")
find_package(ECM ${KF6_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH}  ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)
include(KDEClangFormat)
include(ECMFindQmlModule)
include(KDEGitCommitHooks)
include(ECMSetupQtPluginMacroNames)
include(ECMQueryQt)
include(ECMDeprecationSettings)
include(ECMQmlModule)

option(BUILD_DOC "Whether to build the documentation" ON)

find_package(Qt6 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Core Gui Widgets)

find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS
    Config
    CoreAddons
    I18n
    KCMUtils
    KIO
    Service
    Solid
    Auth
)

find_package(PkgConfig)
pkg_check_modules(libdrm REQUIRED IMPORTED_TARGET libdrm)

if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    find_package(UDev REQUIRED COMPONENTS UDev)
endif()

ecm_find_qmlmodule(org.kde.kirigami 2.5)

macro(kinfocenter_add_kcm target)
    kcmutils_add_qml_kcm(${target} DISABLE_DESKTOP_FILE_GENERATION SOURCES ${ARGN} INSTALL_NAMESPACE plasma/kcms/kinfocenter)
endmacro()

macro(kinfocenter_add_kcm2 target uri)
    kcmutils_add_qml_kcm2(${target}
        URI ${uri}
        DISABLE_DESKTOP_FILE_GENERATION
        SOURCES ${ARGN}
        INSTALL_NAMESPACE plasma/kcms/kinfocenter)
endmacro()

if(BUILD_DOC)
    find_package(KF6DocTools ${KF6_MIN_VERSION})
    set_package_properties(KF6DocTools PROPERTIES
        DESCRIPTION "Tools to generate documentation"
        TYPE REQUIRED
    )
endif()

find_package(SystemSettings QUIET)
set_package_properties(SystemSettings PROPERTIES
    DESCRIPTION "Plasma's SystemSettings"
    TYPE RUNTIME
    PURPOSE "Absolutely required because the KInfoCenter binary is a symlink to it!"
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

remove_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_FROM_BYTEARRAY -DQT_NO_KEYWORDS)
add_definitions(-DQT_NO_NARROWING_CONVERSIONS_IN_CONNECT)
add_definitions(-DQT_NO_URL_CAST_FROM_STRING)
add_definitions(-DQT_USE_QSTRINGBUILDER)
ecm_set_disabled_deprecation_versions(QT 6.8.0
    KF 6.9.0
)


ecm_query_qt(QtBinariesDir QT_INSTALL_BINS)

add_subdirectory( appiumtests )
add_subdirectory( categories )
add_subdirectory( kcms )
add_subdirectory(src)

if(BUILD_DOC)
    add_subdirectory(doc)
    kdoctools_install(po)
endif()

# add clang-format target for all our real source files
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

##install

install( FILES org.kde.kinfocenter.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install( PROGRAMS org.kde.kinfocenter.desktop  DESTINATION  ${KDE_INSTALL_APPDIR} )

ADD_CUSTOM_TARGET(kinfocenter_link ALL
                  COMMAND ${CMAKE_COMMAND} -E create_symlink ${KDE_INSTALL_FULL_BINDIR}/systemsettings ${CMAKE_CURRENT_BINARY_DIR}/kinfocenter)

install( FILES ${CMAKE_CURRENT_BINARY_DIR}/kinfocenter DESTINATION ${KDE_INSTALL_FULL_BINDIR}/ )

ki18n_install(po)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
