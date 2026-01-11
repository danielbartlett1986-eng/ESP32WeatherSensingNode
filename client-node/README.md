# ESP32 Weather Sensor Client Node

This is the **ESP32 client node** for the ESP32 Weather Sensing project.  
It reads environmental data from a **BME280 sensor**, measures battery voltage, sends the data to a server over HTTP, and then enters **deep sleep** to conserve power.

---

## Features

- 🌡 Temperature (°F)
- 💧 Humidity (%)
- 🌬 Pressure (hPa)
- 🔋 Battery voltage monitoring
- 📡 WiFi connectivity
- 🌙 Deep sleep for low power operation
- 🔁 Periodic reporting to a central server

---

## Hardware Requirements

- ESP32 development board
- BME280 sensor (I2C)
- Battery with voltage divider
- WiFi network

---

## Pin Configuration

| Function | ESP32 Pin |
|--------|----------|
| SDA    | GPIO 26  |
| SCL    | GPIO 27  |
| Battery Sense | GPIO 33 |

Battery voltage is measured using a resistor divider:
- R1 = 27kΩ
- R2 = 100kΩ

---

## Software Dependencies

Install the following libraries via Arduino Library Manager or PlatformIO:

- `WiFi`
- `HTTPClient`
- `Wire`
- `Adafruit BME280`
- `Adafruit Unified Sensor`

---

## Configuration (`secrets.h`)

Create a file called `secrets.h` in the `include/` directory:

```cpp
#pragma once

#define WIFI_SSID     "your_wifi_name"
#define WIFI_PASSWORD "your_wifi_password"

static const char* serverIP = "192.168.1.100";  // Server IP
⚠️ secrets.h should not be committed to GitHub.

How It Works
ESP32 boots and initializes the BME280 sensor

Connects to WiFi (15 second timeout)

Reads:

Temperature (converted to °F)

Humidity

Pressure

Battery voltage

Sends data to the server using an HTTP POST request:

http://<serverIP>/update
Enters deep sleep for 5 minutes

Repeats on wake-up

HTTP Payload Format
The client sends data as URL-encoded form data:

temp=<temperature_f>
hum=<humidity>
pres=<pressure>
batt=<battery_voltage>
Example:

temp=72.34&hum=41.2&pres=1013.6&batt=3.97
Power Management
Deep sleep interval: 300 seconds (5 minutes)

WiFi failures trigger an early sleep retry

Sensor failures trigger a short sleep recovery

You can adjust sleep duration here:

#define SLEEP_SECONDS 300
Serial Debug Output
At boot, the ESP32 prints sensor readings and POST status to the serial monitor at 115200 baud.

Notes
BME280 I2C address is set to 0x76

Battery calibration may need adjustment depending on resistor tolerances

Designed for low-power, unattended outdoor operation

License

MIT License (or update as needed)