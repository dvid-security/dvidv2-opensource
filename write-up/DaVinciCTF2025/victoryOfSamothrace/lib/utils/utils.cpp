#include "utils.h"

#ifdef ESP32_PLATFORM
  #define DEBUG_SERIAL Serial2
#else
  #define DEBUG_SERIAL Serial1
#endif

// Debugging macros
#ifdef DEBUG
  #define DEBUG_PRINT(x) DEBUG_SERIAL.print(x)
  #define DEBUG_PRINTLN(x) DEBUG_SERIAL.println(x)
  #define DEBUG_PRINT_HEX(x) DEBUG_SERIAL.print(x, HEX)
#else
  #define DEBUG_PRINT(x)
  #define DEBUG_PRINTLN(x)
  #define DEBUG_PRINT_HEX(x)
#endif

/**
 * Send AT command to the ESP module and get response
 */
String sendATCommand(Stream &serial, String command, int timeout) {
  serial.println(command);
  
  String response = "";
  unsigned long startTime = millis();
  
  bool isCifsrCommand = command.indexOf("AT+CIFSR") >= 0;
  int actualTimeout = isCifsrCommand ? timeout * 2 : timeout; 
  
  while (millis() - startTime < actualTimeout) {
    if (serial.available()) {
      char c = serial.read();
      response += c;
      
      if (!isCifsrCommand && (response.endsWith("OK\r\n") || response.endsWith("ERROR\r\n"))) {
        break;
      }
      
      if (isCifsrCommand) {
        if (response.length() > 10 && !serial.available()) {
          delay(100);
          if (!serial.available()) {
            break;
          }
        }
      }
    }
  }
  
  return response;
}

/**
 * Print information to the OLED display
 */
void printScreen(Adafruit_SSD1306 &display, String title, String subtitle, String text) {
  display.clearDisplay();
  
  display.setCursor(0,0);
  display.println(title);
  display.println(subtitle);
  display.println();
  display.println(text);
  
  display.display();
}

/**
 * Animate dots for loading text
 */
void animateDots(Adafruit_SSD1306 &display, String title, String baseText, String statusText, int cycles) {
  String dots[] = {".", "..", "..."};
  
  for (int i = 0; i < cycles; i++) {
    for (int dotCount = 0; dotCount < 3; dotCount++) {
      display.clearDisplay();
      display.setCursor(0,0);
      display.println(title);
      display.println(baseText + dots[dotCount]);
      display.println();
      display.println(statusText);
      display.display();
      delay(300); // Adjust timing for dot animation
    }
  }
}

/**
 * Connect client to AP
 */
void connectClientToAP(Stream &serial, const char* ssid, const char* password, const String &defaultClientMac, Adafruit_SSD1306 &display) {
  DEBUG_PRINTLN("Disconnecting from any existing AP...");
  String disconnectResp = sendATCommand(serial, "AT+CWQAP", 1000);
  DEBUG_PRINTLN("Disconnect response: " + disconnectResp);
  
  if (disconnectResp.indexOf("busy") >= 0) {
    DEBUG_PRINTLN("Module busy, waiting before retry...");
    delay(2000);
    disconnectResp = sendATCommand(serial, "AT+CWQAP", 1000);
    DEBUG_PRINTLN("Disconnect retry response: " + disconnectResp);
  }
  
  delay(1000);
  
  DEBUG_PRINTLN("Setting client MAC address to " + defaultClientMac);
  String macResp = sendATCommand(serial, "AT+CIPSTAMAC=\"" + defaultClientMac + "\"", 1000);
  DEBUG_PRINTLN("MAC setting response: " + macResp);
  
  if (macResp.indexOf("busy") >= 0) {
    DEBUG_PRINTLN("Module busy, waiting before retry...");
    delay(2000);
    macResp = sendATCommand(serial, "AT+CIPSTAMAC=\"" + defaultClientMac + "\"", 1000);
    DEBUG_PRINTLN("MAC setting retry response: " + macResp);
  }
  
  delay(1000);
  
  DEBUG_PRINTLN("Connecting to AP: " + String(ssid));
  String connectCmd = "AT+CWJAP=\"" + String(ssid) + "\",\"" + String(password) + "\"";
  String connectResp = sendATCommand(serial, connectCmd, 15000); // Increased timeout for connection
  DEBUG_PRINTLN("Connect response: " + connectResp);
  
  if (connectResp.indexOf("busy") >= 0 || connectResp.indexOf("ERROR") >= 0) {
    DEBUG_PRINTLN("Connection failed or module busy, waiting before retry...");
    delay(3000);
    connectResp = sendATCommand(serial, connectCmd, 15000);
    DEBUG_PRINTLN("Connect retry response: " + connectResp);
  }
  
  if (connectResp.indexOf("OK") >= 0 || connectResp.indexOf("ALREADY CONNECTED") >= 0) {
    DEBUG_PRINTLN("Successfully connected to AP");
    printScreen(display, "Client Connected", "to " + String(ssid), "");
    delay(2000);
  } else {
    DEBUG_PRINTLN("Failed to connect to AP after retry");
    printScreen(display, "WARNING", "Client connection failed", "Challenge may not work");
    delay(2000);
  }
}

/**
 * WiFi client setup
 */
void setupClient(Stream &serial, const char* ssid, const char* password, const char* clientHostname, const String &defaultClientMac, Adafruit_SSD1306 &display) {
  DEBUG_PRINTLN("Setting up client in Station+AP mode...");
  String clientCmd = "AT+CWMODE=3";  // Mode 3 = SoftAP + Station
  String clientResp = sendATCommand(serial, clientCmd, 2000);
  DEBUG_PRINTLN("CWMODE response: " + clientResp);
  
  if (clientResp.indexOf("busy") >= 0) {
    DEBUG_PRINTLN("Module busy, waiting before retry...");
    delay(3000);
    clientResp = sendATCommand(serial, clientCmd, 2000);
    DEBUG_PRINTLN("CWMODE retry response: " + clientResp);
  }
  
  if (clientResp.indexOf("OK") < 0) {
    DEBUG_PRINTLN("Failed to set client mode");
    printScreen(display, "ERROR", "Failed to set client mode", "Check ESP firmware");
    return;
  }
  
  delay(1000);
  
  DEBUG_PRINTLN("Setting client hostname to " + String(clientHostname));
  String hostnameCmd = "AT+CWHOSTNAME=\"" + String(clientHostname) + "\"";
  String hostnameResp = sendATCommand(serial, hostnameCmd, 1000);
  DEBUG_PRINTLN("Hostname setting response: " + hostnameResp);
  
  if (hostnameResp.indexOf("busy") >= 0) {
    DEBUG_PRINTLN("Module busy, waiting before retry...");
    delay(2000);
    hostnameResp = sendATCommand(serial, hostnameCmd, 1000);
    DEBUG_PRINTLN("Hostname setting retry response: " + hostnameResp);
  }
  
  delay(1000);
  
  connectClientToAP(serial, ssid, password, defaultClientMac, display);
}

/**
 * Display setup progress on the OLED screen
 */
void displaySetupProgress(Adafruit_SSD1306 &display, String statusMessage, int progressPercentage) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("Victory of Samothrace - Setup");
  display.setCursor(0, 20);
  display.println(statusMessage);
  drawProgressBar(display, 0, 54, 128, 10, progressPercentage);
  display.display();
}

/**
 * Draw a progress bar on the OLED display
 */
void drawProgressBar(Adafruit_SSD1306 &display, int x, int y, int width, int height, int progress) {
  display.drawRect(x, y, width, height, SSD1306_WHITE);
  
  int progressWidth = (width - 4) * progress / 100;
  
  display.fillRect(x + 2, y + 2, progressWidth, height - 4, SSD1306_WHITE);
}
