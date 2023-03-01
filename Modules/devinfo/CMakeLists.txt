# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcmdevinfo\")

set( devinfoSources soldevice.cpp devinfo.cpp devicelisting.cpp infopanel.cpp soldevicetypes.cpp qvlistlayout.cpp solidhelper.cpp)

kcoreaddons_add_plugin(kcm_devinfo SOURCES ${devinfoSources} INSTALL_NAMESPACE "plasma/kcms/kinfocenter")
target_link_libraries(kcm_devinfo
    KF6::KCMUtils
    KF6::I18n
    KF6::KIOCore
    KF6::KIOWidgets
    KF6::Solid
)
