
set( devinfoSources soldevice.cpp devinfo.cpp devicelisting.cpp infopanel.cpp soldevicetypes.cpp qvlistlayout.cpp solidhelper.cpp nicsignals.cpp )

kde4_add_plugin( devinfo ${devinfoSources} )
target_link_libraries( devinfo ${KDE4_KIO_LIBS} ${KDE4_SOLID_LIBS} solidcontrol )

install( TARGETS devinfo DESTINATION ${PLUGIN_INSTALL_DIR} )
install( FILES devinfo.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
