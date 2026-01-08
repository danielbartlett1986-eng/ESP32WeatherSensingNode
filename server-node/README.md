Here’s a ready-to-paste, visually appealing README.md just for your Server-Node code — full of emoji icons, clean Markdown styling, and concise explanations ideal for GitHub presentation:

🖥️ ESP32 Weather Server Node
The ESP32 Weather Server Node is the receiving and display hub of a two-part weather-sensing system. It collects environmental readings sent by a remote ESP32 Sensor Node, updates a live OLED display, drives a temperature LED gauge, and hosts a Wi-Fi web dashboard for real-time monitoring 🌤️.

⚙️ Overview
text
[SENSOR NODE] ---> [SERVER NODE]
 BME280 Data       OLED Display + Web Dashboard
This node acts as a mini web server that receives environmental data such as temperature, humidity, pressure, and battery level from the client node via HTTP requests.
It then displays that data locally and online — simple, elegant, and completely cloud-free.

🌡️ Key Features
📡 Wi-Fi-enabled data receiver for the sensor node

🖥️ Real-time OLED display for temperature, humidity, pressure, and battery voltage

🌈 LED temperature gauge that lights up more LEDs as the temperature rises

🌐 Web dashboard with auto-refresh and friendly icons

🧠 Built-in micro web server (port 80) for easy access on the local network

🚦 Instant response to /update POST or GET requests from the sensor node

🧰 Hardware Requirements
Component	Description
🧩 ESP32 Development Board	Hosts the local server
🖼️ SSD1306 OLED Display	128×64 I²C display for sensor data
🔆 7 LEDs + Resistors	Temperature gauge visualization
⚡ Power Source	USB or Li-ion battery
🔌 Pin Setup
Function	Pin(s)
LED Outputs	2, 4, 5, 18, 19, 21, 22
OLED (I²C)	SDA/SCL default pins
Wi-Fi	Configured in secrets.h
📡 Communication Details
The Sensor Node periodically sends a POST request with environmental readings to the endpoint:

text
/update
Example HTTP Payload:
text
temp=72.4&hum=43.1&pres=1012.6&batt=3.91
Upon receiving data:

The server parses the readings.

Updates the OLED display values.

Adjusts the LED temperature bar level.

Displays the latest data on its web dashboard.

🌐 Web Dashboard
Open your browser to access:

text
http://<server_ip>/
You’ll see an elegant live page automatically refreshing every few seconds:

🌡️ Temperature: XX.X °F

💧 Humidity: XX.X %

📉 Pressure: XXXX.X hPa

🔋 Battery: X.XX V

⏱️ Last update: seconds since last transmission

🪛 Configuration
Update your secrets.h file before uploading:

```cpp
#define WIFI_SSID "YourNetworkName"
#define WIFI_PASSWORD "YourPassword"
Optional: You can assign a static IP if desired. Then upload the code to your ESP32 server and open the Serial Monitor to note the network IP.

🌈 LED Temperature Gauge Logic
Temperature (°F)	LEDs Lit
< 40	1
40–49	2
50–59	3
60–69	4
70–79	5
80–89	6
≥ 90	7
Each additional LED represents a warmer temperature — a simple but intuitive visual cue.

🧭 Use Cases
Local weather or indoor climate station

Smart home environmental display node

IoT classroom project

Pair with remote battery-powered weather sensors

⚡ Quick Summary
✨ Sensor Node: Collects data (BME280 + Battery)
📶 Server Node: Displays and visualizes results locally
🌐 Result: A compact, wireless, cloud-free weather station