#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

// Function declarations
String sendATCommand(Stream &serial, String command, int timeout = 1000);
void printScreen(Adafruit_SSD1306 &display, String title, String subtitle, String text);
void animateDots(Adafruit_SSD1306 &display, String title, String baseText, String statusText, int cycles = 3);
void connectClientToAP(Stream &serial, const char* ssid, const char* password, const String &defaultClientMac, Adafruit_SSD1306 &display);
void setupClient(Stream &serial, const char* ssid, const char* password, const char* clientHostname, const String &defaultClientMac, Adafruit_SSD1306 &display);
void drawProgressBar(Adafruit_SSD1306 &display, int x, int y, int width, int height, int progress);
void displaySetupProgress(Adafruit_SSD1306 &display, String statusMessage, int progressPercentage);

#endif // UTILS_H
