
add_subdirectory( usbview )
add_subdirectory( infosummary )
add_subdirectory( nicviewer )
add_subdirectory( memory )
add_subdirectory( devinfo )
add_subdirectory( info )

if(NOT WIN32)
  add_subdirectory( samba )

  macro_optional_find_package(OpenGL)
  macro_log_feature(OPENGL_FOUND "OpenGL" "API for developing portable, interactive 2D and 3D graphics applications" "http://mesa3d.sourceforge.net" FALSE "" "View OpenGL details in kinfocenter.")

  if(OPENGL_FOUND AND OPENGL_GLU_FOUND)
      add_subdirectory( opengl )
  else(OPENGL_FOUND AND OPENGL_GLU_FOUND)
      MESSAGE(STATUS "OpenGL information module has been disabled.")
  endif(OPENGL_FOUND AND OPENGL_GLU_FOUND)


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
