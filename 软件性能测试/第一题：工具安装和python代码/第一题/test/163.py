import time

from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC

# 初始化浏览器驱动（这里以Chrome为例）
driver = webdriver.Chrome()

try:
    # 打开163邮箱登录页面
    driver.get("https://mail.163.com/")

    # 等待页面加载完成并切换到iframe（163邮箱的登录框在iframe中）
    WebDriverWait(driver, 10).until(
        EC.frame_to_be_available_and_switch_to_it((By.XPATH, "//iframe[contains(@id, 'x-URS-iframe')]"))
    )

    # 等待账号输入框可见并输入"sss"
    username_input = WebDriverWait(driver, 10).until(
        EC.visibility_of_element_located((By.NAME, "email"))
    )
    username_input.send_keys("oooooooooo")
    time.sleep(3)
    username_input.clear()
    username_input.send_keys("sss")

    # 可选：截图查看结果
    driver.save_screenshot("163mail_input.png")

    # 这里可以继续后续操作，如输入密码、点击登录等

finally:
    # 关闭浏览器（实际使用时可根据需要调整）
    # driver.quit()
    pass