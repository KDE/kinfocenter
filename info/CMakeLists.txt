
# OS Base includes
include(../base/CMakeLists.txt)

include_directories( 
	${CMAKE_CURRENT_SOURCE_DIR}/../base
)

########### next target ###############

set(kcm_info_PART_SRCS main.cpp info.cpp ../base/os_current.cpp )

kde4_add_plugin(kcm_info ${kcm_info_PART_SRCS})

target_link_libraries(kcm_info  ${KDE4_KDEUI_LIBS} ${QT_QTGUI_LIBRARY} ${X11_X11_LIB})


install(TARGETS kcm_info  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install(
    FILES 
	dma.desktop 
	interrupts.desktop 
	ioports.desktop 
	sound.desktop 
	devices.desktop 
	scsi.desktop 
	xserver.desktop  
	DESTINATION  ${SERVICES_INSTALL_DIR} 
)

