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
 * Display setup progress on the OLED screen
 */
void displaySetupProgress(Adafruit_SSD1306 &display, String statusMessage, int progressPercentage) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("UART LOUVRE - SETUP");
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
