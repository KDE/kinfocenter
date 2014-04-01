project(kinfocenter)

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)
set(QT_MIN_VERSION "5.2.0")

find_package(ECM 0.0.11 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(FeatureSummary)

find_package(Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
    Core
)

find_package(KF5 CONFIG REQUIRED COMPONENTS
    I18n
    XmlGui
    KDE4Support
    WindowSystem
    KCMUtils
    Solid
    KDE4Support
    Plasma
)

find_package(ZLIB)
set_package_properties(ZLIB PROPERTIES DESCRIPTION "Support for gzip compressed files and data streams"
                       URL "http://www.zlib.net"
                       TYPE REQUIRED
                      )

find_package(OpenGL)
set_package_properties(OpenGL PROPERTIES DESCRIPTION "The OpenGL libraries"
                       URL "http://www.opengl.org"
                       TYPE OPTIONAL
                      )

find_package(OpenGLES)
set_package_properties(OpenGLES PROPERTIES DESCRIPTION "The OpenGLES libraries"
                       URL "http://www.khronos.org/opengles"
                       TYPE OPTIONAL
                      )
find_package(EGL)
set_package_properties(EGL PROPERTIES
                       TYPE OPTIONAL
                       PURPOSE "Required to build KWin with EGL support"
                      )

find_package(Wayland 1.2 COMPONENTS Client Egl Cursor)
set_package_properties(Wayland PROPERTIES
                       TYPE OPTIONAL
                       PURPOSE "Required for building KWin with Wayland support"
                      )
add_feature_info("Wayland-Client" Wayland_Client_FOUND "Required for building the Wayland backend in KWin")
add_feature_info("Wayland-EGL" Wayland_Egl_FOUND "Required for building the Wayland EGL compositing backend in KWin")

find_package(X11)
set_package_properties(X11 PROPERTIES DESCRIPTION "X11 libraries"
                        URL "http://www.x.org"
                        TYPE OPTIONAL
                        PURPOSE "Required for building the X11 based workspace"
                      )
if(X11_FOUND)
  find_package(XCB REQUIRED COMPONENTS XCB XFIXES DAMAGE COMPOSITE SHAPE SYNC RENDER RANDR KEYSYMS IMAGE SHM XTEST)
  set_package_properties(XCB PROPERTIES TYPE REQUIRED)
  find_package(XCB COMPONENTS ICCCM)
  add_feature_info("XCB-ICCCM" XCB_ICCCM_FOUND "Required for building test applications for KWin")
endif()

configure_file(config-workspace.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-workspace.h)
configure_file(config-X11.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-X11.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions( -DKDE_DEFAULT_DEBUG_AREA=1208 )
remove_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_STRICT_ITERATORS -DQT_NO_CAST_FROM_BYTEARRAY -DQT_NO_KEYWORDS)

add_subdirectory( Categories )
add_subdirectory( Modules )
add_subdirectory( doc )

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

target_link_libraries( kinfocenter
    KF5::KCMUtils
    KF5::Completion
    KF5::I18n
    KF5::WindowSystem
    KF5::XmlGui
    KF5::KDE4Support
)

##install

install( TARGETS kinfocenter          ${INSTALL_TARGETS_DEFAULT_ARGS} )
install( PROGRAMS kinfocenter.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES kinfocenterui.rc DESTINATION ${DATA_INSTALL_DIR}/kinfocenter )

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
