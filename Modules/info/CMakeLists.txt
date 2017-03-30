# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcminfo\")

# OS Base includes
include(../base/CMakeLists.txt)

include_directories( 
	${CMAKE_CURRENT_SOURCE_DIR}/../base
)

########### next target ###############

set(kcm_info_PART_SRCS main.cpp info.cpp ../base/os_current.cpp )

if(KF5Wayland_FOUND)
    set(kcm_info_PART_SRCS ${kcm_info_PART_SRCS} ../base/info_wayland.cpp)
endif()

add_library(kcm_info MODULE ${kcm_info_PART_SRCS})

target_link_libraries(kcm_info
    KF5::CoreAddons
    KF5::KCMUtils
    KF5::I18n
    KF5::IconThemes
    ${X11_X11_LIB})

if(${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
    target_link_libraries(kcm_info devinfo)
endif()

if(KF5Wayland_FOUND)
    target_link_libraries(kcm_info KF5::WaylandClient)
endif()

install(TARGETS kcm_info  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install(
    FILES 
	dma.desktop 
	interrupts.desktop 
	ioports.desktop 
	xserver.desktop  
    wayland.desktop
	DESTINATION  ${SERVICES_INSTALL_DIR} 
)

