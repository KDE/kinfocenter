
project( kcm_devinfo )
find_package( KDE4 REQUIRED )
include_directories( ${KDE4_INCLUDES} )

set( devinfoSources soldevice.cpp devinfo.cpp devicelisting.cpp infopanel.cpp soldevicetypes.cpp qvlistlayout.cpp solidhelper.cpp nicsignals.cpp )

kde4_add_plugin( kcm_devinfo ${devinfoSources} )
target_link_libraries( kcm_devinfo ${KDE4_KIO_LIBS} ${KDE4_SOLID_LIBS} solidcontrol )

install( TARGETS kcm_devinfo DESTINATION ${PLUGIN_INSTALL_DIR} )
install( FILES kcm_devinfo.desktop DESTINATION ${SERVICES_INSTALL_DIR} )