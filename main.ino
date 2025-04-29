#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Thông tin WiFi
const char* ssid = "Thanh Nam 2";
const char* password = "77777777";
const char* serverName = "https://script.google.com/macros/s/AKfycbxJ3wT2QfP_IROpggBs8dsS57sN0XnXMrmkm3ttykzg4JiYvxqifH3fsw5AzVTHCtF_Lw/exec";

// Cấu hình chân RFID
const uint8_t SS_PIN = 5;
const uint8_t RST_PIN = 4;
MFRC522DriverPinSimple ss_pin(SS_PIN);
MFRC522DriverSPI driver(ss_pin);
MFRC522 mfrc522(driver);

// Cấu hình chân buzzer
const int buzzerPin = 15;

// Khởi tạo LCD (địa chỉ I2C thường là 0x27 hoặc 0x3F, 16 cột và 2 dòng)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(115200);
  
  // Khởi động LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Starting...");
  
  // Khởi động SPI
  SPI.begin();
  Serial.println("SPI Initialized");

  // Khởi động RFID
  pinMode(SS_PIN, OUTPUT);
  digitalWrite(SS_PIN, HIGH);
  mfrc522.PCD_Init();
  Serial.println("RFID Reader Initialized");
  
  // Khởi động buzzer
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);
  
  // Kiểm tra phiên bản firmware của MFRC522
  byte version = mfrc522.PCD_GetVersion();
  if (version == 0x00 || version == 0xFF) {
    Serial.println("ERROR: Could not detect MFRC522");
  } else {
    Serial.print("MFRC522 Version: 0x");
    Serial.println(version, HEX);
  }

  // Kết nối WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  lcd.setCursor(0, 1);
  lcd.print("WiFi Connecting");
  
  int wifiTimeout = 0;
  while (WiFi.status() != WL_CONNECTED && wifiTimeout < 20) {
    delay(500);
    Serial.print(".");
    wifiTimeout++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi");
    Serial.println("IP Address: " + WiFi.localIP().toString());
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Connected");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
    delay(2000); // Hiển thị IP 2 giây
  } else {
    Serial.println("\nFailed to connect to WiFi");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("WiFi Failed");
    while (true);
  }
  lcd.clear();
}

void beep(int duration, int times) {
  for (int i = 0; i < times; i++) {
    digitalWrite(buzzerPin, HIGH);
    delay(duration);
    digitalWrite(buzzerPin, LOW);
    delay(duration);
  }
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    lcd.setCursor(3, 0);
    lcd.print("Swipe card");
    lcd.setCursor(3, 1);
    lcd.print("Please !!!");
  }
  else {
    Serial.println("Card detected");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Card Detected");
    
    if (mfrc522.PICC_ReadCardSerial()) {
      String uid = "";
      for (byte i = 0; i < mfrc522.uid.size; i++) {
        uid += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
        uid += String(mfrc522.uid.uidByte[i], HEX);
      }
      uid.toUpperCase();
      Serial.print("UID: ");
      Serial.println(uid);
      
      lcd.setCursor(0, 1);
      lcd.print("UID: " + uid.substring(0, 10)); // Hiển thị 10 ký tự đầu của UID

      beep(200, 1);

      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        String url = String(serverName) + "?uid=" + uid;
        Serial.println("Sending request to: " + url);

        http.setTimeout(10000);
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

        if (http.begin(url)) {
          int httpCode = http.GET();
          Serial.print("HTTP Response Code: ");
          Serial.println(httpCode);

          if (httpCode == 200) {
            String response = http.getString();
            Serial.print("Ten: ");
            Serial.println(response);
            
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Welcome:");
            lcd.setCursor(0, 1);
            if (response.length() > 16) {
              lcd.print(response.substring(0, 16)); // Cắt chuỗi nếu dài hơn 16 ký tự
            } else {
              lcd.print(response);
            }
            delay(3000); // Hiển thị tên trong 3 giây
            lcd.clear();
          } else if (httpCode == 302) {
            Serial.println("Redirect detected...");
            String location = http.header("Location");
            http.end();
            http.begin(location);
            httpCode = http.GET();
            if (httpCode == 200) {
              String response = http.getString();
              Serial.print("Ten (after redirect): ");
              Serial.println(response);
              
              // Hiển thị tên lên LCD sau khi redirect
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Welcome:");
              lcd.setCursor(0, 1);
              if (response.length() > 16) {
                lcd.print(response.substring(0, 16));
              } else {
                lcd.print(response);
              }
              delay(2000);
              lcd.clear();
            } else {
              lcd.clear();
              lcd.setCursor(0, 0);
              lcd.print("Server Error");
              Serial.println("Failed after redirect");
              delay(2000);
              lcd.clear();
            }
          } else {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("HTTP Error:");
            lcd.setCursor(0, 1);
            lcd.print(httpCode);
            Serial.println("HTTP Error: " + String(httpCode));
            delay(3000);
            lcd.clear();
          }
          http.end();
        } else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Server Failed");
          Serial.println("Failed to connect to server");
          delay(2000);
          lcd.clear();
        }
      } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("WiFi Lost");
        Serial.println("WiFi Disconnected");
        delay(3000);
        lcd.clear();
      }
      
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Read Error");
      Serial.println("ERROR: Failed to read card");
      delay(3000);
      lcd.clear();
    }
  }
  
  delay(200);
}