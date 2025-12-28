# Toddler Color Game - ESP8266 Button Controller

MQTT publisher for IoT toddler learning game. Press the correct color button to light up LEDs and trigger sounds on the Pi Zero W subscriber.

## Hardware

- **NodeMCU/ESP8266** - WiFi-enabled microcontroller
- **4 Buttons** - Red, Blue, Green, Yellow (connected to D1-D4)
- **4 LEDs** - Status indicators (connected to D5-D8)

## Wiring

| Button Color | Button Pin | LED Pin |
|--------------|------------|---------|
| Red          | D1         | D5      |
| Blue         | D2         | D6      |
| Green        | D3         | D7      |
| Yellow       | D4         | D8      |

- Buttons: Connect between pin and GND (using INPUT_PULLUP)
- LEDs: Connect through 220Ω resistor to GND

## Setup

### 1. Install Arduino IDE
- Download from https://www.arduino.cc/en/software

### 2. Install ESP8266 Board Support
1. Open Arduino IDE
2. File → Preferences
3. Add to "Additional Board Manager URLs":
   ```
   http://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
4. Tools → Board → Board Manager
5. Search "esp8266" and install

### 3. Install Libraries
Tools → Manage Libraries, install:
- **PubSubClient** (by Nick O'Leary)

### 4. Configure

```bash
cp config.h.example config.h
# Edit config.h with your WiFi and MQTT broker details
```

**config.h:**
```cpp
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#define MQTT_BROKER "192.168.1.200"  // Your cluster IP
#define MQTT_TOPIC "button-pressed"
```

### 5. Upload to NodeMCU
1. Connect NodeMCU via USB
2. Tools → Board → NodeMCU 1.0 (ESP-12E Module)
3. Tools → Port → (select your COM/USB port)
4. Sketch → Upload

## Usage

1. Power on the NodeMCU
2. One LED will light up (the active button)
3. Press that button → publishes MQTT message → LED strip lights up
4. Next random LED lights up
5. Repeat!

## MQTT Messages

**Topic:** `button-pressed`  
**Payload:** `red`, `blue`, `green`, or `yellow`

## Architecture

This controller is part of a larger IoT system:

```
NodeMCU (Publisher)
   ↓ MQTT
HashiCorp Cluster (MQTT Broker via Nomad)
   ↓ MQTT
Pi Zero W (Subscriber - LED Strip + Audio)
```

## Development

### Serial Monitor
Open Tools → Serial Monitor (115200 baud) to see debug output:
```
Toddler Color Game - Booting...
Connecting to WiFi....
WiFi connected
IP: 192.168.1.210
Connecting to MQTT...connected
Starting with button: red
Publishing to: button-pressed
```

### Troubleshooting

**WiFi won't connect:**
- Check SSID/password in config.h
- Ensure 2.4GHz WiFi (ESP8266 doesn't support 5GHz)

**MQTT won't connect:**
- Verify MQTT broker IP
- Check network connectivity: `ping 192.168.1.200`
- Ensure Nomad MQTT job is running on cluster

**Buttons not working:**
- Check wiring (buttons should pull pin to GND)
- Verify pinMode(INPUT_PULLUP) is set

## Related Projects

- **homelab-cluster** - HashiCorp cluster running MQTT broker
- **pizero-led-lightshow** - LED strip subscriber

## License

MIT
