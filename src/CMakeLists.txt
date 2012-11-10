configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/Version.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(kcm_SRCS
    main.cpp
    LSBRelease.cpp
    Module.cpp
)

kde4_add_ui_files(kcm_SRCS Module.ui)

kde4_add_plugin(kcm_about_distro ${kcm_SRCS})

target_link_libraries(kcm_about_distro
    ${KDE4_KDEUI_LIBRARY}
    ${KDE4_KIO_LIBS}
    solid
)

install(TARGETS kcm_about_distro DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES about-distro.desktop DESTINATION ${SERVICES_INSTALL_DIR})
