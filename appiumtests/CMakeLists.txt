# SPDX-License-Identifier: BSD-2-Clause
# SPDX-FileCopyrightText: 2023 Harald Sitter <sitter@kde.org>

if(NOT BUILD_TESTING OR NOT CMAKE_SYSTEM_NAME MATCHES "Linux")
    return()
endif()

find_package(SeleniumWebDriverATSPI)
set_package_properties(SeleniumWebDriverATSPI PROPERTIES
    DESCRIPTION "Server component for selenium tests using Linux accessibility infrastructure"
    PURPOSE "Needed for GUI tests"
    URL "https://invent.kde.org/sdk/selenium-webdriver-at-spi"
    TYPE OPTIONAL
)
if(NOT SeleniumWebDriverATSPI_FOUND AND NOT "$ENV{KDECI_BUILD}" STREQUAL "TRUE") # always run on CI
    return()
endif()

# Make sure return values get forwarded properly
add_test(
    NAME truetest
    COMMAND selenium-webdriver-at-spi-run /usr/bin/true
)
set_tests_properties(truetest PROPERTIES ENVIRONMENT "TEST_WITH_KWIN_WAYLAND=0")

add_test(
    NAME appium-about-this-system
    COMMAND selenium-webdriver-at-spi-run ${CMAKE_CURRENT_SOURCE_DIR}/about_this_system.py
)
set_tests_properties(appium-about-this-system PROPERTIES TIMEOUT 600)

add_test(
    NAME falsetest
    COMMAND selenium-webdriver-at-spi-run /usr/bin/false
)
set_tests_properties(falsetest PROPERTIES
    WILL_FAIL TRUE
    ENVIRONMENT "TEST_WITH_KWIN_WAYLAND=0")

endif()

