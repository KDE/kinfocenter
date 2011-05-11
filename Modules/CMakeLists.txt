
add_subdirectory( usbview )
add_subdirectory( infosummary )
add_subdirectory( memory )
add_subdirectory( devinfo )
add_subdirectory( info )
add_subdirectory( nics )

if(NOT WIN32)
  add_subdirectory( samba )

  macro_optional_find_package(OpenGL)
  macro_log_feature(OPENGL_FOUND "OpenGL" "API for developing portable, interactive 2D and 3D graphics applications" "http://mesa3d.sourceforge.net" FALSE "" "View OpenGL details in kinfocenter.")

  macro_optional_find_package(OpenGLES)
  macro_log_feature(OPENGLES_FOUND "OpenGL ES" "Support for OpenGL ES2.0 graphics library" "http://www.khronos.org/opengles" FALSE "" "View OpenGL ES2.0 details in kinfocenter.")

  if(OPENGL_FOUND AND OPENGL_GLU_FOUND AND NOT KWIN_BUILD_WITH_OPENGLES)
      set( KCM_ENABLE_OPENGL 1 )
  endif(OPENGL_FOUND AND OPENGL_GLU_FOUND AND NOT KWIN_BUILD_WITH_OPENGLES)

  if(OPENGLES_FOUND AND OPENGLES_EGL_FOUND AND KWIN_BUILD_WITH_OPENGLES)
      set( KCM_ENABLE_OPENGLES 1 )
  endif(OPENGLES_FOUND AND OPENGLES_EGL_FOUND AND KWIN_BUILD_WITH_OPENGLES)

  if(KCM_ENABLE_OPENGL OR KCM_ENABLE_OPENGLES)
      add_subdirectory( opengl )
  else(KCM_ENABLE_OPENGL OR KCM_ENABLE_OPENGLES)
      MESSAGE(STATUS "OpenGL/ES2.0 information module has been disabled.")
  endif(KCM_ENABLE_OPENGL OR KCM_ENABLE_OPENGLES)


macro_optional_find_package(PCIUTILS)
macro_log_feature(PCIUTILS_FOUND "PCIUTILS" "PciUtils is a library for direct access to PCI slots" "http://atrey.karlin.mff.cuni.cz/~mj/pciutils.shtml" FALSE "" "View PCI details in kinfocenter.")

if(NOT APPLE)
    add_subdirectory( pci )
endif(NOT APPLE)
endif(NOT WIN32)

macro_optional_find_package(RAW1394)
macro_log_feature(RAW1394_FOUND "RAW1394" "library for direct access to IEEE 1394 bus" "http://www.linux1394.org/" FALSE "" "View FireWire devices in kinfocenter.")

if(RAW1394_FOUND)
   add_subdirectory( view1394 )
endif(RAW1394_FOUND)
