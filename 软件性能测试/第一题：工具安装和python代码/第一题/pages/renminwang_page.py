from pages.base.base_page import BasePage
from selenium.webdriver.common.by import By
from selenium.common.exceptions import NoSuchElementException, TimeoutException

class RenminwangPage(BasePage):
    # 定位器
    SEARCH_INPUT = (By.ID, "keyword")
    SEARCH_BUTTON = (By.CLASS_NAME, "search-btn")
    NEWS_LINK = (By.CSS_SELECTOR, ".news-list li a")

    def __init__(self, driver):
        super().__init__(driver)
        self.driver = driver

    def open(self):
        """打开人民网首页"""
        try:
            self.driver.get("http://www.people.com.cn")
            self.wait.visibility_of_element_located(self.SEARCH_INPUT)
            return self
        except TimeoutException:
            raise TimeoutException("人民网首页加载超时或搜索框不可见")

    def search(self, keyword):
        """执行搜索"""
        try:
            self.input_text(self.SEARCH_INPUT, keyword)
            self.click(self.SEARCH_BUTTON)
            return self
        except Exception as e:
            raise Exception(f"搜索操作失败: {str(e)}")

    def get_news_titles(self):
        """获取新闻标题列表"""
        try:
            news_elements = self.find_elements(self.NEWS_LINK)
            return [news.text for news in news_elements if news.text.strip()]
        except NoSuchElementException:
            return []
        except Exception as e:
            raise Exception(f"获取新闻标题失败: {str(e)}")

    def is_search_input_visible(self):
        """检查搜索输入框是否可见"""
        try:
            return self.is_element_visible(self.SEARCH_INPUT)
        except Exception as e:
            print(f"检查搜索输入框可见性时出错: {str(e)}")
            return False