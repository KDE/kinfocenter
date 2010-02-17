
########### next target ###############

set(KCM_SOLIDPROC_PART_SRCS solidproc.cpp )
set(KCM_SOLIDPROC_PART_UIS solidproc.ui )

qt4_wrap_ui(KCM_SOLIDPROC_PART_UIS_H ${KCM_SOLIDPROC_PART_UIS})

kde4_add_plugin(kcm_solidproc ${KCM_SOLIDPROC_PART_SRCS} ${KCM_SOLIDPROC_PART_UIS_H})

target_link_libraries(kcm_solidproc ${KDE4_KDEUI_LIBS} ${QT_QTGUI_LIBRARY} ${KDE4_SOLID_LIBS})

install(TARGETS kcm_solidproc  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES kcmsolidproc.desktop  DESTINATION  ${SERVICES_INSTALL_DIR})
