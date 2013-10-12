
include(CheckStructMember)

check_include_files(sys/sockio.h HAVE_SYS_SOCKIO_H) # nic.cpp
check_symbol_exists(getnameinfo   "sys/socket.h;netdb.h" HAVE_GETNAMEINFO) # nic.cpp
check_struct_member("struct sockaddr" "sa_len" "sys/socket.h" HAVE_STRUCT_SOCKADDR_SA_LEN) # nic.cpp
check_function_exists(getifaddrs  HAVE_GETIFADDRS)  # nic.cpp

configure_file (config-nic.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-nic.h )



########### next target ###############

set(kcm_nic_PART_SRCS nic.cpp )


kde4_add_plugin(kcm_nic ${kcm_nic_PART_SRCS})


target_link_libraries(kcm_nic  KF5::KIOCore KF5::KIOWidgets )

install(TARGETS kcm_nic  DESTINATION ${PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES nic.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )

