# ESP8266 Project using KiwisIoT

A collection of ESP8266 / NodeMCU IoT projects built on the
[Kiwis IoT](https://kiwiot.com/) cloud platform — WiFi setup, MQTT topic wiring,
Kiwis dashboard widgets, plus a few standalone sketches.

Most sketches follow the same pattern: read a sensor, `kiwisiot.send("<widgetID>", value)`,
and drive a relay or buzzer from a value received back from the dashboard via `onReceive()`.

## Projects

### Connected to Kiwis IoT

| Project | Hardware / sensors | Publishes |
| --- | --- | --- |
| `ammonia_gas_detection` | MQ2 gas, DHT11, SSD1306 OLED | temperature, humidity, gas value, danger flag |
| `analog_sensor_for_kiwisiot` | any analog input | analog value (minimal send example) |
| `aquapulse` | capacitive soil moisture, DS18B20, 16x2 I2C LCD | soil %, temperature, dry flag, pump state |
| `baby_cry` | sound level, DHT11, 16x2 I2C LCD | sound level, temperature, humidity + alarm widgets |
| `battery_temperature_monitroing` | DS18B20, 0-25V divider, ACS712, MQ gas | temperature, voltage, current, status, gas |
| `digital_sensor` | any digital input | digital state (minimal send example) |
| `door_lock_security` | PIR (D1) | motion state + alarm widgets |
| `driver_project` | IR receiver (D5), relay, LED | drowsiness flag |
| `example_for_kiwisiot_control` | — | minimal `onReceive()` control example |
| `gas_and_fire_detection` | MQ2, flame sensor (D5), DHT11 | temperature, humidity, gas, alert state |
| `health_monitor` | MAX30105, DHT11 | heart rate, SpO2, temperature, humidity |
| `home_automation` | relays | device on/off + alarm widgets |
| `industrial_project_iot` | Arduino UNO front-end + MQ gas, sound/noise, DHT11 | gas, noise, temperature, humidity |
| `iot_car` | motor drivers, light, horn | control via dashboard |
| `iot_energy_meter` | ACS712 current, voltage RMS | voltage, current, power, energy, cost, gas, daily/monthly estimate |
| `kiwisiot_sensor` | any digital input | base sensor example |
| `smart_agricultur` | soil moisture, LDR, DHT11 | soil, light, temperature, humidity |
| `smart_dustpin` | servo, 16x2 I2C LCD | bin state + alarm widgets |
| `smart_dustpin_gas` | metal detection, MQ gas, DHT11 | bin fill %, gas, temperature, metal/non-metal |
| `soil_moisture_` | soil moisture probe | moisture percentage |
| `street_ad` | HC-SR04, LDR, relay | presence, distance, light, relay (uses `init()` + `setCallback()` + `publish()`) |
| `vsb_home_automation` | relays | device on/off + alarm widgets |
| `water_level_monitoring` | HC-SR04, pump | distance, water value + alarm widgets |
| `weather_monitor` | DHT11, MQ gas, rain sensor, 16x2 I2C LCD | temperature, humidity, gas, rain status |

### Standalone (no Kiwis IoT)

| Project | Hardware / sensors | Notes |
| --- | --- | --- |
| `battery_bludging` | DS18B20, HC-SR04, 16x2 I2C LCD | temperature + distance on LCD |
| `heart_rate_sensor` | pulse sensor on `A0` | threshold-based beat detection, 115200 baud |
| `solar_monitoring` | 2x DC motor (L298N), HC-SR04 | panel tracker |
| `SOLAR_POWERED_SMART_IRRIGATION___ENVIRONMENT_MONITORING` | soil, air quality, DHT11, LCD | auto-pump when soil dries |

## Setup

1. Install the [Arduino IDE](https://www.arduino.cc/en/software).
2. Add the ESP8266 board package via **File > Preferences > Additional Board Manager URLs**:
   ```
   https://arduino.esp8266.com/stable/package_esp8266com_index.json
   ```
3. **Library Manager** -> install `KiwisIoT` by Kiwistron. The sketches also use
   `DHT`, `LiquidCrystal_I2C`, `Adafruit_SSD1306`, `Adafruit_GFX`, `OneWire`,
   `DallasTemperature`, `Servo`, and `MAX30105` (health monitor only).
4. Create a free account and a product at [account.kiwiot.com](https://account.kiwiot.com/).
5. Open a `.ino` and set your credentials at the top of the sketch:

   ```cpp
   const char* ssid  = "YOUR_WIFI_SSID";
   const char* pass  = "YOUR_WIFI_PASSWORD";
   const char* topic = "YOUR_DASHBOARD_TOPIC";
   ```

   `topic` is the dashboard topic ID (`dash_...`) from the product setup page.
6. Select your ESP8266 board and port, then upload at **115200** baud.

## Kiwis IoT topic layout

| Channel | Direction | Purpose |
| --- | --- | --- |
| `Kiwis/<product>/d/<deviceName>` | device -> cloud | sensor data (`send()`) |
| `Kiwis/<product>/c/<command>/<deviceName>` | cloud -> device | commands (`onReceive()`) |
| `Kiwis/<product>/s/<deviceName>` | device -> cloud | status |

Add a **Data topic** widget on the dashboard for each ID you `send()`, and a
**Command topic** widget for each command you handle in `onReceive()`.

## Notes

- Credentials in this repo are placeholders. Keep your own WiFi password and
  dashboard topic out of any public fork.
- Some sketches (notably `iot_energy_meter`, `battery_bludging`) use Arduino
  UNO / ESP32-style pin numbers rather than ESP8266 `D#` labels — check the pin
  map against your board before uploading.
