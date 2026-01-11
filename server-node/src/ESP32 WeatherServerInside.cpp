#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <secrets.h>
#include "time.h"

// ---------- WiFi ----------
const char* ssid     = WIFI_SSID;
const char* password = WIFI_PASSWORD;

// ---------- Web Server ----------
WebServer server(80);

// ---------- OLED ----------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------- Latest Readings ----------
float temperature = 0;
float humidity    = 0;
float pressure    = 0;
float battery     = 0;

unsigned long lastUpdateMillis = 0;

// ---------- LED Gauge ----------
int ledPins[] = {2, 4, 5, 18, 19, 25, 2};
const int ledCount = sizeof(ledPins)/sizeof(ledPins[0]);

// ---------- Function Prototypes ----------
void handleUpdate();
void updateOLED();
void updateLEDGauge(float temp);

void setup() {
  Serial.begin(115200);

  // LED pins
  for (int i=0;i<ledCount;i++){
    pinMode(ledPins[i], OUTPUT);
    digitalWrite(ledPins[i], LOW);
  }

  // OLED init
  Wire.begin();
  if(!display.begin(SSD1306_SWITCHCAPVCC,0x3C)){
    Serial.println("OLED failed");
    while(true);
  }
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  // WiFi
  WiFi.mode(WIFI_STA);
  //WiFi.config(local_IP, gateway, subnet, dns);
  WiFi.begin(ssid,password);
  Serial.print("Connecting to WiFi");
  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected: " + WiFi.localIP().toString());

  // ---------- Time (NTP) ----------
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  struct tm timeinfo;
  if (getLocalTime(&timeinfo)) {
    Serial.println("Time synchronized");
  } else {
   Serial.println("Failed to obtain time");
  }
  // Server route
  server.on("/update", HTTP_POST, handleUpdate);
  server.on("/update", HTTP_GET, handleUpdate);  // temporary GET for browser testing

  server.on("/", HTTP_GET, []() {
  unsigned long ageSeconds = 0;
  if (lastUpdateMillis > 0) {
    ageSeconds = (millis() - lastUpdateMillis) / 1000;
  }
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta charset='UTF-8'>";
  html += "<meta http-equiv='refresh' content='5'>";  // auto-refresh every 5s
  html += "<title>ESP32 Weather Server</title>";
  html += "<style>";
  html += "body { font-family: Arial; background:#111; color:#eee; padding:20px; }";
  html += "h1 { color:#6cf; }";
  html += ".box { font-size:1.2em; margin:10px 0; }";
  html += ".stamp { color:#aaa; font-size:0.9em; margin-top:15px; }";
  html += "</style></head><body>";

  html += "<h1>ESP32 Weather Server</h1>";
  html += "<div class='box'>🌡️ Temperature: " + String(temperature, 1) + " °F</div>";
  html += "<div class='box'>💧 Humidity: " + String(humidity, 1) + " %</div>";
  html += "<div class='box'>📉 Pressure: " + String(pressure, 1) + " hPa</div>";
  html += "<div class='box'>🔋 Battery: " + String(battery, 2) + " V</div>";
  if (lastUpdateMillis > 0) {
    html += "<div class='stamp'>Last update: ";
    html += String(ageSeconds);
    html += " seconds ago</div>";
  } else {
    html += "<div class='stamp'>No data received yet</div>";
  }
  html += "</body></html>";

  server.send(200, "text/html", html);
});

  server.begin();

  // ---------- Startup Screen ----------
display.clearDisplay();

// Title
display.setTextSize(2);
display.setCursor(0,0);
display.println("WEATHER");

// Subtitle
display.setTextSize(1);
display.setCursor(0,18);
display.println("Telemetry Node");

// Divider
display.drawLine(0,28,127,28,SSD1306_WHITE);

// Status
display.setCursor(0,32);
display.println("Status: ONLINE");

// IP Address
display.setCursor(0,44);
display.print("IP ");
display.println(WiFi.localIP());

display.display();

}

void loop() {
  server.handleClient();
}

void handleUpdate() {
  // Read values from form-urlencoded
  if (server.hasArg("temp")) temperature = server.arg("temp").toFloat();
  if (server.hasArg("hum"))  humidity    = server.arg("hum").toFloat();
  if (server.hasArg("pres")) pressure    = server.arg("pres").toFloat();
  if (server.hasArg("batt")) battery     = server.arg("batt").toFloat();

  lastUpdateMillis = millis();

  Serial.print("Raw args: ");
  for(int i=0;i<server.args();i++){
  Serial.print(server.argName(i)); Serial.print("="); Serial.print(server.arg(i)); Serial.print(" ");
  }
  Serial.println();

  Serial.println("Received data:");
  Serial.print("Temp: "); Serial.println(temperature);
  Serial.print("Hum:  "); Serial.println(humidity);
  Serial.print("Pres: "); Serial.println(pressure);
  Serial.print("Batt: "); Serial.println(battery);

  updateOLED();
  updateLEDGauge(temperature);

  server.send(200,"text/plain","OK");
}

void updateOLED() {
  display.clearDisplay();

  // ---------- TOP: Temperature ----------
  display.setTextSize(2);
  display.setCursor(0,0);
  display.print("TEMP ");
  display.print((int)temperature); // whole number only
  display.print("F");

// Divider
  display.drawLine(0,18,127,18,SSD1306_WHITE);


  // ---------- Battery Gauge ----------
  float minV = 3.0;  // minimum battery voltage
  float maxV = 4.2;  // maximum battery voltage
  float battWidth = 40; // width of the gauge in pixels
  float battHeight = 6; // height of the gauge in pixels

  // map voltage to width
  float fillWidth = ((battery - minV) / (maxV - minV)) * battWidth;
  if(fillWidth < 0) fillWidth = 0;
  if(fillWidth > battWidth) fillWidth = battWidth;

  // position: right after battery text
  int x = 50 + String(battery,2).length() * 6; // rough estimate
  int y = 34; // same row as battery text

  // outline
  display.drawRect(x, y, battWidth, battHeight, SSD1306_WHITE);

  // fill
  display.fillRect(x, y, fillWidth, battHeight, SSD1306_WHITE);

  // ---------- BOTTOM: Date + Time with blinking colon ----------
  struct tm timeinfo;
  display.setCursor(0,52);
  if (getLocalTime(&timeinfo)) {
    const char* months[] = {
      "Jan","Feb","Mar","Apr","May","Jun",
      "Jul","Aug","Sep","Oct","Nov","Dec"
    };

    // blinking colon
    char colon = (timeinfo.tm_sec % 2) ? ':' : ' ';

    display.printf(
      "%02d %s %04d  %02d%c%02d",
      timeinfo.tm_mday,
      months[timeinfo.tm_mon],
      timeinfo.tm_year + 1900,
      timeinfo.tm_hour,
      colon,
      timeinfo.tm_min
    );
  } else {
    display.print("Time not set");
  }

  display.display();
}

void updateLEDGauge(float temp){
  int level = 0;
  if (temp<40) level=1;
  else if (temp<50) level=2;
  else if (temp<60) level=3;
  else if (temp<70) level=4;
  else if (temp<80) level=5;
  else if (temp<90) level=6;
  else level=7;

  for(int i=0;i<ledCount;i++){
    digitalWrite(ledPins[i], i<level ? HIGH : LOW);
  }
}
