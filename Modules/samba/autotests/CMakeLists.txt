if(NOT BUILD_TESTING)
    return()
endif()

remove_definitions(-DQT_NO_CAST_FROM_ASCII)

find_package(Qt5Test ${QT_MIN_VERSION} CONFIG REQUIRED)

include(ECMAddTests)

ecm_add_tests(
    ksambasharemodeltest
    smbmountmodeltest
    LINK_LIBRARIES
        Qt::Core
        Qt::Test
        kcm_samba_static
)
