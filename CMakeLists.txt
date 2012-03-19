
include_directories( ${KDE4_INCLUDES} )
add_definitions( -DKDE_DEFAULT_DEBUG_AREA=1208 )

add_subdirectory( Categories )
add_subdirectory( Modules )

INCLUDE_DIRECTORIES( ToolTips )

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

kde4_add_executable( kinfocenter ${kinfocenter_SRCS} )

target_link_libraries( kinfocenter ${KDE4_KFILE_LIBS} ${KDE4_KCMUTILS_LIBS} )

##install

install( TARGETS kinfocenter          ${INSTALL_TARGETS_DEFAULT_ARGS} )
install( PROGRAMS kinfocenter.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
INSTALL( FILES kinfocenterui.rc DESTINATION ${DATA_INSTALL_DIR}/kinfocenter )
