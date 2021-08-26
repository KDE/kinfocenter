project(kinfocenter)
set(PROJECT_VERSION "5.22.80")

cmake_minimum_required(VERSION 3.16)
set(QT_MIN_VERSION "5.15.0")

set(KF5_MIN_VERSION "5.74")
find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH}  ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)
include(KDEClangFormat)
include(ECMQMLModules)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Core Gui Widgets)

find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    Config
    ConfigWidgets
    CoreAddons
    DocTools
    I18n
    IconThemes
    KCMUtils
    KIO
    Service
    Solid
    WidgetsAddons
    Declarative
    Package
    Solid
    Declarative
)

if(${Qt5Gui_OPENGL_IMPLEMENTATION} STREQUAL "GL")
    find_package(OpenGL)
    set_package_properties(OpenGL PROPERTIES DESCRIPTION "The OpenGL libraries"
                        URL "https://www.opengl.org"
                        TYPE OPTIONAL
                        )
else()
    find_package(OpenGLES)
    set_package_properties(OpenGLES PROPERTIES DESCRIPTION "The OpenGLES libraries"
                        URL "https://www.khronos.org/opengles"
                        TYPE OPTIONAL
                        )
endif()

find_package(EGL)
set_package_properties(EGL PROPERTIES
                       TYPE OPTIONAL
                       PURPOSE "Required for OpenGL Information Module"
                      )

find_package(KF5Wayland CONFIG)
set_package_properties(KF5Wayland PROPERTIES
                       TYPE OPTIONAL
                      )
add_feature_info("KF5Wayland" KF5Wayland_FOUND "Required for Wayland Compositor Information Module")

find_package(X11)
set_package_properties(X11 PROPERTIES DESCRIPTION "X11 libraries"
    URL "https://www.x.org"
    TYPE OPTIONAL
    PURPOSE "Required for building the X11 based workspace"
)
if(X11_FOUND)
    set(HAVE_X11 TRUE)
endif()

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

configure_file(config-X11.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-X11.h)

add_subdirectory( Categories )
add_subdirectory( kcontrol/menus )
add_subdirectory( Modules )
add_subdirectory(src)

include(ECMOptionalAddSubdirectory)
ecm_optional_add_subdirectory( doc )

# add clang-format target for all our real source files
file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})

##install

install( FILES org.kde.kinfocenter.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install( PROGRAMS org.kde.kinfocenter.desktop  DESTINATION  ${KDE_INSTALL_APPDIR} )

ADD_CUSTOM_TARGET(kinfocenter_link ALL
                  COMMAND ${CMAKE_COMMAND} -E create_symlink ${KDE_INSTALL_FULL_BINDIR}/systemsettings5 ${CMAKE_CURRENT_BINARY_DIR}/kinfocenter)

install( FILES ${CMAKE_CURRENT_BINARY_DIR}/kinfocenter DESTINATION ${KDE_INSTALL_FULL_BINDIR}/ )

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
