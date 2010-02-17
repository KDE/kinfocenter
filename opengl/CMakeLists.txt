
include_directories( ${OPENGL_INCLUDE_DIR} ${OPENGL_GLU_INCLUDE_DIR} )

########### next target ###############

set(KCM_OPENGL_PART_SRCS opengl.cpp )
set(KCM_OPENGL_PART_UIS opengl.ui )

qt4_wrap_ui(KCM_OPENGL_PART_UIS_H ${KCM_OPENGL_PART_UIS})

kde4_add_plugin(kcm_opengl ${KCM_OPENGL_PART_SRCS} ${KCM_OPENGL_PART_UIS_H})

target_link_libraries(kcm_opengl ${KDE4_KDEUI_LIBS} ${QT_QTGUI_LIBRARY} ${X11_X11_LIB})

find_library(DL_LIBRARY dl)
if (DL_LIBRARY)
  target_link_libraries(kcm_opengl ${DL_LIBRARY})
endif(DL_LIBRARY)

target_link_libraries(kcm_opengl ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})

install(TARGETS kcm_opengl  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install(FILES opengl.desktop  DESTINATION  ${SERVICES_INSTALL_DIR})
