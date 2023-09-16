# KI18N Translation Domain for this library
add_definitions(-DTRANSLATION_DOMAIN=\"kcmnic\")

include(CheckStructHasMember)

# sa_len is only available on freebsd, not linux.
check_struct_has_member("struct sockaddr" "sa_len" "sys/socket.h" HAVE_STRUCT_SOCKADDR_SA_LEN) # networkmodel.cpp

configure_file(config-nic.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-nic.h)

kinfocenter_add_kcm(kcm_nic nic.cpp networkmodel.cpp)

target_link_libraries(kcm_nic PRIVATE
    KF6::I18n
    KF6::CoreAddons
    KF6::KCMUtils
    KF6::KCMUtilsQuick)

target_compile_features(kcm_nic PUBLIC cxx_std_14)
