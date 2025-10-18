from pages.base.base_page import BasePage
from selenium.webdriver.common.by import By

class BaiduPage(BasePage):
    # 定位器
    SEARCH_INPUT = (By.ID, "kw")
    SEARCH_BUTTON = (By.ID, "su")
    SETTINGS_LINK = (By.LINK_TEXT, "设置")
    SEARCH_SETTINGS = (By.LINK_TEXT, "搜索设置")
    SAVE_SETTINGS = (By.CLASS_NAME, "prefpanelgo")

    def __init__(self, driver):
        super().__init__(driver)
        self.driver = driver

    def open(self):
        """打开百度首页"""
        self.driver.get("https://www.baidu.com")
        return self

    def search(self, keyword):
        """执行搜索"""
        self.input_text(self.SEARCH_INPUT, keyword)
        self.click(self.SEARCH_BUTTON)
        return self

    def open_settings(self):
        """打开搜索设置"""
        self.click(self.SETTINGS_LINK)
        self.click(self.SEARCH_SETTINGS)
        return self

    def save_settings(self):
        """保存搜索设置"""
        self.click(self.SAVE_SETTINGS)
        self.accept_alert()
        return self

    def is_search_input_visible(self):
        """检查搜索输入框是否可见"""
        return self.is_element_visible(self.SEARCH_INPUT)