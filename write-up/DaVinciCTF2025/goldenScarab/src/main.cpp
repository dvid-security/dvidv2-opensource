#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <vector>
#include <string>
#include "../lib/utils.h"
#include "../lib/images.h"

// Constants
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_ADDRESS 0x3C // I2C address of the OLED screen

// PIN definitions
#if defined(STM32F103xB)
  #define I2C_MAIN_SDA PB7
  #define I2C_MAIN_SCL PB6
  // Data pins for the flag parts
  #define FLAG_PIN_1 PB10  // UART signal (Part 1)
  #define FLAG_PIN_2 PB11  // Manchester signal (Part 2)
  #define FLAG_PIN_3 PA6   // UART signal (Part 3)
  #define FLAG_PIN_4 PA7   // Manchester signal (Part 4)
#elif defined(ARDUINO_ARCH_ESP32)
  #define I2C_MAIN_SDA 15
  #define I2C_MAIN_SCL 4
  // Data pins for the flag parts
  #define FLAG_PIN_1 5     // UART signal (Part 1)
  #define FLAG_PIN_2 26    // Manchester signal (Part 2)
  #define FLAG_PIN_3 32    // UART signal (Part 3)
  #define FLAG_PIN_4 33    // Manchester signal (Part 4)
#endif

// Flag parts - Will be transmitted through the pins
const char* FLAG_PART_1 = "DVCTF{1r34d_";
const char* FLAG_PART_2 = "us1ng";
const char* FLAG_PART_3 = "_pr0b3s";
const char* FLAG_PART_4 = "_0ndv1d}";

const char* CHALLENGE_TITLE = "Golden Scarab Signal";

// Display object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

// Function declarations
void transmitFlagOnPins();
void showVictoryAnimation();
void displayChallengeInfo();
void sendBinaryData(int pin, const char* data);
void sendManchesterData(int pin, const char* data);

// Timing variables
unsigned long lastTransmissionTime = 0;
const unsigned long TRANSMISSION_INTERVAL = 5000; // ms between transmissions (increased interval)
const int BIT_DELAY_US = 670; // Base delay for one bit in microseconds (approx 1500 bps)

void setup() {
  #if defined(STM32F103xB)
    Serial.begin(9600);
    Wire.setSDA(I2C_MAIN_SDA);
    Wire.setSCL(I2C_MAIN_SCL);
    Wire.begin();
    delay(100);
    Serial.println("STM32 architecture detected");
  #else
    Serial.begin(115200);
    Wire.setPins(I2C_MAIN_SDA, I2C_MAIN_SCL);
    Wire.begin();
    delay(100);
    Serial.println("ESP32 architecture detected");
  #endif
  
  Serial.println("Golden Scarab Signal: Logic Analyzer Challenge");

  // Initialize the pins for flag transmission
  pinMode(FLAG_PIN_1, OUTPUT);
  pinMode(FLAG_PIN_2, OUTPUT);
  pinMode(FLAG_PIN_3, OUTPUT);
  pinMode(FLAG_PIN_4, OUTPUT);
  
  // Set initial states to LOW
  digitalWrite(FLAG_PIN_1, LOW);
  digitalWrite(FLAG_PIN_2, LOW);
  digitalWrite(FLAG_PIN_3, LOW);
  digitalWrite(FLAG_PIN_4, LOW);

  // Initialize display
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  
  displaySetupProgress(display, "Starting up...", 20);
  delay(500);
  
  displaySetupProgress(display, "Loading artifacts...", 40);
  delay(500);
  
  displaySetupProgress(display, "Initializing signals...", 60);
  delay(500);
  
  displaySetupProgress(display, "Setting up challenge...", 80);
  delay(500);
  
  displaySetupProgress(display, "Ready!", 100);
  delay(1000);
  
  displayChallengeInfo();
  
  Serial.println("Challenge initialized. Connect logic analyzer to pins to decode the signals.");
}

void loop() {
  // Transmit flag parts on pins every few seconds
  if (millis() - lastTransmissionTime >= TRANSMISSION_INTERVAL) {
    transmitFlagOnPins();
    lastTransmissionTime = millis();
  }
  
  static unsigned long lastDisplayUpdate = 0;
  if (millis() - lastDisplayUpdate > 5000) {
    displayChallengeInfo();
    lastDisplayUpdate = millis();
  }
}

void transmitFlagOnPins() {
  Serial.println("Transmitting signal parts...");
  
  sendBinaryData(FLAG_PIN_1, FLAG_PART_1);
  sendManchesterData(FLAG_PIN_2, FLAG_PART_2);
  sendBinaryData(FLAG_PIN_3, FLAG_PART_3);
  sendManchesterData(FLAG_PIN_4, FLAG_PART_4);
  
  Serial.println("Transmission complete");
}

void sendBinaryData(int pin, const char* data) {
  // UART signal (1500 bps, 8N1, LSB first)
  digitalWrite(pin, HIGH);
  delay(10);
  
  for (int i = 0; i < strlen(data); i++) {
    char c = data[i];
    
    // Start bit
    digitalWrite(pin, LOW);
    delayMicroseconds(BIT_DELAY_US);
    
    // 8 data bits (LSB first)
    for (int bit = 0; bit < 8; bit++) {
      bool bitValue = (c >> bit) & 1;
      digitalWrite(pin, bitValue);
      delayMicroseconds(BIT_DELAY_US);
    }
    
    // 2 stop bits
    digitalWrite(pin, HIGH);
    delayMicroseconds(BIT_DELAY_US * 2);
    
    delay(5);
  }
  
  digitalWrite(pin, HIGH);
  delay(10);
}

void sendManchesterData(int pin, const char* data) {
  // Manchester encoding (IEEE 802.3, 1500 bps, LSB first)
  // In IEEE 802.3: '1' = HIGH->LOW, '0' = LOW->HIGH
  digitalWrite(pin, LOW);
  delay(10);
  
  digitalWrite(pin, LOW);
  delay(20);
  
  for (int i = 0; i < strlen(data); i++) {
    char c = data[i];
    
    // 8 bits per character (LSB first)
    for (int bit = 0; bit < 8; bit++) {
      bool bitValue = (c >> bit) & 1;
      
      if (bitValue) {
        // '1' bit (IEEE 802.3): HIGH then LOW
        digitalWrite(pin, HIGH);
        delayMicroseconds(BIT_DELAY_US / 2);
        digitalWrite(pin, LOW);
        delayMicroseconds(BIT_DELAY_US / 2);
      } else {
        // '0' bit (IEEE 802.3): LOW then HIGH
        digitalWrite(pin, LOW);
        delayMicroseconds(BIT_DELAY_US / 2);
        digitalWrite(pin, HIGH);
        delayMicroseconds(BIT_DELAY_US / 2);
      }
    }
    
    digitalWrite(pin, LOW);
    delay(10);
  }
  
  digitalWrite(pin, LOW);
  delay(20);
}

void drawHeaderBar() {
  display.fillRect(0, 0, display.width(), 10, SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(15, 1);
  display.print("GOLDEN SCARAB");
  display.setTextColor(SSD1306_WHITE);
}

void displayChallengeInfo() {
  display.clearDisplay();
  display.setTextSize(1);
  
  drawHeaderBar();
  
  display.setCursor(0, 15);
  display.println("PB10: HORUS");
  display.println("PB11: ANUBIS");
  display.println("PA6:  HORUS");
  display.println("PA7:  ANUBIS");
  
  display.display();
}