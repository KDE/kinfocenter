# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcmdevinfo\")

set( devinfoSources soldevice.cpp devinfo.cpp devicelisting.cpp infopanel.cpp soldevicetypes.cpp qvlistlayout.cpp solidhelper.cpp)

add_library(kcm_devinfo MODULE ${devinfoSources})
target_link_libraries(kcm_devinfo
    KF5::KCMUtils
    KF5::I18n
    KF5::KIOCore
    KF5::KIOWidgets
    KF5::Solid
)

install(TARGETS kcm_devinfo DESTINATION ${KDE_INSTALL_PLUGINDIR})
install( FILES devinfo.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR} )
