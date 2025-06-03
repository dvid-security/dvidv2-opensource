#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

void printScreen(Adafruit_SSD1306 &display, String title, String subtitle, String text);
void animateDots(Adafruit_SSD1306 &display, String title, String baseText, String statusText, int cycles = 3);
void drawProgressBar(Adafruit_SSD1306 &display, int x, int y, int width, int height, int progress);
void displaySetupProgress(Adafruit_SSD1306 &display, String statusMessage, int progressPercentage);

#endif // UTILS_H
