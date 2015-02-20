
add_subdirectory( usbview )
add_subdirectory( infosummary )
add_subdirectory( memory )
add_subdirectory( devinfo )
add_subdirectory( info )

if(NOT WIN32)
add_subdirectory( samba )
add_subdirectory( nics )

set(KCM_ENABLE_OPENGL FALSE)
set(KCM_ENABLE_OPENGLES FALSE)
if(OPENGL_FOUND AND OPENGL_GLU_FOUND AND (${Qt5Gui_OPENGL_IMPLEMENTATION} STREQUAL "GL"))
    set(KCM_ENABLE_OPENGL TRUE)
endif()
if(OPENGLES_FOUND AND (${Qt5Gui_OPENGL_IMPLEMENTATION} STREQUAL "GLESv2"))
    set(KCM_ENABLE_OPENGLES TRUE)
endif()
set(KCM_HAVE_GLX ${KCM_ENABLE_OPENGL})
set(KCM_HAVE_EGL ${EGL_FOUND})

if(${Qt5Gui_OPENGL_IMPLEMENTATION} STREQUAL "GL")
    add_feature_info("OpenGL support" OPENGL_FOUND "View OpenGL details in kinfocenter." )
    add_feature_info("OpenGL Utility Library (GLU)" OPENGL_GLU_FOUND "Required for OpenGL details in kinfocenter." )
endif()

if(${Qt5Gui_OPENGL_IMPLEMENTATION} STREQUAL "GLESv2")
    add_feature_info("OpenGL/ES support" KCM_ENABLE_OPENGLES "View OpenGL ES2.0 details in kinfocenter." )
endif()

  if(NOT APPLE)
    if(KCM_ENABLE_OPENGL OR KCM_ENABLE_OPENGLES)
      add_subdirectory( opengl )
    else()
      message(STATUS "OpenGL/ES2.0 information module has been disabled.")
    endif()
  endif()

find_package(PCIUTILS MODULE)
set_package_properties(PCIUTILS PROPERTIES DESCRIPTION "PciUtils is a library for direct access to PCI slots"
                       URL "http://atrey.karlin.mff.cuni.cz/~mj/pciutils.shtml"
                       TYPE OPTIONAL
                       PURPOSE "View PCI details in kinfocenter."
                      )

if(NOT APPLE)
    add_subdirectory( pci )
endif()
endif()

find_package(RAW1394 MODULE)
set_package_properties(RAW1394 PROPERTIES DESCRIPTION "library for direct access to IEEE 1394 bus"
                       URL "http://www.linux1394.org/"
                       TYPE OPTIONAL
                       PURPOSE "View FireWire devices in kinfocenter."
                      )

if(RAW1394_FOUND)
   add_subdirectory( view1394 )
endif()

if (CMAKE_SYSTEM_NAME MATCHES Linux)
    add_subdirectory(energy)
endif()

