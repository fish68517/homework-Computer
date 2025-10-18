import until
from selenium.webdriver.common.by import By
from selenium.webdriver.support.select import Select
from utils.wait_until import WaitUntil

class BasePage:
    def __init__(self, driver):
        self.driver = driver
        self.wait = WaitUntil(driver)

    def find_element(self, locator):
        """查找元素并确保其存在"""
        return self.wait.presence_of_element_located(locator)

    def find_elements(self, locator):
        """查找多个元素"""
        return self.wait.all_elements_presence_located(locator)

    def click(self, locator):
        """点击元素"""
        element = self.wait.element_to_be_clickable(locator)
        element.click()

    def input_text(self, locator, text):
        """输入文本"""
        element = self.wait.visibility_of_element_located(locator)
        element.clear()
        element.send_keys(text)

    def get_text(self, locator):
        """获取元素文本"""
        element = self.wait.visibility_of_element_located(locator)
        return element.text

    def select_by_visible_text(self, locator, text):
        """通过可见文本选择下拉选项"""
        select = Select(self.wait.visibility_of_element_located(locator))
        select.select_by_visible_text(text)

    def select_by_value(self, locator, value):
        """通过value属性选择下拉选项"""
        select = Select(self.wait.visibility_of_element_located(locator))
        select.select_by_value(value)

    def select_by_index(self, locator, index):
        """通过索引选择下拉选项"""
        select = Select(self.wait.visibility_of_element_located(locator))
        select.select_by_index(index)

    def switch_to_frame(self, locator):
        """切换到iframe/frame"""
        self.wait.frame_to_be_available_and_switch_to_it(locator)

    def switch_to_default_content(self):
        """切换回默认内容"""
        self.driver.switch_to.default_content()

    def is_element_visible(self, locator):
        """检查元素是否可见"""
        try:
            self.wait.visibility_of_element_located(locator)
            return True
        except:
            return False

    def is_element_present(self, locator):
        """检查元素是否存在"""
        try:
            self.wait.presence_of_element_located(locator)
            return True
        except:
            return False

    def get_element_attribute(self, locator, attribute):
        """获取元素属性"""
        element = self.wait.presence_of_element_located(locator)
        return element.get_attribute(attribute)

    def accept_alert(self):
        """接受alert"""
        alert = self.wait.alert_is_present()
        alert.accept()

    def dismiss_alert(self):
        """取消alert"""
        alert = self.wait.alert_is_present()
        alert.dismiss()