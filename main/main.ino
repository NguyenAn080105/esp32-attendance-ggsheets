#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// WiFi information
const char* ssid = "WiFi_SSID";
const char* password = "WiFi_Password";
const char* serverName = "Google_sheets_URL";

const uint8_t SS_PIN = 5;
const uint8_t RST_PIN = 4;
MFRC522DriverPinSimple ss_pin(SS_PIN);
MFRC522DriverSPI driver(ss_pin);
MFRC522 mfrc522(driver);
const int buzzerPin = 25;

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(115200);
  
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Starting...");
  
  // Initialize SPI
  SPI.begin();
  Serial.println("SPI Initialized");

  // Initialize RFID
  pinMode(SS_PIN, OUTPUT);
  digitalWrite(SS_PIN, HIGH);
  mfrc522.PCD_Init();
  Serial.println("RFID Reader Initialized");
  
  // Initialize buzzer
  pinMode(buzzerPin, OUTPUT);
  digitalWrite(buzzerPin, LOW);

  // Connect WiFi
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
  // Control buzzer with specified duration and times
  for (int i = 0; i < times; i++) {
    digitalWrite(buzzerPin, HIGH);
    delay(duration);
    digitalWrite(buzzerPin, LOW);
    delay(duration);
  }
}

void display_name(String name)
{
  Serial.print("Ten: ");
  Serial.println(name);
            
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome:");
  lcd.setCursor(0, 1);
  if (name.length() > 16) {
  lcd.print(name.substring(0, 16)); // Truncate string if longer than 16 characters
  } else {
    lcd.print(name);
  }
  delay(3000); // Display name for 3 seconds
  lcd.clear();
}

void loop() {
  if (!mfrc522.PICC_IsNewCardPresent())
  {
    lcd.setCursor(1, 0);
    lcd.print("SCAN YOUR CARD");
    lcd.setCursor(5, 1);
    lcd.print("...");
  }
  else {
    Serial.println("Card detected");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Card Detected");
    beep(200, 1);
    
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
      lcd.print("UID: " + uid.substring(0, 10)); // Display first ten characters of UID

      if (WiFi.status() == WL_CONNECTED) {  // Check if WiFi connection is successful
        HTTPClient http;
        String url = String(serverName) + "?uid=" + uid;  // Construct the URL by appending UID to the serverName
        Serial.println("Sending request to: " + url);

        http.setTimeout(10000);
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

        if (http.begin(url)) {
          int httpCode = http.GET();
          Serial.print("HTTP Response Code: ");
          Serial.println(httpCode);

          if (httpCode == 200) {
            String response = http.getString();
            display_name(response);
          } else if (httpCode == 302) {
            Serial.println("Redirect detected...");
            String location = http.header("Location");
            http.end();
            http.begin(location);
            httpCode = http.GET();
            if (httpCode == 200) {
              String response = http.getString();
              display_name(response);
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
          // Handle failure to start HTTP connection
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Server Failed");
          Serial.println("Failed to connect to server");
          delay(2000);
          lcd.clear();
        }
      } else {
        // WiFi is not connected
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("WiFi Lost");
        Serial.println("WiFi Disconnected");
        delay(3000);
        lcd.clear();
      }
      
      mfrc522.PICC_HaltA(); // Halt the PICC (Proximity Integrated Circuit Card) to stop communication
      mfrc522.PCD_StopCrypto1();  // Stop the encryption on the PCD (Proximity Coupling Device) to reset the crypto state
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Read Error");
      Serial.println("ERROR: Failed to read card");
      delay(3000);
      lcd.clear();
    }
  }
}