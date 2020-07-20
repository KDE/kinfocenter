
add_subdirectory( about-distro )
add_subdirectory( memory )
add_subdirectory( devinfo )
add_subdirectory( info )

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

if(KCM_ENABLE_OPENGL OR KCM_ENABLE_OPENGLES)
    add_subdirectory( opengl )
else()
    message(STATUS "OpenGL/ES2.0 information module has been disabled.")
endif()

find_package(PCIUTILS MODULE)
set_package_properties(PCIUTILS PROPERTIES DESCRIPTION "PciUtils is a library for direct access to PCI slots"
                       URL "https://mj.ucw.cz/sw/pciutils"
                       TYPE OPTIONAL
                       PURPOSE "View PCI details in kinfocenter."
                      )

# PCI module doesn't work on Linux arm64, nor on MacOS, but does on
# FreeBSD arm64, so this is a bit of a tangle of what-is-supported.
set(_want_pci ON)
if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm" OR ${CMAKE_SYSTEM_PROCESSOR} MATCHES "aarch64")
    if (${CMAKE_SYSTEM} MATCHES "FreeBSD")
        message(STATUS "FreeBSD arm64 pci support enabled.")
    else()
        set(_want_pci OFF)
    endif()
endif()

if(_want_pci)
    add_subdirectory( pci )
endif()

find_package(RAW1394 MODULE)
set_package_properties(RAW1394 PROPERTIES DESCRIPTION "library for direct access to IEEE 1394 bus"
                       URL "https://ieee1394.wiki.kernel.org/index.php/Main_Page"
                       TYPE OPTIONAL
                       PURPOSE "View FireWire devices in kinfocenter."
                      )

if(RAW1394_FOUND)
   add_subdirectory( view1394 )
endif()

find_package(USB1 MODULE)
set_package_properties(USB1 PROPERTIES DESCRIPTION "library that provides generic access to USB devicess"
                       URL "https://libusb.info/"
                       TYPE OPTIONAL
                       PURPOSE "View USB devices in kinfocenter"
                      )

if(USB1_FOUND)
   add_subdirectory( usbview )
endif()

if (CMAKE_SYSTEM_NAME MATCHES Linux)
    add_subdirectory(energy)
    add_subdirectory(fileindexermonitor)
endif()

