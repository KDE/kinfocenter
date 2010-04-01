
project( kinfocenter )
find_package( KDE4 REQUIRED )

include (KDE4Defaults)
include_directories( ${KDE4_INCLUDES} )

add_definitions( -DKDE_DEFAULT_DEBUG_AREA=1208 )

add_subdirectory( Modules )

set( kinfocenter_SRCS infocenter.cpp main.cpp sidepanel.cpp kcmcontainer.cpp kcmtreeitem.cpp infokcmmodel.cpp)

kde4_add_executable( kinfocenter ${kinfocenter_SRCS})

target_link_libraries( kinfocenter ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_KUTILS_LIBS})

##install

install(TARGETS kinfocenter          ${INSTALL_TARGETS_DEFAULT_ARGS})
install( FILES kinfocenter.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
