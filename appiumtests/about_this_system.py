#!/usr/bin/env python3

# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2021-2022 Harald Sitter <sitter@kde.org>
# SPDX-FileCopyrightText: 2023 Alexander Wilms <f.alexander.wilms@gmail.com>

import unittest
from appium import webdriver
from appium.webdriver.common.appiumby import AppiumBy
import selenium.common.exceptions
from selenium.webdriver.support.ui import WebDriverWait


class AboutThisSystemTests(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        desired_caps = {}
        # The app capability may be a command line or a desktop file id.
        desired_caps["app"] = "org.kde.kinfocenter.desktop"
        # Boilerplate, always the same
        self.driver = webdriver.Remote(
            command_executor='http://127.0.0.1:4723',
            desired_capabilities=desired_caps)
        # Set a timeout for waiting to find elements. If elements cannot be found
        # in time we'll get a test failure. This should be somewhat long so as to
        # not fall over when the system is under load, but also not too long that
        # the test takes forever.
        self.driver.implicitly_wait = 10

    @classmethod
    def tearDownClass(self):
        # Make sure to terminate the driver again, lest it dangles.
        self.driver.quit()

    def setUp(self):
        pass

    @classmethod
    def test_plasma_version_clipboard(self):
        self.driver.find_element(by=AppiumBy.NAME, value="About this System").click()
        self.driver.find_element(by=AppiumBy.NAME, value="Copy to Clipboard").click()
        self.assertTrue(self, "Graphics Platform: Wayland" in self.driver.get_clipboard_text())
        self.driver.get_screenshot_as_file("appium_artifact_screenshot_{}.png".format(self.__qualname__))


if __name__ == '__main__':
    unittest.main()
