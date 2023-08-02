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

