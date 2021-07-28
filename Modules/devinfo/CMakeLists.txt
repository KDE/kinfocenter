# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcmdevinfo\")

set( devinfoSources soldevice.cpp devinfo.cpp devicelisting.cpp infopanel.cpp soldevicetypes.cpp qvlistlayout.cpp solidhelper.cpp)

kcoreaddons_add_plugin(kcm_devinfo SOURCES ${devinfoSources} INSTALL_NAMESPACE "plasma/kcms/kinfocenter")
kcoreaddons_desktop_to_json(kcm_devinfo "devinfo.desktop")
target_link_libraries(kcm_devinfo
    KF5::KCMUtils
    KF5::I18n
    KF5::KIOCore
    KF5::KIOWidgets
    KF5::Solid
)
