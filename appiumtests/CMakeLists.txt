# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2023 Harald Sitter <sitter@kde.org>

find_package(SeleniumWebDriverATSPI)

# Make sure return values get forwarded properly
add_test(
    NAME truetest
    COMMAND selenium-webdriver-at-spi-run /usr/bin/true
)
set_tests_properties(truetest PROPERTIES ENVIRONMENT "TEST_WITH_KWIN_WAYLAND=0")

add_test(
    NAME falsetest
    COMMAND selenium-webdriver-at-spi-run /usr/bin/false
)
set_tests_properties(falsetest PROPERTIES
    WILL_FAIL TRUE
    ENVIRONMENT "TEST_WITH_KWIN_WAYLAND=0")

if(NOT "$ENV{KDECI_BUILD}" STREQUAL "TRUE")
    include(ExternalProject)
    ExternalProject_Add(selenium-webdriver-at-spi-examples
        SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/appiumtests
        BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR}/appiumtests
        INSTALL_COMMAND ""
        TEST_EXCLUDE_FROM_MAIN true
    )
    
add_test(
    NAME appium-about-this-system
    COMMAND selenium-webdriver-at-spi-run ${CMAKE_CURRENT_SOURCE_DIR}/about_this_system.py
)

    set_tests_properties(appium-about-this-system PROPERTIES TIMEOUT 600)
endif()

