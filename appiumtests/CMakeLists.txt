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


add_test(
    NAME appium-about-this-system
    COMMAND selenium-webdriver-at-spi-run ${CMAKE_CURRENT_SOURCE_DIR}/about_this_system.py
)
set_tests_properties(appium-about-this-system PROPERTIES
    TIMEOUT 60
    ENVIRONMENT "RECORD_VIDEO_NAME=appium_artifact_about.webm;QT_LOGGING_RULES_OVERRIDE=*=true")
