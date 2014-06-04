# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcm_infosummary\")

set( infosummarySources infosum.cpp defaultboxwidget.cpp progressboxwidget.cpp osdepinfo.cpp)

kde4_add_plugin( kcm_infosummary ${infosummarySources} )
target_link_libraries( kcm_infosummary
    KF5::KCMUtils
    KF5::I18n
    KF5::Solid
    KF5::CoreAddons
    KF5::Plasma
 )

install( TARGETS kcm_infosummary DESTINATION ${PLUGIN_INSTALL_DIR} )
install( FILES kcm_infosummary.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
