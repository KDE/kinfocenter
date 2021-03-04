# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcmopengl\")

if(KCM_ENABLE_OPENGLES)
include_directories( ${OPENGLES_INCLUDE_DIR} )
endif()

########### next target ###############

set(KCM_OPENGL_PART_SRCS opengl.cpp )
set(KCM_OPENGL_PART_UIS opengl.ui )

ki18n_wrap_ui(KCM_OPENGL_PART_UIS_H ${KCM_OPENGL_PART_UIS})

add_library(kcm_opengl MODULE ${KCM_OPENGL_PART_SRCS} ${KCM_OPENGL_PART_UIS_H})

target_link_libraries(kcm_opengl KF5::KCMUtils KF5::I18n ${X11_X11_LIB})

find_library(DL_LIBRARY dl)
if (DL_LIBRARY)
  target_link_libraries(kcm_opengl ${DL_LIBRARY})
endif()

if(KCM_ENABLE_OPENGL)
target_link_libraries(kcm_opengl OpenGL::GL OpenGL::GLU)
endif()

if(KCM_HAVE_EGL)
  target_link_libraries(kcm_opengl EGL::EGL)
endif()

if(KCM_ENABLE_OPENGLES)
target_link_libraries(kcm_opengl ${OPENGLES_LIBRARIES})
endif()

configure_file(openglconfig.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/openglconfig.h )

install(TARGETS kcm_opengl  DESTINATION ${KDE_INSTALL_PLUGINDIR})

########### install files ###############

install(FILES opengl.desktop  DESTINATION  ${KDE_INSTALL_KSERVICES5DIR})
