#include <LiquidCrystal.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <time.h>

// WiFi credentials
const char* ssid = "EDU Students";
const char* password = "campus@334422";

// LCD (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(D1, D2, D5, D6, D7, D8);

// Web server
ESP8266WebServer server(80);

// Time client
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 6 * 3600, 60000);  // GMT+6

// State
bool newMessageReceived = false;
String newMessage = "";
int newRepeatCount = 1;
bool wifiDisplayed = false;
bool welcomeShown = false;
unsigned long messageCooldown = 0;
const int scrollSpeed = 300;  // Optimal scroll speed (ms per step)
const int slowerScrollSpeed = 500;  // Slower speed for the welcome message
const int buzzerPin = D3;     // Buzzer pin
const int backlightPin = D4;  // LCD backlight pin

void setup() {
  Serial.begin(115200);
  lcd.begin(16, 2);
  lcd.print("Connecting...");

  // Initialize pins
  pinMode(backlightPin, OUTPUT);
  digitalWrite(backlightPin, HIGH);
  pinMode(buzzerPin, OUTPUT);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.print(".");
    if (millis() - start > 20000) {
      lcd.clear();
      lcd.print("WiFi Failed");
      while(1) delay(1000);
    }
  }

  lcd.clear();
  lcd.print("WiFi Connected!");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  delay(3000);
  wifiDisplayed = true;

  // Start services
  timeClient.begin();
  server.on("/", handleRoot);
  server.on("/submit", HTTP_GET, handleSubmit);
  server.begin();

  Serial.println("\nSystem Ready");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void handleRoot() {
  server.send(200, "text/plain", "ESP8266 Notice Board");
}

void handleSubmit() {
  String message = server.arg("msg");
  int repeatCount = server.arg("count").toInt();
  
  // Debug output
  Serial.printf("\nReceived: '%s' x%d\n", message.c_str(), repeatCount);

  // Validate input
  message.trim();
  if (repeatCount < 1) repeatCount = 1;

  // Update display state
  newMessage = message;
  newRepeatCount = repeatCount;
  newMessageReceived = true;

  // Visual feedback
  tone(buzzerPin, 1500, 200);
  
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, "text/plain", "Message received: " + message);
}

void scrollMessage(const String &msg, int count, int scrollSpeed) {
  String displayMsg = " " + msg + " "; // Padding for smooth scrolling
  int totalLength = displayMsg.length();
  
  if (totalLength < 16) totalLength = 16; // Ensure we always scroll at least one step

  for (int r = 0; r < count; r++) {
    // Static display for short messages
    if (msg.length() <= 16) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(msg);
      lcd.setCursor(0, 1);
      lcd.print("Repeat: " + String(r + 1) + "/" + String(count));
      delay(2000);  // Added a delay to prevent immediate message change
    } 
    
    // Scrolling for long messages
    else {
      for (int pos = 0; pos <= max(0, totalLength - 16); pos++) {
        // Exit scroll early if a new message is submitted
        if (newMessageReceived) return;

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(displayMsg.substring(pos, pos + 16));
        lcd.setCursor(0, 1);
        lcd.print("Repeat: " + String(r + 1) + "/" + String(count));
        delay(scrollSpeed);
      }

      // Extra pause at end of scroll
      delay(5000);
    }

    // Visual feedback for each repeat
    digitalWrite(backlightPin, LOW);
    tone(buzzerPin, 1000, 100);
    delay(100);
    digitalWrite(backlightPin, HIGH);
  }
}

void displayTime() {
  timeClient.update();
  unsigned long epoch = timeClient.getEpochTime();
  time_t rawTime = static_cast<time_t>(epoch);
  struct tm *tmStruct = localtime(&rawTime);

  // Format time (12-hour with AM/PM)
  int hour = tmStruct->tm_hour % 12;
  if (hour == 0) hour = 12;
  char timeStr[17];
  snprintf(timeStr, sizeof(timeStr), "%02d:%02d:%02d %s",
           hour, tmStruct->tm_min, tmStruct->tm_sec,
           tmStruct->tm_hour >= 12 ? "PM" : "AM");

  // Format date
  char dateStr[17];
  snprintf(dateStr, sizeof(dateStr), "%02d/%02d/%04d",
           tmStruct->tm_mday, tmStruct->tm_mon + 1, tmStruct->tm_year + 1900);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(timeStr);
  lcd.setCursor(0, 1);
  lcd.print(dateStr);
}

void loop() {
  server.handleClient();
  static unsigned long lastTimeUpdate = 0;

  if (wifiDisplayed) {
    // Show welcome message once with slower scroll speed
    if (!welcomeShown) {
      scrollMessage("Welcome to Notice Board!", 2, slowerScrollSpeed);
      welcomeShown = true;
    }

    // Handle new messages
    if (newMessageReceived) {
      // Debugging the message being processed
      Serial.println("Processing new message: " + newMessage);
      scrollMessage(newMessage, newRepeatCount, scrollSpeed);
      newMessageReceived = false;
      messageCooldown = millis() + 5000; // Show time after 5s
    } 
    // Show time when no messages
    else {
      if (millis() > messageCooldown && millis() - lastTimeUpdate >= 5000) {
        displayTime();
        lastTimeUpdate = millis();
      }
    }
  }
}
