import os
import time
from fastapi import FastAPI
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.chrome.options import Options
import uvicorn
from datetime import datetime, timedelta

def format_time(time_string):
    parts = time_string.split()
    number = int(parts[0])
    unit = parts[1]

    if unit.startswith("second"):
        delta = timedelta(seconds=number)
    elif unit.startswith("minute"):
        delta = timedelta(minutes=number)
    elif unit.startswith("hour"):
        delta = timedelta(hours=number)
    else:
        raise ValueError("unsupported time unit")

    new_time = datetime.now() - delta
    return new_time.strftime("%H:%M")

app = FastAPI()

@app.get("/scrape")
async def scrape_data():
    chrome_options = Options()
    chrome_options.add_argument("--headless")  
    chrome_options.add_argument("--no-sandbox")
    chrome_options.add_argument("--disable-dev-shm-usage")

    service = Service("/usr/bin/chromedriver")

    driver = webdriver.Chrome(service=service, options=chrome_options)

    try:
        url = "https://iss-mimic.github.io/Mimic/"
        driver.get(url)

        time.sleep(5)

        element = driver.find_element(By.ID, "NODE3000005")
        element_time = driver.find_element(By.ID, "telem_time")
        value = element.text  
        time_value = element_time.text
        return {"element_value": value, "time": format_time(time_value)}

    except Exception as e:
        return {"error": str(e)}
    finally:
        driver.quit()


if __name__ == "__main__":
    port = int(os.environ.get("PORT", 8080))
    uvicorn.run(app, host="0.0.0.0", port=port)
