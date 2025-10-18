import time
from selenium import webdriver
from selenium.webdriver.common.by import By

driver = webdriver.Chrome()
driver.maximize_window()
driver.get("http://search.people.cn/")
# search_box = driver.find_element(By.XPATH, "//*[@id='rmw-search']/div/div[2]/div/div[1]/input")
# search_box.send_keys("思想")
search_box = driver.find_element(By.XPATH, "//*[@type='text']")
search_box.send_keys("品德")
time.sleep(2)
# search_box.clear()
search_box.send_keys("\x01")
time.sleep(2)
search_box.send_keys("思想")
driver.find_element(By.XPATH, "//*[@id='rmw-search']/div/div[2]/div/div[2]").click()
time.sleep(2)
search_box = driver.find_element(By.XPATH, "//*[@type='text']")
search_box.click()
time.sleep(2)
driver.find_element(By.XPATH, "//*[@id='rmw-search']/div/div[1]/div[1]/div/div/div[2]/div/div[3]/div/i").click()
time.sleep(5)

