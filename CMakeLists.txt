project(kinfocenter)
set(PROJECT_VERSION "5.24.80")

cmake_minimum_required(VERSION 3.16)
set(QT_MIN_VERSION "5.15.0")

set(KF5_MIN_VERSION "5.90")
set(KDE_COMPILERSETTINGS_LEVEL "5.82")
find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH}  ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)
include(KDEClangFormat)
include(ECMFindQmlModule)
include(KDEGitCommitHooks)
include(ECMSetupQtPluginMacroNames)

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Core Gui Widgets)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    Config
    ConfigWidgets
    CoreAddons
    DocTools
    I18n
    KCMUtils
    KIO
    Service
    Solid
    WidgetsAddons
    Declarative
    Package
    Solid
)

ecm_find_qmlmodule(org.kde.kirigami 2.5)

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

add_subdirectory( Categories )
add_subdirectory( kcontrol/menus )
add_subdirectory( Modules )
add_subdirectory(src)

include(ECMOptionalAddSubdirectory)
ecm_optional_add_subdirectory( doc )

# add clang-format target for all our real source files
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
kde_configure_git_pre_commit_hook(CHECKS CLANG_FORMAT)

##install

install( FILES org.kde.kinfocenter.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install( PROGRAMS org.kde.kinfocenter.desktop  DESTINATION  ${KDE_INSTALL_APPDIR} )

ADD_CUSTOM_TARGET(kinfocenter_link ALL
                  COMMAND ${CMAKE_COMMAND} -E create_symlink ${KDE_INSTALL_FULL_BINDIR}/systemsettings5 ${CMAKE_CURRENT_BINARY_DIR}/kinfocenter)

install( FILES ${CMAKE_CURRENT_BINARY_DIR}/kinfocenter DESTINATION ${KDE_INSTALL_FULL_BINDIR}/ )

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
