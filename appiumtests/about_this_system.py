#!/usr/bin/env python3

# Execute tests with the following command:
# selenium-webdriver-at-spi-run appiumtests/about_this_system.py

# SPDX-License-Identifier: MIT
# SPDX-FileCopyrightText: 2021-2022 Harald Sitter <sitter@kde.org>
# SPDX-FileCopyrightText: 2023 Alexander Wilms <f.alexander.wilms@gmail.com>

#import logging
import sys
import unittest
import time

from appium import webdriver
from appium.webdriver.common.appiumby import AppiumBy

def eprint(*args, **kwargs):
    print(*args, file=sys.stderr, **kwargs)

class AboutThisSystemTests(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        desired_caps = {}
        desired_caps["app"] = "org.kde.kinfocenter.desktop"
        desired_caps['timeouts'] = {
            'implicit': 30000
        }
        self.driver = webdriver.Remote(
            command_executor='http://127.0.0.1:4723',
            desired_capabilities=desired_caps)

    @classmethod
    def tearDownClass(self):
        self.driver.quit()

    def setUp(self):
        pass

    @classmethod
    def test_plasma_version_clipboard(self):
        self.driver.find_element(by=AppiumBy.NAME, value="About this System").click()
        self.driver.find_element(by=AppiumBy.NAME, value="Copy Details").click()
        text = self.driver.get_clipboard_text()
        eprint("▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼")
        eprint("Contents of clipboard:")
        eprint(text)
        eprint("▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲")
        self.assertTrue(self, "Graphics Platform: Wayland" in text)
        self.driver.get_screenshot_as_file(f"appium_artifact_screenshot_{self.__qualname__}_{sys._getframe().f_code.co_name}.png")

    # @classmethod
    # def test_CPU(self):
    #     self.driver.find_element(by=AppiumBy.NAME, value="Devices").click()
    #     self.driver.find_element(by=AppiumBy.NAME, value="CPU").click()
    #     #time.sleep(10)
    #     self.driver.find_element(by=AppiumBy.NAME, value="Copy to Clipboard").click()
    #     clipboard_contents = self.driver.get_clipboard_text()
    #     log = logging.getLogger("log")
    #     eprint("▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼▼")
    #     eprint("Contents of clipboard:")
    #     eprint(clipboard_contents)
    #     eprint("▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲▲")
    #     self.assertTrue(self, "64-bit" in clipboard_contents)
    #     self.driver.get_screenshot_as_file(f"appium_artifact_screenshot_{self.__qualname__}_{sys._getframe().f_code.co_name}.png")

    # @classmethod
    # def test_Wayland(self):
    #     # subprocess.run(['zypper install -y wayland-info'])
    #     self.driver.find_element(by=AppiumBy.NAME, value="Graphics").click()
    #     self.driver.find_element(by=AppiumBy.NAME, value="Wayland").click()
    #     self.driver.find_element(by=AppiumBy.NAME, value="Copy to Clipboard").click()
    #     clipboard_contents = self.driver.get_clipboard_text()
    #     self.assertTrue(self, "interface: 'wl_compositor'" in clipboard_contents)
    #     self.driver.get_screenshot_as_file(f"appium_artifact_screenshot_{self.__qualname__}_{sys._getframe().f_code.co_name}.png")

    # @classmethod
    # def test_get_clipboard(self):
    #     self.driver.set_clipboard_text("asdf")
    #     text = self.driver.get_clipboard_text()
    #     self.assertEqual(text, "asdf", "oops")

if __name__ == '__main__':
    unittest.main()
