add_executable(kinfocenter-vulkan-helper main.cpp)
target_link_libraries(kinfocenter-vulkan-helper Qt::Gui)
install(TARGETS kinfocenter-vulkan-helper DESTINATION ${KDE_INSTALL_LIBEXECDIR})
