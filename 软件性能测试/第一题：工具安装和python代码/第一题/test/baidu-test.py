import time

from selenium import webdriver
from selenium.webdriver.common.by import By

driver = webdriver.Chrome()
driver.maximize_window()

driver.get("https://www.baidu.com")
time.sleep(2)
driver.find_element(By.ID, "chat-textarea").send_keys("selenium")
driver.find_element(By.ID, "chat-submit-button").click()
time.sleep(2)
driver.quit()