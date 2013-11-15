
########### next target ###############

set(KCM_MEMORY_PART_SRCS memory.cpp chartWidget.cpp totalMemoryChart.cpp physicalMemoryChart.cpp swapMemoryChart.cpp )

add_library(kcm_memory MODULE ${KCM_MEMORY_PART_SRCS})

target_link_libraries(kcm_memory KF5::KCMUtils KF5::KI18n KF5::KDE4Support)

if (${CMAKE_SYSTEM_NAME} STREQUAL "FreeBSD")
   target_link_libraries(kcm_memory kvm)
endif ()

install(TARGETS kcm_memory  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES kcm_memory.desktop  DESTINATION  ${SERVICES_INSTALL_DIR})
