
add_subdirectory( about-distro )
add_subdirectory( devinfo )
add_subdirectory(vulkan)
add_subdirectory(cpu)
add_subdirectory(wayland)
add_subdirectory(interrupts)
add_subdirectory(xserver)
add_subdirectory(egl)
add_subdirectory(glx)
add_subdirectory(pci)

add_subdirectory( samba )
add_subdirectory( nics )

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
endif()

