configure_file(config-workspace.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-workspace.h)
configure_file(config-X11.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-X11.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

include_directories( ${KDE4_INCLUDES} )
add_definitions( -DKDE_DEFAULT_DEBUG_AREA=1208 )
remove_definitions(-DQT_NO_CAST_FROM_ASCII -DQT_STRICT_ITERATORS -DQT_NO_CAST_FROM_BYTEARRAY -DQT_NO_KEYWORDS)

add_subdirectory( Categories )
add_subdirectory( Modules )

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
