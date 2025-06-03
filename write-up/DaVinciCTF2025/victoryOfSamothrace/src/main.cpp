#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include "../lib/utils/utils.h"
#include "../lib/utils/images.h"

// Constants
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C // I2C address of the OLED screen

// PIN definitions
#if defined(STM32F103xB)
  #define I2C_MAIN_SDA PB7
  #define I2C_MAIN_SCL PB6
  #define UART_ALT_RX PA3
  #define UART_ALT_TX PA2
  #define UART_MAIN_RX PA10
  #define UART_MAIN_TX PA9
#else
  #define UART_MAIN_RX 3
  #define UART_MAIN_TX 1
  #define UART_ALT_RX 16  
  #define UART_ALT_TX 17
  #define I2C_MAIN_SDA 15
  #define I2C_MAIN_SCL 4
#endif

// WiFi settings for the access point
const char* ssid = "C_LOUVRE_Security_System_6.8.7";
const char* password = "L0uvr3_W1F1";

// Admin credentials for IoT management interface 
const char* adminUsername = "curator";
const char* adminPassword = "VictoireDeSamothrace1863";

// Firmware version
const char* firmwareVersion = "v6.8.7";

// Flag (hidden in credentials transmission)
const char* hiddenFlag = "DVCTF{V1ct0ry_W1ng3d_N1k3_Sn1ff3d_Tr4ff1c}";

bool serverStarted = false;
bool challengeCompleted = false;
bool animationPlayed = false;

#if defined(STM32F103xB)
  HardwareSerial wifi(1);  // Use USART1 for STM32 (PA2/PA3)
#else
  HardwareSerial wifi(1);  // Use UART1 for ESP32
#endif
String ipAddress = "192.168.4.1";
String apMacAddress = "AA:CA:FE:C0:DE:BB";

bool processingHttpRequest = false;
unsigned long lastHttpRequestTime = 0;
const unsigned long HTTP_QUIET_PERIOD = 20000;
bool httpQuietPeriodActive = false;

// Telemetry
unsigned long lastSuccessfulTelemetry = 0;
unsigned long lastTelemetryTime = 0;
const unsigned long TELEMETRY_MIN_INTERVAL = 5000;

// Initialize the OLED display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

void setupHttpServer(Stream &serial);
void setupWifi(Stream &serial);
void handleHttpRequests(Stream &serial, Adafruit_SSD1306 &display);
void serveLoginPage(Stream &serial, String connectionId);
void simulateClientRequest(Stream &serial);
void showVictoryAnimation(Adafruit_SSD1306 &display);
void displayChallengeInfo();
void showStaticInfo();

void setup() {
  #if defined(STM32F103xB)
    Serial.setRx(UART_MAIN_RX);
    Serial.setTx(UART_MAIN_TX);
    Serial.begin(9600);
    
    Wire.setSDA(I2C_MAIN_SDA);
    Wire.setSCL(I2C_MAIN_SCL);
    Wire.begin();
    delay(100);
    

    wifi.setRx(UART_ALT_RX);
    wifi.setTx(UART_ALT_TX);
    wifi.begin(115200);
    delay(100);
    
    Serial.println("STM32 architecture");
  #else
    Serial.begin(115200);
    
    Wire.setPins(I2C_MAIN_SDA, I2C_MAIN_SCL);
    Wire.begin();
    delay(100);
    
    wifi.begin(115200, SERIAL_8N1, UART_ALT_RX, UART_ALT_TX);
    delay(100);
    
    Serial.println("ESP32 architecture");
  #endif
  
  Serial.println("Victory of Samothrace System - Traffic Monitoring Challenge");

  // Initialize display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
    for(;;);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  
  displaySetupProgress(display, "Initializing...", 0);
  delay(100);
  
  displaySetupProgress(display, "Testing Security Module...", 20);
  
  Serial.println("Resetting ESP-C6...");
  wifi.println("AT+RST");
  delay(2000);
  
  // Clear any response from reset
  while (wifi.available()) {
    wifi.read();
  }
  
  Serial.println("Testing ESP-C6 communication...");
  String atResponse = sendATCommand(wifi, "AT", 1000);
  Serial.println("AT response: " + atResponse);
  
  if (atResponse.indexOf("OK") < 0) {
    printScreen(display, "BOOT ERROR", "Security module", "not responding!");
    while(1) { delay(100); } // Stop if ESP not responding
  }
  
  
  displaySetupProgress(display, "Setting up WiFi...", 40);
  
  Serial.println("Initializing WiFi driver...");
  String initResponse = sendATCommand(wifi, "AT+CWINIT=1", 1000);
  Serial.println("CWINIT response: " + initResponse);
  
  displaySetupProgress(display, "Creating Access Point...", 60);
  
  // Setup the WiFi in AP mode
  setupWifi(wifi);
  
  displaySetupProgress(display, "Starting Security Portal...", 80);
  
  setupHttpServer(wifi);
  
  displaySetupProgress(display, "Setup Complete", 100);
  delay(1000);
  
  // Display final static screen
  showStaticInfo();
  
  Serial.println("Setup complete - Victory of Samothrace Challenge is active");
}

void setupHttpServer(Stream &serial) {
  Serial.println("Setting up Security Portal...");

  // Activate multiple connections mode
  String muxCmd = "AT+CIPMUX=1";
  String muxResp = sendATCommand(serial, muxCmd, 1000);
  Serial.println("Multiple connections mode: " + muxResp);
  
  if (muxResp.indexOf("OK") < 0) {
    delay(1000);
    muxResp = sendATCommand(serial, muxCmd, 1000);
    Serial.println("Retry multiple connections mode: " + muxResp);
  }
  
  delay(500);
  
  String serverCmd = "AT+CIPSERVER=1,80";
  String serverResp = sendATCommand(serial, serverCmd, 2000);
  Serial.println("Server setup response: " + serverResp);
  
  if (serverResp.indexOf("OK") >= 0) {
    serverStarted = true;
    Serial.println("Security Portal started successfully on port 80");
    printScreen(display, "Victory of Samothrace", "Victory of Samothrace", "Monitoring at " + ipAddress);
  } else {
    Serial.println("Failed to start Security Portal");
    printScreen(display, "ERROR", "Security system failure", "Check module");
  }
}

void setupWifi(Stream &serial) {
  String apCmd = "AT+CWMODE=2";
  String apResp = sendATCommand(serial, apCmd, 1000);
  Serial.println("AP mode response: " + apResp);
  
  String ssidCmd = "AT+CWSAP=\"" + String(ssid) + "\",\"" + String(password) + "\",5,3";
  String ssidResp = sendATCommand(serial, ssidCmd, 1000);
  Serial.println("SSID setup response: " + ssidResp);
  
  String ipCmd = "AT+CIPAP=\"" + ipAddress + "\"";
  String ipResp = sendATCommand(serial, ipCmd, 1000);
  Serial.println("IP setup response: " + ipResp);
  
  delay(1000);
}

void handleHttpRequests(Stream &serial, Adafruit_SSD1306 &display) {
  if (!serverStarted) return;
  
  if (serial.available() > 0) {
    processingHttpRequest = true;
    
    String data = "";
    unsigned long startTime = millis();
    while ((millis() - startTime) < 300) {
      if (serial.available()) {
        data += (char)serial.read();
      } else {
        delay(5); // Shorter delay if no data
      }
    }
    
    Serial.println("Received data length: " + String(data.length()));
    Serial.println("Data received: " + data);
    
    if (data.indexOf("CLOSED") >= 0 && data.indexOf("+IPD") < 0) {
      Serial.println("Client disconnected");
      processingHttpRequest = false;
      return;
    }
    
    if (data.indexOf("+IPD,") >= 0) {
      int ipdIndex = data.indexOf("+IPD,");
      if (ipdIndex >= 0 && ipdIndex + 5 < data.length()) {
        int commaIdx = data.indexOf(",", ipdIndex + 5);
        int connIdIndex = ipdIndex + 5;
        
        if (commaIdx > connIdIndex) {
          String connectionId = data.substring(connIdIndex, commaIdx);
          
          // Verify that connectionId is valid (a number)
          bool validConnId = true;
          for (int i = 0; i < connectionId.length(); i++) {
            if (!isDigit(connectionId.charAt(i))) {
              validConnId = false;
              break;
            }
          }
          
          if (!validConnId) {
            Serial.println("Invalid connection ID: " + connectionId);
            processingHttpRequest = false;
            return;
          }
          
          Serial.println("Connection ID: " + connectionId);
          
          // Update time tracking - start the quiet period
          lastHttpRequestTime = millis();
          httpQuietPeriodActive = true;
          
          // Check for different request 
          bool isPost = data.indexOf("POST") >= 0;
          bool isGet = data.indexOf("GET") >= 0;
          bool isSimpleGet = isGet && (data.indexOf("GET / HTTP") >= 0 || data.indexOf("GET /index.html") >= 0);
          
          bool hasCreds = false;
          
          bool hasUsername = data.indexOf(adminUsername) >= 0;
          bool hasRawPassword = data.indexOf(adminPassword) >= 0;
          bool hasEncodedPassword = data.indexOf("VictoireDeSamothrace1863") >= 0;
          
          Serial.println("Username present: " + String(hasUsername) + ", Raw password: " + String(hasRawPassword) + ", Encoded password: " + String(hasEncodedPassword));
          
          hasCreds = (hasUsername && (hasRawPassword || hasEncodedPassword)) ||
                     (data.indexOf("username=" + String(adminUsername)) >= 0 && 
                     (data.indexOf("password=" + String(adminPassword)) >= 0 || 
                      data.indexOf("password=VictoireDeSamothrace1863") >= 0));
          
          Serial.println("Request type - POST: " + String(isPost) + ", GET: " + String(isGet) + ", SimpleGET: " + String(isSimpleGet) + ", HasCreds: " + String(hasCreds));
          
          if (hasCreds) {
            Serial.println("Login attempt with correct credentials!");
            
            String successPage = "HTTP/1.1 200 OK\r\n"
                                "Content-Type: text/html\r\n"
                                "Connection: close\r\n"
                                "\r\n"
                                "<!DOCTYPE html>\r\n"
                                "<html>\r\n"
                                "<head>\r\n"
                                "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n"
                                "<title>Victory of Samothrace 4K - Success</title>\r\n"
                                "<style>\r\n"
                                "body { font-family: Arial, sans-serif; background-color: #f0f0f0; margin: 0; padding: 20px; display: flex; justify-content: center; align-items: center; height: 100vh; }\r\n"
                                ".success-container { background-color: white; border-radius: 10px; box-shadow: 0 4px 10px rgba(0, 0, 0, 0.2); padding: 30px; width: 350px; max-width: 100%; text-align: center; }\r\n"
                                "h1 { color: #333; margin-bottom: 20px; }\r\n"
                                "h2 { color: #28a745; margin-bottom: 30px; }\r\n"
                                ".flag { font-family: monospace; background-color: #f8f9fa; border: 1px solid #dee2e6; padding: 10px; margin: 20px 0; font-weight: bold; color: #d63384; }\r\n"
                                "</style>\r\n"
                                "</head>\r\n"
                                "<body>\r\n"
                                "<div class=\"success-container\">\r\n"
                                "<h1>Victory of Samothrace 4K Admin Panel</h1>\r\n"
                                "<h2>Login Successful!</h2>\r\n"
                                "<p>You have successfully accessed the security system for the Victory of Samothrace.</p>\r\n"
                                "<div class=\"flag\">" + String(hiddenFlag) + "</div>\r\n"
                                "<p>Challenge completed!</p>\r\n"
                                "</div>\r\n"
                                "</body>\r\n"
                                "</html>";
            
            serial.println("AT+CIPSEND=" + connectionId + "," + String(successPage.length()));
            delay(700);
            
            if (serial.find(">")) {
              serial.print(successPage);
              Serial.println("Success page sent");
              challengeCompleted = true;
            } else {
              Serial.println("Failed to get > prompt for success page");
            }
            
            delay(500);
            serial.println("AT+CIPCLOSE=" + connectionId);
          } else if (isPost) {
            Serial.println("Login attempt with incorrect credentials");
            
            String failPage = "HTTP/1.1 200 OK\r\n"
                            "Content-Type: text/html\r\n"
                            "Connection: close\r\n"
                            "\r\n"
                            "<!DOCTYPE html>\r\n"
                            "<html>\r\n"
                            "<head>\r\n"
                            "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n"
                            "<title>Victory of Samothrace 4K - Error</title>\r\n"
                            "<style>\r\n"
                            "body { font-family: Arial, sans-serif; background-color: #f0f0f0; margin: 0; padding: 20px; display: flex; justify-content: center; align-items: center; height: 100vh; }\r\n"
                            ".error-container { background-color: white; border-radius: 10px; box-shadow: 0 4px 10px rgba(0,0,0,0.2); padding: 30px; width: 350px; max-width: 100%; }\r\n"
                            "h1 { color: #333; margin-bottom: 20px; }\r\n"
                            "h2 { color: #dc3545; margin-bottom: 30px; }\r\n"
                            "a { color: #007bff; text-decoration: none; }\r\n"
                            "a:hover { text-decoration: underline; }\r\n"
                            "</style>\r\n"
                            "</head>\r\n"
                            "<body>\r\n"
                            "<div class=\"error-container\">\r\n"
                            "<h1>Victory of Samothrace 4K Admin Panel</h1>\r\n"
                            "<h2>Authentication Failed</h2>\r\n"
                            "<p><a href='/'>Try Again</a></p>\r\n"
                            "</div>\r\n"
                            "</body>\r\n"
                            "</html>";
            
            serial.println("AT+CIPSEND=" + connectionId + "," + String(failPage.length()));
            delay(700);
            
            if (serial.find(">")) {
              serial.print(failPage);
              Serial.println("Failed login page sent");
            } else {
              Serial.println("Failed to get > prompt for error page");
            }
            
            delay(500);
            serial.println("AT+CIPCLOSE=" + connectionId);
          } else {
            Serial.println("Serving login page");
            serveLoginPage(serial, connectionId);
          }
        }
      }
    }
    
    processingHttpRequest = false;
  }
}

void serveLoginPage(Stream &serial, String connectionId) {
  Serial.println("Serving Victory of Samothrace login page to connection " + connectionId);
  
  // Part 1 - HTTP headers and beginning of HTML
  String loginPage1 = "HTTP/1.1 200 OK\r\n"
                      "Content-Type: text/html\r\n"
                      "Connection: close\r\n"
                      "\r\n"
                      "<!DOCTYPE html>\r\n"
                      "<html>\r\n"
                      "<head>\r\n"
                      "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\r\n"
                      "<title>Victory of Samothrace 4K Management Interface</title>\r\n"
                      "<style>\r\n"
                      "body { font-family: Courier, monospace; background-color: #e0e0e0; margin: 0; padding: 0; }\r\n"
                      ".login-container { border: 3px solid #999; background-color: #f0f0f0; margin: 50px auto; padding: 15px; width: 360px; box-shadow: 5px 5px 15px rgba(0,0,0,0.3); }\r\n"
                      "h1 { color: #000080; text-align: center; font-size: 18px; margin-top: 5px; }\r\n"
                      "h2 { font-size: 14px; color: #444; text-align: center; margin-bottom: 15px; }\r\n"
                      ".form-group { margin-bottom: 10px; }\r\n"
                      "label { display: block; margin-bottom: 5px; font-weight: bold; }\r\n"
                      "input[type=\"text\"], input[type=\"password\"] { width: 100%; padding: 5px; border: 1px solid #999; background-color: #f8f8f8; box-sizing: border-box; font-family: Courier, monospace; }\r\n"
                      "input[type=\"submit\"] { background-color: #808080; color: white; border: 1px solid #666; padding: 8px; width: 100%; cursor: pointer; font-family: Courier, monospace; margin-top: 15px; }\r\n"
                      "input[type=\"submit\"]:hover { background-color: #666; }\r\n"
                      ".header { border-bottom: 1px solid #999; padding-bottom: 10px; margin-bottom: 15px; }\r\n"
                      ".footer { font-size: 10px; text-align: center; margin-top: 15px; color: #666; border-top: 1px solid #999; padding-top: 10px; }\r\n"
                      "</style>\r\n"
                      "</head>\r\n"
                      "<body>\r\n"
                      "<div class=\"login-container\">\r\n"
                      "<div class=\"header\">\r\n"
                      "<h1>Victory of Samothrace 4K System</h1>\r\n"
                      "<h2>Administrator Login Portal</h2>\r\n"
                      "</div>\r\n";

  // Clear buffer
  while (serial.available()) {
    serial.read();
  }
  
  serial.println("AT+CIPSEND=" + connectionId + "," + String(loginPage1.length()));
  delay(700);
  
  if (serial.find(">")) {
    serial.print(loginPage1);
    delay(500);
    Serial.println("Login page part 1 sent");
  } else {
    Serial.println("Failed to get > prompt for part 1");
  }
  
  delay(500);
  
  // Part 2 - Form and end of HTML
  String loginPage2 = "<form action=\"/login\" method=\"post\">\r\n"
                      "<div class=\"form-group\">\r\n"
                      "<label for=\"username\">Username:</label>\r\n"
                      "<input type=\"text\" id=\"username\" name=\"username\" required>\r\n"
                      "</div>\r\n"
                      "<div class=\"form-group\">\r\n"
                      "<label for=\"password\">Password:</label>\r\n"
                      "<input type=\"password\" id=\"password\" name=\"password\" required>\r\n"
                      "</div>\r\n"
                      "<input type=\"submit\" value=\"LOGIN TO SECURITY SYSTEM\">\r\n"
                      "</form>\r\n"
                      "<div class=\"footer\">\r\n"
                      "Victory of Samothrace 4K - Firmware " + String(firmwareVersion) + " - Serial: SC4K0072518<br>\r\n"
                      " 2022-2024 IoT Security Systems Inc. All rights reserved.<br>\r\n"
                      "WARNING: Unauthorized access is prohibited.\r\n"
                      "</div>\r\n"
                      "</div>\r\n"
                      "</body>\r\n"
                      "</html>";
  
  serial.println("AT+CIPSEND=" + connectionId + "," + String(loginPage2.length()));
  delay(700);
  
  if (serial.find(">")) {
    serial.print(loginPage2);
    delay(500);
    Serial.println("Login page part 2 sent");
  } else {
    Serial.println("Failed to get > prompt for part 2");
  }
  
  // Close connection after a delay to ensure data is sent fully
  delay(500);
  serial.println("AT+CIPCLOSE=" + connectionId);
}

void simulateClientRequest(Stream &serial) {
  // Skip telemetry if we're processing HTTP or if not enough time has passed
  if (processingHttpRequest || (millis() - lastTelemetryTime < TELEMETRY_MIN_INTERVAL)) {
    return;
  }
  
  lastTelemetryTime = millis(); // Update last telemetry time
  Serial.println("Simulating IoT camera traffic...");
  
  // First close any existing connections
  sendATCommand(serial, "AT+CIPCLOSE=1", 300);
  delay(300);
  
  // Generate random values for realistic telemetry
  int motionDetected = random(0, 2);  // 0 or 1
  float temperature = 35.5 + (random(-10, 10) / 10.0);  // Temperature around 35.5°C
  int batteryLevel = random(75, 100);  // Battery between 75-100%
  float storageUsed = random(4500, 5800) / 100.0;  // Storage used in GB
  int uptime = random(15000, 25000);  // Device uptime in seconds
  int lightLevel = random(5, 95);     // Ambient light level
  int soundLevel = random(10, 70);    // Ambient sound level in dB
  int peopleDetected = random(0, 3);  // Number of people detected 0-2
  String recordingMode = random(0, 10) > 7 ? "continuous" : "motion";
  const char* eventTypes[] = {"motion_detected", "status_update", "person_detected", "config_update", "storage_alert"};
  String eventType = eventTypes[random(0, 5)];
  
  // Broadcast message directly to server using UDP broadcast
  String startCmd = "AT+CIPSTART=1,\"UDP\",\"192.168.4.255\",1900,1902,0";
  String response = sendATCommand(serial, startCmd, 1000);
  
  // Check for success and retry once if needed
  bool connectionSuccess = (response.indexOf("OK") >= 0 || 
                          response.indexOf("ALREADY CONNECTED") >= 0 || 
                          response.indexOf("CONNECT") >= 0);
  
  if (!connectionSuccess) {
    // Try one more time with TCP to a specific IP
    startCmd = "AT+CIPSTART=1,\"TCP\",\"192.168.4.1\",80";
    response = sendATCommand(serial, startCmd, 1000);
    connectionSuccess = (response.indexOf("OK") >= 0 || 
                        response.indexOf("ALREADY CONNECTED") >= 0 || 
                        response.indexOf("CONNECT") >= 0);
  }
  
  if (connectionSuccess) {
    static int messageCounter = 0;
    messageCounter++;
    
    if (messageCounter % 3 == 0 || random(1, 100) <= 70) {
      // Select a message type based on event type and counter
      String telemetryJson;
      
      switch (messageCounter % 5) {
        case 0:
          if (random(1, 100) <= 40) {
            telemetryJson = "{"
              "\"device\":\"Louvre_Security\","
              "\"type\":\"status_update\","
              "\"timestamp\":" + String(millis()) + ","
              "\"firmware\":\"" + String(firmwareVersion) + "\","
              "\"system\":{"
              "\"uptime\":" + String(uptime) + ","
              "\"temperature\":" + String(temperature, 1) + ","
              "\"battery\":" + String(batteryLevel) + ","
              "\"storage_used_gb\":" + String(storageUsed, 2) + ","
              "\"storage_total_gb\":128.0,"
              "\"wifi_strength\":" + String(random(65, 90)) + ""
              "},"
              "\"config\":{"
              "\"recording\":1,"
              "\"auth\":{"
              "\"UI\":\"" + String(ipAddress) + "\","
              "\"user\":\"" + String(adminUsername) + "\","
              "\"pw\":\"" + String(adminPassword) + "\""
              "}"
              "}"
              "}";
          } else {
            telemetryJson = "{"
              "\"device\":\"Louvre_Security\","
              "\"type\":\"status_update\","
              "\"timestamp\":" + String(millis()) + ","
              "\"firmware\":\"" + String(firmwareVersion) + "\","
              "\"system\":{"
              "\"uptime\":" + String(uptime) + ","
              "\"temperature\":" + String(temperature, 1) + ","
              "\"battery\":" + String(batteryLevel) + ","
              "\"storage_used_gb\":" + String(storageUsed, 2) + ","
              "\"storage_total_gb\":128.0,"
              "\"wifi_strength\":" + String(random(65, 90)) + ""
              "},"
              "\"auth\":{"
              "\"UI\":\"" + String(ipAddress) + "\","
              "\"user\":\"" + String(adminUsername) + "\","
              "\"pw\":\"" + String(adminPassword) + "\""
              "}"
              "}";
          }
          break;
        
        case 1:
          // Motion detection event
          telemetryJson = "{"
            "\"device\":\"Louvre_Security\","
            "\"type\":\"event\","
            "\"event_type\":\"motion_detected\","
            "\"timestamp\":" + String(millis()) + ","
            "\"data\":{"
            "\"zone\":\"" + String(random(1, 4)) + "\","
            "\"confidence\":" + String(random(70, 99)) + ","
            "\"light_level\":" + String(lightLevel) + ","
            "\"recording_started\":true,"
            "\"clip_id\":\"" + String(random(10000, 99999)) + "\""
            "},"
            "\"auth\":{"
            "\"UI\":\"" + String(ipAddress) + "\","
            "\"user\":\"" + String(adminUsername) + "\","
            "\"pw\":\"" + String(adminPassword) + "\""
            "}"
            "}";
          break;
        
        case 2:
          // Person detection
          telemetryJson = "{"
            "\"device\":\"Louvre_Security\","
            "\"type\":\"event\","
            "\"event_type\":\"person_detected\","
            "\"timestamp\":" + String(millis()) + ","
            "\"data\":{"
            "\"count\":" + String(peopleDetected) + ","
            "\"confidence\":" + String(random(85, 99)) + ","
            "\"zone\":\"" + String(random(1, 4)) + "\","
            "\"recording_started\":true,"
            "\"clip_id\":\"" + String(random(10000, 99999)) + "\""
            "}"
            "\"auth\":{"
            "\"UI\":\"" + String(ipAddress) + "\","
            "\"user\":\"" + String(adminUsername) + "\","
            "\"pw\":\"" + String(adminPassword) + "\""
            "}"
            "}";
          break;
          
        case 3:
          // Config update with credentials
          telemetryJson = "{"
            "\"device\":\"Louvre_Security\","
            "\"type\":\"config_update\","
            "\"timestamp\":" + String(millis()) + ","
            "\"changes\":{"
            "\"recording_mode\":\"" + recordingMode + "\","
            "\"motion_sensitivity\":" + String(random(5, 10)) + ","
            "\"night_vision\":true,"
            "\"notifications\":true"
            "},"
            "\"auth\":{"
            "\"UI\":\"" + String(ipAddress) + "\","
            "\"user\":\"" + String(adminUsername) + "\","
            "\"pw\":\"" + String(adminPassword) + "\""
            "}"
            "}";
          break;
          
        case 4:
          // System alert
          telemetryJson = "{"
            "\"device\":\"Louvre_Security\","
            "\"type\":\"alert\","
            "\"timestamp\":" + String(millis()) + ","
            "\"alert_type\":\"" + (batteryLevel < 85 ? "low_battery" : "storage_alert") + "\","
            "\"data\":{"
            "\"severity\":\"" + (batteryLevel < 80 ? "warning" : "info") + "\","
            "\"value\":" + String(batteryLevel < 85 ? batteryLevel : storageUsed, 1) + ","
            "\"threshold\":" + String(batteryLevel < 85 ? 20 : 100) + ""
            "},"
            "\"auth\":{"
            "\"UI\":\"" + String(ipAddress) + "\","
            "\"user\":\"" + String(adminUsername) + "\","
            "\"pw\":\"" + String(adminPassword) + "\""
            "}"
            "}";
          break;
      }
      
      // Send data with shorter command
      serial.println("AT+CIPSEND=1," + String(telemetryJson.length()));
      delay(300);
      
      if (serial.find(">")) {
        serial.print(telemetryJson);
        Serial.println("Telemetry JSON sent: " + eventType);
        lastSuccessfulTelemetry = millis(); // Update last successful time
      } else {
        Serial.println("Failed to get > prompt for telemetry");
      }
    }
    
    // Close the connection
    delay(200);
    sendATCommand(serial, "AT+CIPCLOSE=1", 300);
  } else {
    Serial.println("Failed to establish connection for telemetry");
  }
}

void displayChallengeInfo() {
  display.clearDisplay();
  display.setTextSize(1);
  
  // Draw title
  display.setCursor(0, 0);
  display.println("Victory of Samothrace");
  display.println("Challenge");
  
  display.setCursor(0, 20);
  display.println("Find the credentials");
  display.println("to access the security");
  display.println("system of the Victory");
  display.println("of Samothrace exhibit");
  
  display.display();
}

void showStaticInfo() {
  display.clearDisplay();
  display.setTextSize(1);
  
  // Draw title
  display.setCursor(0, 0);
  display.println("Victory of Samothrace");
  
  // Draw separator
  display.drawLine(0, 10, display.width(), 10, SSD1306_WHITE);
  
  // Draw info
  display.setCursor(0, 15);
  display.println("Monitoring System");
  display.println("WiFi: " + String(ssid));
  display.println("IP: " + ipAddress);
  display.println("Firmware: " + String(firmwareVersion));
  
  display.display();
}

void loop() {
  // Check for incoming HTTP requests first (higher priority)
  handleHttpRequests(wifi, display);
  
  if (httpQuietPeriodActive && (millis() - lastHttpRequestTime >= HTTP_QUIET_PERIOD)) {
    httpQuietPeriodActive = false;
    Serial.println("HTTP quiet period ended, resuming normal operation");
    while (wifi.available()) wifi.read();
    
    if (!challengeCompleted) {
      showStaticInfo();
    }
  }
  
  // Only run telemetry if we're not in quiet period and not processing HTTP
  if (!httpQuietPeriodActive && !processingHttpRequest) {
    simulateClientRequest(wifi);
  } else if (httpQuietPeriodActive) {
    static unsigned long lastDisplayUpdate = 0;
    if (millis() - lastDisplayUpdate > 1000) {
      lastDisplayUpdate = millis();
      
      int remainingSeconds = (HTTP_QUIET_PERIOD - (millis() - lastHttpRequestTime)) / 1000;
    }
  }
  
  if (challengeCompleted && !animationPlayed) {
    for (int repeat = 0; repeat < 10; repeat++) {
      for (int i = 0; i < 30; i++) {
        display.clearDisplay();
        display.drawBitmap(0, 0, bufferAnimation[i], 128, 64, 1);
        display.display();
      }
    }
    animationPlayed = true;
  }
  
  // Reset the ESP if we haven't been able to send telemetry for too long
  static unsigned long lastResetCheckTime = 0;
  
  if (millis() - lastResetCheckTime > 60000) { // Check once per minute
    lastResetCheckTime = millis();
    
    // If haven't sent telemetry for 5 minutes, reset ESP
    if (millis() - lastSuccessfulTelemetry > 300000) {
      Serial.println("Telemetry stuck for too long, resetting module...");
      sendATCommand(wifi, "AT+RST", 1000);
      lastSuccessfulTelemetry = millis(); // Reset timer
      delay(5000); // Wait for ESP to restart
      setupWifi(wifi); // Reconfigure WiFi
      setupHttpServer(wifi); // Restart HTTP server
      // Restore static display after reset
      showStaticInfo();
    }
  }
}
