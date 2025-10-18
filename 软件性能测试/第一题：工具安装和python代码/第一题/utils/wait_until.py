from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.common.exceptions import TimeoutException

class WaitUntil:
    def __init__(self, driver, timeout=10):
        self.driver = driver
        self.timeout = timeout
        self.wait = WebDriverWait(driver, timeout)

    def presence_of_element_located(self, locator):
        """等待元素存在于DOM中"""
        try:
            return self.wait.until(EC.presence_of_element_located(locator))
        except TimeoutException:
            raise TimeoutException(f"元素 {locator} 在 {self.timeout} 秒内未出现在DOM中")

    def visibility_of_element_located(self, locator):
        """等待元素可见"""
        try:
            return self.wait.until(EC.visibility_of_element_located(locator))
        except TimeoutException:
            raise TimeoutException(f"元素 {locator} 在 {self.timeout} 秒内不可见")

    def element_to_be_clickable(self, locator):
        """等待元素可点击"""
        try:
            return self.wait.until(EC.element_to_be_clickable(locator))
        except TimeoutException:
            raise TimeoutException(f"元素 {locator} 在 {self.timeout} 秒内不可点击")

    def frame_to_be_available_and_switch_to_it(self, locator):
        """等待frame可用并切换到它"""
        try:
            return self.wait.until(EC.frame_to_be_available_and_switch_to_it(locator))
        except TimeoutException:
            raise TimeoutException(f"frame {locator} 在 {self.timeout} 秒内不可用")

    def all_elements_presence_located(self, locator):
        """等待所有匹配元素存在于DOM中"""
        try:
            return self.wait.until(EC.presence_of_all_elements_located(locator))
        except TimeoutException:
            raise TimeoutException(f"元素 {locator} 在 {self.timeout} 秒内未出现在DOM中")

    def text_to_be_present_in_element(self, locator, text):
        """等待元素包含指定文本"""
        try:
            return self.wait.until(EC.text_to_be_present_in_element(locator, text))
        except TimeoutException:
            raise TimeoutException(f"元素 {locator} 在 {self.timeout} 秒内未包含文本 '{text}'")

    def alert_is_present(self):
        """等待alert出现"""
        try:
            return self.wait.until(EC.alert_is_present())
        except TimeoutException:
            raise TimeoutException(f"alert 在 {self.timeout} 秒内未出现")