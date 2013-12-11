project( devinfo )

set( devinfoSources soldevice.cpp devinfo.cpp devicelisting.cpp infopanel.cpp soldevicetypes.cpp qvlistlayout.cpp solidhelper.cpp)

kde4_add_plugin( devinfo ${devinfoSources} )
target_link_libraries( devinfo
    KF5::KCMUtils
    KF5::I18n
    KF5::KIOCore
    KF5::KIOWidgets
    KF5::Solid
    KF5::KDE4Support
)

install( TARGETS devinfo DESTINATION ${PLUGIN_INSTALL_DIR} )
install( FILES devinfo.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
