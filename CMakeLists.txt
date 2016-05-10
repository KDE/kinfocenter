project(kinfocenter)
set(PROJECT_VERSION "5.6.4")

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
set(QT_MIN_VERSION "5.4.0")

find_package(ECM 1.2.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS Core Gui Widgets)

set(KF5_MIN_VERSION "5.9.0")
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
    Completion
    Config
    ConfigWidgets
    CoreAddons
    DBusAddons
    DocTools
    I18n
    IconThemes
    KCMUtils
    KDELibs4Support
    KIO
    Service
    Solid
    WidgetsAddons
    XmlGui
    Declarative
    Package
    Solid
)

if(${Qt5Gui_OPENGL_IMPLEMENTATION} STREQUAL "GL")
    find_package(OpenGL)
    set_package_properties(OpenGL PROPERTIES DESCRIPTION "The OpenGL libraries"
                        URL "http://www.opengl.org"
                        TYPE OPTIONAL
                        )
else()
    find_package(OpenGLES)
    set_package_properties(OpenGLES PROPERTIES DESCRIPTION "The OpenGLES libraries"
                        URL "http://www.khronos.org/opengles"
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

if(NOT APPLE)
  find_package(X11)
  set_package_properties(X11 PROPERTIES DESCRIPTION "X11 libraries"
                          URL "http://www.x.org"
                          TYPE OPTIONAL
                          PURPOSE "Required for building the X11 based workspace"
                        )
    if(X11_FOUND)
        set(HAVE_X11 TRUE)
    endif()
else()
  set(X11_FOUND False)
endif()


configure_file(config-workspace.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-workspace.h)
configure_file(config-X11.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-X11.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions( -DKDE_DEFAULT_DEBUG_AREA=1208 )
remove_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_STRICT_ITERATORS -DQT_NO_CAST_FROM_BYTEARRAY -DQT_NO_KEYWORDS)

add_subdirectory( Categories )
add_subdirectory( kcontrol/menus )
add_subdirectory( Modules )
include(ECMOptionalAddSubdirectory)
ecm_optional_add_subdirectory( doc )

include_directories( ToolTips )

set( kinfocenter_SRCS
infocenter.cpp
main.cpp
sidepanel.cpp
kcmcontainer.cpp
kcmtreeitem.cpp
kcmcategoryitem.cpp
infokcmmodel.cpp
infokcmproxymodel.cpp
ToolTips/ktooltip.cpp
ToolTips/ktooltipwindow.cpp
ToolTips/tooltipmanager.cpp
)

add_executable( kinfocenter ${kinfocenter_SRCS} )
target_compile_definitions(kinfocenter PRIVATE -DPROJECT_VERSION="${PROJECT_VERSION}")

target_link_libraries( kinfocenter
    KF5::Completion
    KF5::ConfigWidgets
    KF5::CoreAddons
    KF5::DBusAddons
    KF5::I18n
    KF5::IconThemes
    KF5::KCMUtils
    KF5::KDELibs4Support
    KF5::Service
    KF5::Solid
    KF5::WidgetsAddons
    KF5::XmlGui
)

##install

install( TARGETS kinfocenter          ${INSTALL_TARGETS_DEFAULT_ARGS} )
install( PROGRAMS org.kde.kinfocenter.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES kinfocenterui.rc DESTINATION ${KXMLGUI_INSTALL_DIR}/kinfocenter )

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
