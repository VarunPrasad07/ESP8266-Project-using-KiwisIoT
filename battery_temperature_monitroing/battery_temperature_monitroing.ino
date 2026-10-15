#include <KiwisIoT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// =====================================================
// KiwisIoT CONFIGURATION
// =====================================================

 char* ssid  = "YOUR_WIFI_SSID";
 char* pass  = "YOUR_WIFI_PASSWORD";
 char* topic = "YOUR_DASHBOARD_TOPIC";

KiwisIoT kiwisiot(ssid, pass, topic);

// =====================================================
// PIN CONFIGURATION
// =====================================================

#define DS18B20_PIN 4
#define VOLTAGE_PIN 34
#define CURRENT_PIN 35
#define GAS_PIN     32

#define GREEN_LED 25
#define RED_LED   26
#define BUZZER    27

// =====================================================
// DS18B20
// =====================================================

OneWire oneWire(DS18B20_PIN);
DallasTemperature sensors(&oneWire);

// =====================================================
// BATTERY LIMITS
// =====================================================

#define MAX_TEMPERATURE 60.0
#define MIN_TEMPERATURE 0.0

#define MAX_VOLTAGE 9.0
#define MIN_VOLTAGE 3.0

#define MAX_CURRENT 5.0

// =====================================================
// GAS SENSOR LIMIT
// Raw ADC value: 0 - 4095
// Adjust according to your MQ sensor
// =====================================================

#define MAX_GAS_VALUE 2000

// =====================================================
// VOLTAGE SENSOR
// Example: 0-25V voltage sensor
// =====================================================

#define VOLTAGE_DIVIDER_RATIO 5.0

// =====================================================
// CURRENT SENSOR
// Example: ACS712 5A
// Sensitivity = 185 mV/A
// =====================================================

#define ACS712_SENSITIVITY 0.185
#define ACS712_ZERO 2.5

// =====================================================
// VARIABLES
// =====================================================

float temperature = 0;
float voltage = 0;
float current = 0;
int gasValue = 0;

// =====================================================
// READ VOLTAGE
// =====================================================

float readVoltage()
{
  int adcValue = analogRead(VOLTAGE_PIN);

  float adcVoltage =
      (adcValue / 4095.0) * 3.3;

  float batteryVoltage =
      adcVoltage * VOLTAGE_DIVIDER_RATIO;

  return batteryVoltage;
}

// =====================================================
// READ CURRENT
// =====================================================

float readCurrent()
{
  int adcValue = analogRead(CURRENT_PIN);

  float sensorVoltage =
      (adcValue / 4095.0) * 3.3;

  float current =
      (sensorVoltage - ACS712_ZERO)
      / ACS712_SENSITIVITY;

  if (abs(current) < 0.10)
  {
    current = 0;
  }

  return abs(current);
}

// =====================================================
// READ GAS SENSOR
// =====================================================

int readGas()
{
  int gasADC = analogRead(GAS_PIN);

  return gasADC;
}

// =====================================================
// SETUP
// =====================================================

void setup()
{
  Serial.begin(115200);

  // DS18B20
  sensors.begin();

  // Gas sensor
  pinMode(GAS_PIN, INPUT);

  // Outputs
  pinMode(GREEN_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(GREEN_LED, LOW);
  digitalWrite(RED_LED, LOW);
  digitalWrite(BUZZER, LOW);

  // KiwisIoT
  kiwisiot.begin();

  Serial.println("================================");
  Serial.println("Battery Monitoring System");
  Serial.println("Gas Sensor Added");
  Serial.println("KiwisIoT Starting...");
  Serial.println("================================");
}

// =====================================================
// LOOP
// =====================================================

void loop()
{
  // Maintain KiwisIoT connection
  kiwisiot.run();

  // ===================================================
  // READ TEMPERATURE
  // ===================================================

  sensors.requestTemperatures();

  temperature = sensors.getTempCByIndex(0);

  // ===================================================
  // READ VOLTAGE
  // ===================================================

  voltage = readVoltage();

  // ===================================================
  // READ CURRENT
  // ===================================================

  current = readCurrent();

  // ===================================================
  // READ GAS
  // ===================================================

  gasValue = readGas();

  // ===================================================
  // CHECK ABNORMAL CONDITION
  // ===================================================

  bool abnormal = false;

  // Temperature check
  if (temperature > MAX_TEMPERATURE ||
      temperature < MIN_TEMPERATURE)
  {
    abnormal = true;
  }

  // Voltage check
  if (voltage > MAX_VOLTAGE ||
      voltage < MIN_VOLTAGE)
  {
    abnormal = true;
  }

  // Current check
  if (current > MAX_CURRENT)
  {
    abnormal = true;
  }

  // Gas check
  if (gasValue > MAX_GAS_VALUE)
  {
    abnormal = true;
  }

  // ===================================================
  // BATTERY STATUS
  // ===================================================

  const char* status;

  if (abnormal)
  {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    digitalWrite(BUZZER, HIGH);

    status = "Abnormal";

    Serial.println("BATTERY STATUS : ABNORMAL");
  }
  else
  {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BUZZER, LOW);

    status = "Normal";

    Serial.println("BATTERY STATUS : NORMAL");
  }

  // ===================================================
  // SEND DATA TO KIWISIOT
  // ===================================================

  kiwisiot.send("1", temperature);
  kiwisiot.send("2", voltage);
  kiwisiot.send("3", current);
  kiwisiot.send("4", status);
  kiwisiot.send("5", gasValue);

  // ===================================================
  // SERIAL MONITOR
  // ===================================================

  Serial.print("Temperature : ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Voltage     : ");
  Serial.print(voltage);
  Serial.println(" V");

  Serial.print("Current     : ");
  Serial.print(current);
  Serial.println(" A");

  Serial.print("Gas Value   : ");
  Serial.println(gasValue);

  Serial.print("Status      : ");
  Serial.println(status);

  Serial.println("--------------------------------");

  delay(1000);
}
