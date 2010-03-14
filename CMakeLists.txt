
add_definitions( -DKDE_DEFAULT_DEBUG_AREA=1208 )

add_subdirectory( about ) 

add_subdirectory( usbview )
add_subdirectory( memory )
add_subdirectory( devinfo )
add_subdirectory( info )

add_subdirectory( ioslaveinfo )

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

set(kinfocenter_SRCS
   main.cpp 
   toplevel.cpp 
   indexwidget.cpp 
   dockcontainer.cpp 
   aboutwidget.cpp 
   moduletreeview.cpp 
   global.cpp 
   modules.cpp 
   proxywidget.cpp 
)

kde4_add_kdeinit_executable( kinfocenter ${kinfocenter_SRCS})

if(${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
    find_library(KVM_LIBRARY kvm)
    target_link_libraries(kinfocenter ${KVM_LIBRARY})
endif(${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")

target_link_libraries(kdeinit_kinfocenter ${KDE4_KIO_LIBS} ${KDE4_KHTML_LIBS} ${KDE4_KUTILS_LIBS} )

install(TARGETS kdeinit_kinfocenter          ${INSTALL_TARGETS_DEFAULT_ARGS})
install(TARGETS kinfocenter          ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install( FILES kinfocenter.desktop  DESTINATION  ${XDG_APPS_INSTALL_DIR} )
install( FILES kinfocenterui.rc  DESTINATION  ${DATA_INSTALL_DIR}/kinfocenter )
