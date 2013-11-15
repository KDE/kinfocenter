if(KCM_ENABLE_OPENGL)
include_directories( ${OPENGL_INCLUDE_DIR} ${OPENGL_GLU_INCLUDE_DIR} )
endif()

if(KCM_ENABLE_OPENGLES)
include_directories( ${OPENGLES_INCLUDE_DIR} )
endif()

########### next target ###############

set(KCM_OPENGL_PART_SRCS opengl.cpp )
set(KCM_OPENGL_PART_UIS opengl.ui )

qt5_wrap_ui(KCM_OPENGL_PART_UIS_H ${KCM_OPENGL_PART_UIS})

add_library(kcm_opengl MODULE ${KCM_OPENGL_PART_SRCS} ${KCM_OPENGL_PART_UIS_H})

target_link_libraries(kcm_opengl KF5::KCMUtils KF5::KI18n KF5::KDE4Support ${X11_X11_LIB})

find_library(DL_LIBRARY dl)
if (DL_LIBRARY)
  target_link_libraries(kcm_opengl ${DL_LIBRARY})
endif()

if(KCM_ENABLE_OPENGL)
target_link_libraries(kcm_opengl ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})
endif()

if(KCM_ENABLE_OPENGLES)
target_link_libraries(kcm_opengl ${OPENGLES_LIBRARIES})
endif()

configure_file(openglconfig.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/openglconfig.h )

install(TARGETS kcm_opengl  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install(FILES opengl.desktop  DESTINATION  ${SERVICES_INSTALL_DIR})
