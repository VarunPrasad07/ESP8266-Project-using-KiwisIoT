
#include <ESP8266WiFi.h>
#include <KiwisIoT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// ------------ WIFI SETTINGS ------------
 char* ssid  = "YOUR_WIFI_SSID";
 char* pass  = "YOUR_WIFI_PASSWORD";
 char* topic = "YOUR_DASHBOARD_TOPIC";

KiwisIoT kiwisiot(ssid, pass, topic);

// ------------ PIN CONFIGURATION ------------
#define SOIL_PIN A0
#define DS18B20_PIN D4
#define RELAY_PIN D5

// ------------ LCD CONFIGURATION ------------
#define LCD_ADDRESS 0x27

LiquidCrystal_I2C lcd(LCD_ADDRESS, 16, 2);

// ------------ DS18B20 CONFIGURATION ------------
OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);

// ------------ SOIL MOISTURE SETTINGS ------------
// Dry soil = 0%
// Wet soil = 100%
int dryThreshold = 30;

// Calibration endpoints
// Change these values according to your sensor readings.
const int SOIL_DRY_VALUE = 1023;
const int SOIL_WET_VALUE = 0;

// ------------ RELAY CONFIGURATION ------------
// Active-LOW relay
#define RELAY_ON  LOW
#define RELAY_OFF HIGH

// ------------ TIMING ------------
unsigned long lastSend = 0;
const unsigned long interval = 3000;

// ------------ SENSOR VARIABLES ------------
int soilValue = 0;
int soilMoisturePercent = 0;

bool soilDry = false;
bool pumpState = false;

float temperatureC = DEVICE_DISCONNECTED_C;
bool temperatureValid = false;

// ------------ SETUP ------------
void setup()
{
    Serial.begin(115200);

    // Initialize I2C LCD
    Wire.begin(D2, D1);  // SDA = D2, SCL = D1

    lcd.init();
    lcd.backlight();
    lcd.clear();

    // Initialize DS18B20
    sensors.begin();
    sensors.setResolution(11);
    sensors.setWaitForConversion(false);

    // Initialize relay
    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, RELAY_OFF);

    // Startup LCD
    lcd.setCursor(0, 0);
    lcd.print("Auto Irrigation");

    lcd.setCursor(0, 1);
    lcd.print("System Starting");

    delay(2000);
    lcd.clear();

    // Initialize KiwisIoT
    kiwisiot.begin();

    // Start first temperature conversion
    sensors.requestTemperatures();

    Serial.println();
    Serial.println("Automatic Irrigation System");
    Serial.println("---------------------------");
    Serial.println("System Started");
}

// ------------ MAIN LOOP ------------
void loop()
{
    // Maintain KiwisIoT communication
    kiwisiot.run();

    unsigned long now = millis();

    if (now - lastSend >= interval)
    {
        lastSend = now;

        // ------------ READ SOIL SENSOR ------------
        soilValue = analogRead(SOIL_PIN);

        // Convert raw ADC reading into percentage
        // Dry endpoint = 1023 -> 0%
        // Wet endpoint = 0    -> 100%
        soilMoisturePercent = map(
            soilValue,
            SOIL_DRY_VALUE,
            SOIL_WET_VALUE,
            0,
            100
        );

        // Limit percentage to 0-100
        soilMoisturePercent = constrain(
            soilMoisturePercent,
            0,
            100
        );

        // Determine soil condition
        soilDry = (soilMoisturePercent < dryThreshold);

        // ------------ READ TEMPERATURE ------------
        float temp = sensors.getTempCByIndex(0);

        if (temp != DEVICE_DISCONNECTED_C &&
            temp >= -55.0 &&
            temp <= 125.0)
        {
            temperatureC = temp;
            temperatureValid = true;
        }
        else
        {
            temperatureC = DEVICE_DISCONNECTED_C;
            temperatureValid = false;
        }

        // Start next temperature conversion
        sensors.requestTemperatures();

        // ------------ AUTOMATIC PUMP CONTROL ------------
        if (soilDry)
        {
            digitalWrite(RELAY_PIN, RELAY_ON);
            pumpState = true;
        }
        else
        {
            digitalWrite(RELAY_PIN, RELAY_OFF);
            pumpState = false;
        }

        // ------------ LCD ROW 1: SOIL MOISTURE ------------
        lcd.setCursor(0, 0);

        char soilLine[17];

        snprintf(
            soilLine,
            sizeof(soilLine),
            "Soil:%3d%% %s",
            soilMoisturePercent,
            soilDry ? "DRY" : "WET"
        );

        lcd.print(soilLine);

        // Clear remaining characters
        lcd.print("   ");

        // ------------ LCD ROW 2: TEMPERATURE ------------
        lcd.setCursor(0, 1);

        if (temperatureValid)
        {
            char tempLine[17];

            snprintf(
                tempLine,
                sizeof(tempLine),
                "Temp:%5.1f C",
                temperatureC
            );

            lcd.print(tempLine);
            lcd.print("   ");
        }
        else
        {
            lcd.print("Temp: Sensor Err");
        }

        // ------------ SERIAL MONITOR ------------
        Serial.println("---------------------------");

        Serial.print("Raw Soil Value: ");
        Serial.println(soilValue);

        Serial.print("Soil Moisture: ");
        Serial.print(soilMoisturePercent);
        Serial.println("%");

        Serial.print("Soil Status: ");
        Serial.println(soilDry ? "DRY" : "WET");

        Serial.print("Pump Status: ");
        Serial.println(pumpState ? "ON" : "OFF");

        Serial.print("Temperature: ");

        if (temperatureValid)
        {
            Serial.print(temperatureC, 1);
            Serial.println(" C");
        }
        else
        {
            Serial.println("Sensor Error");
        }

        // ------------ SEND DATA TO KIWISIOT ------------

        // Channel 0: Soil moisture percentage (0-100)
        kiwisiot.send("0", soilMoisturePercent);

        // Channel 1: Temperature in Celsius
        if (temperatureValid)
        {
            kiwisiot.send("1", temperatureC);
        }

        // Channel 2: Soil status
        // 1 = DRY, 0 = WET
        kiwisiot.send("2", soilDry ? 1 : 0);

        // Channel 3: Pump status
        // 1 = ON, 0 = OFF
        kiwisiot.send("3", pumpState ? 1 : 0);

        Serial.println("Data sent to KiwisIoT");
    }
}
