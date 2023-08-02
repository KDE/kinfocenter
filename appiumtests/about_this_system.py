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
        # The app capability may be a command line or a desktop file id.
        desired_caps["app"] = "org.kde.kinfocenter.desktop"
        # Boilerplate, always the same
        self.driver = webdriver.Remote(
            command_executor='http://127.0.0.1:4723',
            desired_capabilities=desired_caps)
        time.sleep(10)
        # Set a timeout for waiting to find elements. If elements cannot be found
        # in time we'll get a test failure. This should be somewhat long so as to
        # not fall over when the system is under load, but also not too long that
        # the test takes forever.
        self.driver.implicitly_wait = 10

    @classmethod
    def tearDownClass(self):
        #log = logging.getLogger("log")
        # Make sure to terminate the driver again, lest it dangles.
        self.driver.quit()

    def setUp(self):
        pass

    @classmethod
    def test_plasma_version_clipboard(self):
        self.driver.find_element(by=AppiumBy.NAME, value="About this System").click()
        self.driver.find_element(by=AppiumBy.NAME, value="Copy to Clipboard").click()
        text = self.driver.get_clipboard_text()
        #log = logging.getLogger("log")
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
    #logging.basicConfig( stream=sys.stderr )
    #logging.getLogger("log").setLevel( logging.DEBUG )
    unittest.main()
