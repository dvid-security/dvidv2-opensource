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
  #define UART_MAIN_RX PA10
  #define UART_MAIN_TX PA9
#elif defined(ARDUINO_ARCH_ESP32)
  #define I2C_MAIN_SDA 15 // strapping pin HSPICS0
  #define I2C_MAIN_SCL 4 // strapping pin HSPIHD
  #define UART_MAIN_RX 3
  #define UART_MAIN_TX 1
#endif

const char* CHALLENGE_TITLE = "Louvre Security System";
const char* CHALLENGE_VERSION = "v1.0.3";
const char* DEFAULT_USERNAME = "visitor";
const char* DEFAULT_PASSWORD = "monalisa";
const char* ADMIN_USERNAME = "leonardo";
const char* ADMIN_PASSWORD = "LeonardoDaVinci743$BicniVaDodranoeL";
const char* MQTT_DEVICE_ID = "devices/0a56eecf-2955-4756-8f5d-80adc2e55ac0";
const char* FLAG = "DVCTF{L0uvr3_U4rt_Sh3ll_4rt1st}";

const char* louvreArt[] = {
  "       /\\       ",
  "      /  \\      ",
  "     /    \\     ",
  "    /      \\    ",
  "   /        \\   ",
  "  /          \\  ",
  " /            \\ ",
  "/______/\\______\\"
};

bool isAuthenticated = false;
bool isAdmin = false;
bool challengeCompleted = false;
bool mqttConnected = false;
bool mqttValidated = false;

String commandBuffer = "";
const int MAX_BUFFER = 64;

// Captcha system
String currentCaptcha = "";
const int CAPTCHA_LENGTH = 8;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

void processCommand();
void printHelp();
void printConfig();
void clearScreen();
void printWelcome();
void printLoginPrompt();
void showVictoryAnimation();
void displayChallengeInfo();
void drawHeaderBar();
String generateCaptcha();
void updateDisplayStatus();

void setup() {
  #if defined(STM32F103xB)
    Serial.setRx(UART_MAIN_RX);
    Serial.setTx(UART_MAIN_TX);
    Serial.begin(9600);
    
    Wire.setSDA(I2C_MAIN_SDA);
    Wire.setSCL(I2C_MAIN_SCL);
    Wire.begin();
    delay(100);
    
    Serial.println("STM32 architecture detected");
  #else
    Serial.begin(9600);
    
    Wire.setPins(I2C_MAIN_SDA, I2C_MAIN_SCL);
    Wire.begin();
    delay(100);
    
    Serial.println("ESP32 architecture detected");
  #endif
  
  Serial.println("Louvre Museum UART Security Challenge");

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println("SSD1306 allocation failed");
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  
  displaySetupProgress(display, "Starting up...", 20);
  delay(500);
  
  displaySetupProgress(display, "Initializing UART...", 40);
  delay(500);
  
  displaySetupProgress(display, "Loading challenge...", 60);
  delay(500);
  
  displaySetupProgress(display, "Ready!", 100);
  delay(1000);
  
  displayChallengeInfo();
  
  delay(2000);
  
  currentCaptcha = generateCaptcha();
  
  updateDisplayStatus();
  
  printWelcome();
}

void loop() {
  while (Serial.available() > 0) {
    char incomingChar = Serial.read();
    
    if (incomingChar == 8 || incomingChar == 127) {
      if (commandBuffer.length() > 0) {
        commandBuffer.remove(commandBuffer.length() - 1);
        Serial.print("\b \b");
      }
    }
    else if (incomingChar == '\r' || incomingChar == '\n') {
      if (commandBuffer.length() > 0) {
        Serial.println();
        processCommand();
        commandBuffer = ""; 
        
        if (isAuthenticated) {
          if (isAdmin) {
            Serial.print("leonardo@louvre:~# ");
          } else {
            Serial.print("visitor@louvre:~$ ");
          }
        }
      }
    }
    else if (commandBuffer.length() < MAX_BUFFER) {
      commandBuffer += incomingChar;
      Serial.print(incomingChar);
    }
  }
  
  static unsigned long lastDisplayUpdate = 0;
  if (millis() - lastDisplayUpdate > 5000) {
    updateDisplayStatus();
    lastDisplayUpdate = millis();
  }
}

void processCommand() {
  String command = commandBuffer;
  command.trim();
  
  if (!isAuthenticated) {
    if (command.startsWith("login")) {
      int spaceIndex = command.indexOf(' ');
      if (spaceIndex > 0 && spaceIndex < command.length() - 1) {
        String credentials = command.substring(spaceIndex + 1);
        int colonIndex = credentials.indexOf(':');
        
        if (colonIndex > 0 && colonIndex < credentials.length() - 1) {
          String username = credentials.substring(0, colonIndex);
          
          int captchaIndex = credentials.indexOf(':', colonIndex + 1);
          if (captchaIndex > 0 && captchaIndex < credentials.length() - 1) {
            String password = credentials.substring(colonIndex + 1, captchaIndex);
            String providedCaptcha = credentials.substring(captchaIndex + 1);
            
            if (providedCaptcha == currentCaptcha) {
              if ((username == DEFAULT_USERNAME && password == DEFAULT_PASSWORD) ||
                  (username == ADMIN_USERNAME && password == ADMIN_PASSWORD)) {
                isAuthenticated = true;
                isAdmin = (username == ADMIN_USERNAME);
                
                Serial.println("Authentication successful!");
                if (isAdmin) {
                  Serial.println("Welcome, leonardo. You have administrative access.");
                  Serial.print("leonardo@louvre:~# ");
                } else {
                  Serial.println("Welcome, Visitor. You have limited access.");
                  Serial.print("visitor@louvre:~$ ");
                }
                
                updateDisplayStatus();
              } else {
                Serial.println("Invalid credentials. Please try again.");
                currentCaptcha = generateCaptcha();
                updateDisplayStatus();
                printLoginPrompt();
              }
            } else {
              Serial.println("Invalid captcha. Please try again.");
              currentCaptcha = generateCaptcha();
              updateDisplayStatus();
              printLoginPrompt();
            }
          } else {
            Serial.println("Invalid format. Use: 'login username:password:captcha'");
            currentCaptcha = generateCaptcha();
            updateDisplayStatus();
            printLoginPrompt();
          }
        } else {
          Serial.println("Invalid format. Use: 'login username:password:captcha'");
          currentCaptcha = generateCaptcha();
          updateDisplayStatus();
          printLoginPrompt();
        }
      } else {
        printLoginPrompt();
      }
    } else if (command == "help") {
      Serial.println("Available commands:");
      Serial.println("  login username:password:captcha - Log in to the system");
      Serial.println("  help                           - Show this help message");
      printLoginPrompt();
    } else {
      Serial.println("Authentication required. Please login first.");
      printLoginPrompt();
    }
    return;
  }
  
  if (command == "help") {
    printHelp();
  } else if (command == "logout") {
    isAuthenticated = false;
    isAdmin = false;
    Serial.println("Logged out successfully.");
    printLoginPrompt();
  } else if (command == "clear") {
    clearScreen();
    if (isAdmin) {
      Serial.print("leonardo@louvre:~# ");
    } else {
      Serial.print("visitor@louvre:~$ ");
    }
  } else if (command == "about") {
    Serial.println("Louvre Museum Security System " + String(CHALLENGE_VERSION));
    Serial.println("Developed by the Louvre Security Team");
  } else if (command == "art") {
    for (int i = 0; i < 8; i++) {
      Serial.println(louvreArt[i]);
    }
    Serial.println("The Louvre Pyramid - Designed by I.M. Pei, 1989");
  } else if (command == "config") {
    if (isAdmin) {
      printConfig();
    } else {
      Serial.println("Permission denied: Administrative privileges required.");
    }
  } else if (command.startsWith("cat ")) {
    String filename = command.substring(4);
    if (isAdmin) {
      if (filename == "/etc/passwd") {
        Serial.println("visitor:x:1000:1000:Visitor Account:/home/visitor:/bin/bash");
        Serial.println("leonardo:x:1001:1001:Museum leonardo:/home/leonardo:/bin/bash");
        Serial.println("security:x:1002:1002:Security System:/var/security:/sbin/nologin");
      } else if (filename == "/etc/shadow") {
        Serial.println("Permission denied: Root privileges required.");
      } else if (filename == "/var/log/security.log" || filename == "security.log") {
        Serial.println("=== Security Log ===");
        Serial.println("[2025-03-29 08:15:23] System startup");
        Serial.println("[2025-03-29 10:32:45] Failed login attempt: username=admin");
        Serial.println("[2025-03-29 12:45:18] Backup completed successfully");
        Serial.println("[2025-03-29 14:03:52] MQTT connection established: " + String(MQTT_DEVICE_ID));
        Serial.println("[2025-03-29 15:30:11] System check: All sensors operational");
      } else if (filename == ".bash_history") {
        Serial.println("ls -la");
        Serial.println("cd /var/security");
        Serial.println("cat config.json");
        Serial.println("./security_thread --status");
        Serial.println("clear");
        Serial.println("exit");
      } else if (filename == ".bashrc"){
        Serial.println("alias grep='grep --color=auto'");
        Serial.println("alias fgrep='fgrep --color=auto'");
        Serial.println("alias egrep='egrep --color=auto'");
        Serial.println("alias ll='ls -alF'");
        Serial.println("alias la='ls -A'");
        Serial.println("alias l='ls -CF'");
      } else if (filename == "notes.txt") {
        Serial.println("Remember to rotate thread key daily.");
        Serial.println("Current key: Check security log (/var/log/security.log)");
      } else {
        Serial.println("File not found: " + filename);
      }
    } else {
      if (filename == "welcome.txt") {
        Serial.println("Welcome to the Louvre Museum Security System");
        Serial.println("For visitor inquiries, please contact the information desk.");
        Serial.println("For administrative access, please use the leonardo account.");
      } else if (filename == "/etc/issue") {
        Serial.println("Louvre Security System " + String(CHALLENGE_VERSION));
        Serial.println("Unauthorized access is strictly prohibited.");
      } else if (filename == ".bashrc"){
        Serial.println("alias grep='grep --color=auto'");
        Serial.println("alias fgrep='fgrep --color=auto'");
        Serial.println("alias egrep='egrep --color=auto'");
        Serial.println("alias ll='ls -alF'");
        Serial.println("alias la='ls -A'");
        Serial.println("alias l='ls -CF'");
      } else if (filename == "pw.txt"){
        Serial.println("GlassPyramid235%HdimaryPssalG");
        Serial.println("VenusDeMilo625&BoliMeDsuneV");
        Serial.println("VenusDeMilo497&BoliMeDsuneV");
        Serial.println("RomanPortraiture856#HerutiartroPnamoR");
        Serial.println("VenusDeMilo268!JoliMeDsuneV");
        Serial.println("AncientEgypt213!JtpygEtneicnA");
        Serial.println("AncientEgypt933&C");
        Serial.println("MedievalWell625$DlleWlaveideM");
        Serial.println("EugeneDelacroix331$J");
        Serial.println("CycladicArt199&ItrAcidalcyC");
        Serial.println("GlassPyramid448%G");
        Serial.println("PaoloVeronese844!IesenoreVoloaP");
        Serial.println("MedievalWell882$AlleWlaveideM");
        Serial.println("JeanFrancoisChampollion706&F");
        Serial.println("CycladicArt250#B");
        Serial.println("JeanFrancoisChampollion831*AnoillopmahCsiocnarFnaeJ");
        Serial.println("RomanPortraiture589!EerutiartroPnamoR");
        Serial.println("WeddingAtCana979%JanaCtAgniddeW");
        Serial.println("RomanPortraiture525*JerutiartroPnamoR");
        Serial.println("TheodoreGericault161$A");
        Serial.println("JacquesLouisDavid622&EdivaDsiuoLseuqcaJ");
        Serial.println("WingedVictory243$A");
        Serial.println("VenusDeMilo992*ColiMeDsuneV");
        Serial.println("RomanPortraiture898&G");
        Serial.println("GreekAntiquities993@CseitiuqitnAkeerG");
        Serial.println("RomanPortraiture174#AerutiartroPnamoR");
        Serial.println("JacquesLouisDavid100@E");
        Serial.println("CoronationOfNapoleon484$D");
        Serial.println("VenusDeMilo326%C");
        Serial.println("WeddingAtCana660$DanaCtAgniddeW");
        Serial.println("CoronationOfNapoleon584&EnoelopaNfOnoitanoroC");
        Serial.println("EugeneDelacroix604!GxiorcaleDeneguE");
        Serial.println("Napoleon315$JnoelopaN");
        Serial.println("FrenchSculptures712&GserutplucShcnerF");
        Serial.println("WingedVictory977*CyrotciVdegniW");
        Serial.println("WeddingAtCana557#E");
        Serial.println("WeddingAtCana161!B");
        Serial.println("WingedVictory866!HyrotciVdegniW");
        Serial.println("LibertyLeadingThePeople263#IelpoePehTgnidaeLytrebiL");
        Serial.println("WeddingAtCana529@EanaCtAgniddeW");
        Serial.println("RomanPortraiture506*J");
        Serial.println("AncientEgypt655$ItpygEtneicnA");
        Serial.println("EugeneDelacroix899@D");
        Serial.println("MedievalWell358$D");
        Serial.println("JacquesLouisDavid906%IdivaDsiuoLseuqcaJ");
        Serial.println("RomanPortraiture813@C");
        Serial.println("PaoloVeronese870#B");
        Serial.println("FrenchSculptures511%FserutplucShcnerF");
        Serial.println("Napoleon152$C");
        Serial.println("CycladicArt618!AtrAcidalcyC");
        Serial.println("EugeneDelacroix434!G");
        Serial.println("TheodoreGericault239!D");
        Serial.println("GlassPyramid707@DdimaryPssalG");
        Serial.println("LeonardoDaVinci488#BicniVaDodranoeL");
        Serial.println("FrenchSculptures583$HserutplucShcnerF");
        Serial.println("RaftOfTheMedusa898*F");
        Serial.println("LeonardoDaVinci743$BicniVaDodranoeL");
        Serial.println("IslamicArt396#A");
        Serial.println("TheodoreGericault353@AtluacireGerodoehT");
        Serial.println("GreekAntiquities161%AseitiuqitnAkeerG");
        Serial.println("PaoloVeronese303#DesenoreVoloaP");
        Serial.println("PaoloVeronese621%I");
        Serial.println("WingedVictory982&ByrotciVdegniW");
        Serial.println("WeddingAtCana144#I");
        Serial.println("RomanPortraiture518*D");
        Serial.println("AncientEgypt563&J");
        Serial.println("CoronationOfNapoleon369%G");
        Serial.println("AncientEgypt414*B");
        Serial.println("CoronationOfNapoleon764@DnoelopaNfOnoitanoroC");
        Serial.println("CycladicArt811&DtrAcidalcyC");
        Serial.println("GrandGallery835@ByrellaGdnarG");
        Serial.println("JacquesLouisDavid462#D");
        Serial.println("Napoleon409!GnoelopaN");
        Serial.println("RomanPortraiture895&A");
        Serial.println("EugeneDelacroix354@FxiorcaleDeneguE");
        Serial.println("MedievalWell160$D");
        Serial.println("GrandGallery547$A");
        Serial.println("RomanPortraiture296%BerutiartroPnamoR");
        Serial.println("FrenchSculptures874@BserutplucShcnerF");
        Serial.println("Napoleon616*G");
        Serial.println("VenusDeMilo153%C");
        Serial.println("CoronationOfNapoleon647#J");
        Serial.println("PaoloVeronese606&H");
        Serial.println("RaftOfTheMedusa216&D");
        Serial.println("MedievalWell892@I");
        Serial.println("CoronationOfNapoleon880@I");
        Serial.println("WeddingAtCana215!C");
        Serial.println("EugeneDelacroix828$JxiorcaleDeneguE");
        Serial.println("CoronationOfNapoleon916@E");
        Serial.println("MonaLisa457#GasiLanoM");
        Serial.println("LeonardoDaVinci905@JicniVaDodranoeL");
        Serial.println("GlassPyramid141%EdimaryPssalG");
        Serial.println("GreekAntiquities244@I");
        Serial.println("RaftOfTheMedusa736&C");
        Serial.println("TheodoreGericault312&B");
        Serial.println("TheodoreGericault349#AtluacireGerodoehT");
        Serial.println("IslamicArt196$J");
        Serial.println("EugeneDelacroix671*BxiorcaleDeneguE");
        Serial.println("JeanFrancoisChampollion367%JnoillopmahCsiocnarFnaeJ");
        Serial.println("GlassPyramid474&H");
      } else {
        Serial.println("Permission denied: You don't have access to this file.");
      }
    }
  } else if (command.startsWith("mqtt ")) {
    String arg = command.substring(5);
    if (isAdmin) {
      if (arg == "--status") {
        Serial.println("MQTT connection status: " + String(mqttConnected ? "CONNECTED" : "DISCONNECTED"));
        Serial.println("Validation status: " + String(mqttValidated ? "VALIDATED" : "NOT VALIDATED"));
      } else if (arg == "--connect") {
        Serial.println("MQTT connection...");
        delay(1000);
        mqttConnected = true;
        Serial.println("MQTT connection established!");
        Serial.println("To validate, send your discovered device ID using: mqtt --validate <device_id>");
      } else if (arg.startsWith("--validate ")) {
        if (mqttConnected) {
          String providedDeviceId = arg.substring(String("--validate ").length());
          providedDeviceId.trim(); // Remove any leading/trailing whitespace
          
          Serial.println("Attempting to validate with device ID: " + providedDeviceId);
          delay(1000);

          if (providedDeviceId == String(MQTT_DEVICE_ID)) {
            mqttValidated = true;
            challengeCompleted = true;
            Serial.println("Device ID matched! Successfully validated!");
            Serial.println("MQTT validation complete.");
            Serial.println("FLAG: " + String(FLAG));
            showVictoryAnimation();
          } else {
            Serial.println("Error: Provided Device ID is incorrect.");
          }
        } else {
          Serial.println("Error: MQTT not connected. Use 'mqtt --connect' first.");
        }
      } else {
        Serial.println("Unknown mqtt command or incorrect usage. Available options:");
        Serial.println("  --status              : Check MQTT connection status");
        Serial.println("  --connect             : Connect to MQTT broker");
        Serial.println("  --validate <device_id> : Validate device with its ID");
      }
    } else {
      Serial.println("Permission denied: Administrative privileges required.");
    }
  } else if (command == "ls" || command == "ls -la") {
    if (isAdmin) {
      Serial.println("total 28");
      Serial.println("drwxr-xr-x 4 leonardo leonardo 4096 Mar 29 15:32 .");
      Serial.println("drwxr-xr-x 3 root    root    4096 Mar 28 09:15 ..");
      Serial.println("-rw------- 1 leonardo leonardo  220 Mar 28 09:15 .bash_history");
      Serial.println("-rw-r--r-- 1 leonardo leonardo 3526 Mar 28 09:15 .bashrc");
      Serial.println("drwx------ 2 leonardo leonardo 4096 Mar 28 09:15 .cache");
      Serial.println("-rw-r--r-- 1 leonardo leonardo   56 Mar 29 12:43 notes.txt");
      Serial.println("lrwxrwxrwx 1 leonardo leonardo   21 Mar 28 09:15 security.log -> /var/log/security.log");
    } else {
      Serial.println("total 16");
      Serial.println("drwxr-xr-x 3 visitor visitor 4096 Mar 29 14:22 .");
      Serial.println("drwxr-xr-x 3 root    root    4096 Mar 28 09:15 ..");
      Serial.println("-rw-r--r-- 1 visitor visitor 3526 Mar 28 09:15 .bashrc");
      Serial.println("-rw-r--r-- 1 visitor visitor 4096 Mar 28 09:15 pw.txt");
      Serial.println("-rw-r--r-- 1 visitor visitor  124 Mar 29 14:22 welcome.txt");
    }
  } else if (command == "cat notes.txt" && isAdmin) {
    Serial.println("Remember to rotate thread key daily.");
    Serial.println("Current key: Check security log");
  } else if (command == "cat welcome.txt" && !isAdmin) {
    Serial.println("Welcome to the Louvre Museum Security System");
    Serial.println("For visitor inquiries, please contact the information desk.");
    Serial.println("For administrative access, please contact the leonardo.");
  } else {
    Serial.println("Unknown command: " + command);
    Serial.println("Type 'help' for a list of available commands.");
  }
}

void printHelp() {
  Serial.println("Available commands:");
  Serial.println("  help      - Show this help message");
  Serial.println("  clear     - Clear the screen");
  Serial.println("  logout    - Log out of the system");
  Serial.println("  about     - Display system information");
  Serial.println("  art       - Display Louvre Pyramid ASCII art");
  Serial.println("  ls        - List files in current directory");
  Serial.println("  cat <file> - Display contents of a file");
  
  if (isAdmin) {
    Serial.println("  config    - Display system configuration (admin only)");
    Serial.println("  mqtt --status     - Check MQTT connection status (admin only)");
    Serial.println("  mqtt --connect    - Connect to MQTT broker (admin only)");
    Serial.println("  mqtt --validate <device_id>   - Validate device subscription (admin only)");
  }
}

void printConfig() {
  Serial.println("=== Louvre Security System Configuration ===");
  Serial.println("Version: " + String(CHALLENGE_VERSION));
  Serial.println("System: UART-based Authentication");
  Serial.println("Admin User: " + String(ADMIN_USERNAME));
  Serial.println("MQTT Device ID: " + String(MQTT_DEVICE_ID));
  Serial.println("Backup Schedule: Daily at 12:45");
  Serial.println("Log Rotation: Weekly");
  Serial.println("Sensor Check Interval: 2 hours");
  Serial.println("Emergency Protocol: Code 1789");
  Serial.println("NOTE: MQTT device ID can be found in security logs");
}

void clearScreen() {
  for (int i = 0; i < 25; i++) {
    Serial.println("");
  }
}

void printWelcome() {
  clearScreen();
  Serial.println("*****************************************************");
  Serial.println("*                                                   *");
  Serial.println("*             LOUVRE MUSEUM SECURITY                *");
  Serial.println("*                SYSTEM TERMINAL                    *");
  Serial.println("*                                                   *");
  Serial.println("*                                                   *");
  for (int i = 0; i < 8; i++) {
    Serial.print("*                  ");
    Serial.print(louvreArt[i]);
    Serial.println("                 *");
  }
  Serial.println("*                                                   *");
  Serial.println("*               RESTRICTED ACCESS ONLY              *");
  Serial.println("*                                                   *");
  Serial.println("*****************************************************");
  Serial.println("");
  
  printLoginPrompt();
}

void printLoginPrompt() {
  Serial.println("Please login to continue.");
  Serial.println("Usage: login username:password:" + currentCaptcha);
  Serial.print("> ");
}

void showVictoryAnimation() {
  display.clearDisplay();
  drawHeaderBar();
  delay(2000);
  
  for (int repeat = 0; repeat < 10; repeat++) {
    for (int i = 0; i < 30; i++) {
      display.clearDisplay();
      drawHeaderBar();
      display.drawBitmap(0, 12, bufferAnimation[i], 128, 64, 1);
      display.display();
      delay(50);
    }
  }
}

void updateDisplayStatus() {
  display.clearDisplay();
  
  drawHeaderBar();
  
  display.setCursor(0, 15);
  display.println("UART Security");
  display.println("Baud: 9600");
  
  if (isAuthenticated) {
    if (isAdmin) {
      display.println("\nAdmin Connected");
    } else {
      display.println("\nVisitor Connected");
    }
  } else {
    display.println("\nAwaiting login...");
    display.println("Captcha: " + currentCaptcha);
  }
  
  if (challengeCompleted) {
    display.println("\nChallenge Complete!");
  }
  
  display.display();
}

void drawHeaderBar() {
  display.fillRect(0, 0, display.width(), 10, SSD1306_WHITE);
  display.setTextColor(SSD1306_BLACK);
  display.setCursor(15, 1);
  display.print("LOUVRE SECURITY");
  display.setTextColor(SSD1306_WHITE);
}

String generateCaptcha() {
  String captcha = "";
  const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  
  randomSeed(millis());
  
  for (int i = 0; i < CAPTCHA_LENGTH; i++) {
    int index = random(0, sizeof(charset) - 1);
    captcha += charset[index];
  }
  
  return captcha;
}

void displayChallengeInfo() {
  display.clearDisplay();
  display.setTextSize(1);
  
  drawHeaderBar();
  
  display.setCursor(0, 15);
  display.println(CHALLENGE_TITLE);
  display.print("Version: ");
  display.println(CHALLENGE_VERSION);
  
  display.display();
}