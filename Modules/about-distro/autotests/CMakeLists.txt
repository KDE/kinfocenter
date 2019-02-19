remove_definitions(-DQT_NO_CAST_FROM_ASCII)

include(ECMAddTests)

find_package(Qt5Test ${REQUIRED_QT_VERSION} CONFIG REQUIRED)

# Include src to get access to the OSRelease.h
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

add_library(about-distro-test STATIC ../src/OSRelease.cpp)
target_link_libraries(about-distro-test
    PUBLIC
        KF5::CoreAddons
)

ecm_add_tests(
    OSReleaseTest.cpp
    LINK_LIBRARIES
        Qt5::Test
        about-distro-test
)

ecm_mark_nongui_executable(OSReleaseTest)
