
# OS Base includes
include(../base/CMakeLists.txt)

include_directories( 
	${CMAKE_CURRENT_SOURCE_DIR}/../base
)

########### next target ###############

set(kcm_info_PART_SRCS main.cpp info.cpp ../base/os_current.cpp )

add_library(kcm_info MODULE ${kcm_info_PART_SRCS})

target_link_libraries(kcm_info
    KF5::KCMUtils
    KF5::I18n
    KF5::IconThemes
    KF5::KDE4Support
    ${X11_X11_LIB})

install(TARGETS kcm_info  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install(
    FILES 
	dma.desktop 
	interrupts.desktop 
	ioports.desktop 
	scsi.desktop 
	xserver.desktop  
	DESTINATION  ${SERVICES_INSTALL_DIR} 
)

