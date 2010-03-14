
set( nicviewSources nicview.cpp )

kde4_add_plugin( kcm_nicview ${nicviewSources} )
target_link_libraries( kcm_nicview ${KDE4_KIO_LIBS} ${KDE4_SOLID_LIBS} )

install( TARGETS kcm_nicview DESTINATION ${PLUGIN_INSTALL_DIR} )
install( FILES kcm_nicview.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
