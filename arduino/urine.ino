#include <WiFiS3.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>
#include <LiquidCrystal.h>

const int rs = 8, en = 9, d4 = 4, d5 = 5, d6 = 6, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

char ssid[] = "";
char pass[] = ""

char serverAddress[] = "deployed_docker_container_url"
int port = 443;
String path = "/scrape";

WiFiSSLClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);

void setup() {
  lcd.begin(16, 2);  // 16x2 LCD
  
  Serial.begin(9600);
  while (!Serial);
  
  connectWiFi();
}

void connectWiFi() {
  lcd.clear();
  lcd.print("Connecting WiFi..");

  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, pass);
    delay(5000);
  }

  lcd.clear();
  lcd.print("Connected!");
  delay(1000);
}

String adjustTime(const char* timeStr) {
  int hour, minute;
  sscanf(timeStr, "%d:%d", &hour, &minute);
  
  // Add 2 hours
  hour = (hour + 2) % 24;
  
  char adjustedTime[6];
  sprintf(adjustedTime, "%02d:%02d", hour, minute);
  return String(adjustedTime);
}

void drawProgressBar(int percentage) {
  // Calculate number of filled blocks (max 10 blocks for the bar)
  int numBlocks = map(percentage, 0, 100, 0, 10);
  
  lcd.setCursor(0, 1);
  lcd.print("[");
  
  // Draw filled blocks
  for (int i = 0; i < numBlocks; i++) {
    lcd.print("=");
  }
  
  // Draw empty spaces
  for (int i = numBlocks; i < 10; i++) {
    lcd.print(" ");
  }
  
  lcd.print("]");
  
  lcd.print(" ");
  lcd.print(percentage);
  lcd.print("%");
}

void fetchAndDisplayData() {
  Serial.println("Making HTTPS request...");
  
  client.setTimeout(10000);  
  
  client.get(path);
  
  int statusCode = client.responseStatusCode();
  String response = client.responseBody();
  
  Serial.print("Status code: ");
  Serial.println(statusCode);
  Serial.print("Response: ");
  Serial.println(response);
  
  if (statusCode == 200) {
    JsonDocument doc;
    
    DeserializationError error = deserializeJson(doc, response);
    
    if (!error) {
      const char* time = doc["time"].as<const char*>();
      const char* percentageStr = doc["element_value"].as<const char*>();
      int percentage = atoi(percentageStr);
      
      String adjustedTime = adjustTime(time);

      lcd.clear();
      lcd.print("ISS UT ");
      lcd.print(adjustedTime);
      
      drawProgressBar(percentage);
      
      Serial.println("Display updated successfully");
    } else {
      lcd.clear();
      lcd.print("JSON Parse Error");
      Serial.print("JSON Error: ");
      Serial.println(error.c_str());
    }
  } else {
    lcd.clear();
    lcd.print("HTTP Error: ");
    lcd.print(statusCode);
    Serial.print("Detailed response: ");
    Serial.println(client.responseBody());
  }
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }
  
  fetchAndDisplayData();
  
  delay(600000);  // Update every ten minutes
}
