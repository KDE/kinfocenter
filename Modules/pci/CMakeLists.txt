
# OS Base includes
include(../base/CMakeLists.txt)

include_directories( 
	${CMAKE_CURRENT_SOURCE_DIR}/../base
)

########### next target ###############

set(KCM_LIBS
    KF5::KCMUtils
    KF5::KI18n
    KF5::KIconThemes
    KF5::KDE4Support
    ${X11_LIBRARIES}
)

if(PCIUTILS_FOUND)
    MESSAGE(STATUS "Enabling PCI module based on pciutils library")

    add_definitions( -DHAVE_PCIUTILS )
if (UNIX AND NOT APPLE)
   add_definitions (-fpermissive)
endif (UNIX AND NOT APPLE)

    set(KCM_PCI_PART_SRCS kcm_pci.cpp ../base/os_current.cpp kpci.cpp )

    kde4_add_plugin(kcm_pci ${KCM_PCI_PART_SRCS})

    target_link_libraries(kcm_pci ${KCM_LIBS} ${PCIUTILS_LIBRARIES} ${ZLIB_LIBRARIES})

    
else(PCIUTILS_FOUND)

    set(KCM_PCI_PART_SRCS kcm_pci.cpp ../base/os_current.cpp )

    kde4_add_plugin(kcm_pci ${KCM_PCI_PART_SRCS})
    
    target_link_libraries(kcm_pci ${KCM_LIBS} )
endif(PCIUTILS_FOUND)

install(TARGETS kcm_pci  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES kcm_pci.desktop  DESTINATION  ${SERVICES_INSTALL_DIR})
