# ESP32 Attendance System with Google Sheets

A student attendance management system built with ESP32 and RFID (Module MFRC522), with data uploaded directly to Google Sheets for easy access and management.

## Project Overview
This project develops an automated attendance system using the ESP32 microcontroller and RFID technology (MFRC522 module). It reads RFID tags, records attendance (UID, name, timestamp, and status), and uploads the data to Google Sheets via HTTP requests. The system also features an LCD display (I2C) to show attendance details and a buzzer for audio feedback.

### Features
- **RFID-based Attendance**: Scans RFID tags to identify students and record attendance.
- **Google Sheets Integration**: Uploads attendance data (name, timestamp, status) to Google Sheets for real-time tracking.
- **LCD Display**: Shows student name and attendance status (e.g., "On Time" or "Late").
- **Buzzer Feedback**: Plays a sound ("beep") when a card is scanned.
- **Local Storage**: Uses SPIFFS to store UID-to-name mappings for faster lookup.
- **Time-based Status**: Automatically marks attendance as "On Time" or "Late" based on a predefined schedule (e.g., 8:00 AM).

## Technologies Used
- **Hardware**:
  - ESP32 microcontroller
  - MFRC522 RFID module
  - LCD I2C display
  - Buzzer
- **Software**:
  - Arduino IDE
  - Libraries: `WiFi.h`, `HTTPClient.h`, `MFRC522v2.h`, `LiquidCrystal_I2C.h`
  - Google Apps Script (for Google Sheets integration)
  - SPIFFS (for local storage)
- **Programming Language**: C++ (Arduino)

## Hardware Setup
1. Connect the MFRC522 RFID module to the ESP32:
   - SDA -> GPIO 5
   - SCK -> GPIO 18
   - MOSI -> GPIO 23
   - MISO -> GPIO 19
   - RST -> GPIO 4
   - GND -> GND
   - 3.3V -> 3.3V
2. Connect the LCD I2C:
   - SDA -> GPIO 21
   - SCL -> GPIO 22
   - VCC -> 5V
   - GND -> GND
3. Connect the buzzer:
   - Positive -> GPIO 25
   - Negative -> GND

> **Note:** Refer to [docs/circuit_diagram.png](docs/circuit_diagram.png) for a detailed schematic.

## Software Setup
1. **Install Arduino IDE** and required libraries:
   - `WiFi.h`
   - `HTTPClient.h`
   - `MFRC522v2.h`
   - `LiquidCrystal_I2C.h`
2. **Configure Google Sheets**:
   - Create a Google Sheet.
   - Deploy a Google Apps Script to handle HTTP requests (refer to `docs/google_apps_script.js` if available).
   - Copy the script URL and update it in `config.h` (not included in this repo for security).
3. **Configure WiFi**:
   - Update WiFi credentials in `config.h` (not included in this repo for security).
4. **Upload Code**:
   - Open `main.ino` in Arduino IDE.
   - Upload the code to the ESP32.

## Usage
1. Power on the ESP32 system.
2. Scan an RFID card:
   - The LCD displays the student's name and status ("On Time" or "Late").
   - The buzzer beeps to confirm the scan.
3. Check the Google Sheet for updated attendance records (name, timestamp, status).

## Future Improvements
- Add a real-time clock (RTC) module for better time accuracy.
- Implement a web dashboard for attendance monitoring.
- Add user authentication for secure access to Google Sheets.

## Demo
> **Note**: Add images or videos if available (e.g., `docs/demo_video.mp4` or `docs/screenshot.jpg`).

## Author
- **Name**: Nguyễn Anh Nguyên
- **GitHub**: [NguyenAN80105](https://github.com/NguyenAN80105)
- **Email**: your.email@example.com
- **LinkedIn**: [linkedin.com/in/yourprofile](https://linkedin.com/in/yourprofile)

## License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
