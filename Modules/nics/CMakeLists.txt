
include(CheckSymbolExists)
include(CheckStructMember)

check_include_files(sys/sockio.h HAVE_SYS_SOCKIO_H) # nic.cpp
check_symbol_exists(getnameinfo   "sys/socket.h;netdb.h" HAVE_GETNAMEINFO) # nic.cpp
check_struct_member("struct sockaddr" "sa_len" "sys/socket.h" HAVE_STRUCT_SOCKADDR_SA_LEN) # nic.cpp
check_function_exists(getifaddrs  HAVE_GETIFADDRS)  # nic.cpp

configure_file (config-nic.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-nic.h )



########### next target ###############

set(kcm_nic_PART_SRCS nic.cpp )


add_library(kcm_nic MODULE ${kcm_nic_PART_SRCS})


target_link_libraries(kcm_nic KF5::KCMUtils KF5::I18n KF5::KDELibs4Support )

install(TARGETS kcm_nic  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES nic.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )

