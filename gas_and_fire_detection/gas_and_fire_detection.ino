#include <ESP8266WiFi.h>
#include <KiwisIoT.h>
#include <DHT.h>

// ============== WIFI SETTINGS ==============
 char* ssid  = "YOUR_WIFI_SSID";
 char* pass  = "YOUR_WIFI_PASSWORD";
 char* topic = "YOUR_DASHBOARD_TOPIC";

KiwisIoT kiwisiot(ssid, pass, topic);

// ============== PIN CONFIGURATION ==========
#define DHT_PIN     D4
#define DHTTYPE     DHT11

#define MQ2_PIN     A0
#define FLAME_PIN   D5
#define RELAY_PIN   D1
#define BUZZER_PIN  D6

DHT dht(DHT_PIN, DHTTYPE);

// ============== THRESHOLDS =================
#define GAS_THRESHOLD   400
#define TEMP_THRESHOLD  50.0

// Typical flame sensor: LOW means flame detected
#define FLAME_ACTIVE LOW

// Typical relay module: LOW means relay ON
#define RELAY_ON  LOW
#define RELAY_OFF HIGH

// Typical active-HIGH buzzer
#define BUZZER_ON  HIGH
#define BUZZER_OFF LOW

// ============== TIMING =====================
unsigned long lastSend = 0;
const unsigned long interval = 2000;

// ============== SENSOR DATA =================
int gasValue = 0;
int flameValue = HIGH;

float temperature = 0.0;
float humidity = 0.0;

bool alertState = false;

// ============================================

void setup()
{
  Serial.begin(115200);

  pinMode(FLAME_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Keep relay and buzzer OFF initially
  digitalWrite(RELAY_PIN, RELAY_OFF);
  digitalWrite(BUZZER_PIN, BUZZER_OFF);

  dht.begin();

  // Initialize KiwisIoT
  kiwisiot.begin();

  Serial.println();
  Serial.println("GAS AND FIRE DETECTION SYSTEM");
  Serial.println("NodeMCU + KiwisIoT");
  Serial.println("System Started");
}

// ============================================

void loop()
{
  // Maintain KiwisIoT communication
  kiwisiot.run();

  unsigned long now = millis();

  if (now - lastSend >= interval)
  {
    lastSend = now;

    // Read MQ-2 gas sensor
    gasValue = analogRead(MQ2_PIN);

    // Read flame sensor
    flameValue = digitalRead(FLAME_PIN);

    // Read DHT11
    float temp = dht.readTemperature();
    float hum  = dht.readHumidity();

    bool temperatureValid = !isnan(temp);
    bool humidityValid = !isnan(hum);

    // Update valid readings only
    if (temperatureValid)
    {
      temperature = temp;
    }

    if (humidityValid)
    {
      humidity = hum;
    }

    // Detect danger conditions
    bool gasDetected = (gasValue >= GAS_THRESHOLD);
    bool flameDetected = (flameValue == FLAME_ACTIVE);

    bool highTemperature =
      temperatureValid && (temp >= TEMP_THRESHOLD);

    // Single combined alert state
    alertState =
      gasDetected ||
      flameDetected ||
      highTemperature;

    // Relay and buzzer control
    if (alertState)
    {
      digitalWrite(RELAY_PIN, RELAY_ON);
      digitalWrite(BUZZER_PIN, BUZZER_ON);
    }
    else
    {
      digitalWrite(RELAY_PIN, RELAY_OFF);
      digitalWrite(BUZZER_PIN, BUZZER_OFF);
    }

    // ============== SERIAL MONITOR ==============

    Serial.println();
    Serial.println("========== SENSOR DATA ==========");

    Serial.print("Temperature: ");
    if (temperatureValid)
    {
      Serial.print(temperature);
      Serial.println(" C");
    }
    else
    {
      Serial.println("DHT11 Error");
    }

    Serial.print("Humidity: ");
    if (humidityValid)
    {
      Serial.print(humidity);
      Serial.println(" %");
    }
    else
    {
      Serial.println("DHT11 Error");
    }

    Serial.print("Gas Value: ");
    Serial.println(gasValue);

    Serial.print("Alert Status: ");
    Serial.println(alertState ? "ALERT" : "NORMAL");

    Serial.print("Relay: ");
    Serial.println(alertState ? "ON" : "OFF");

    Serial.print("Buzzer: ");
    Serial.println(alertState ? "ON" : "OFF");

    // ============== KIWISIOT ====================

    // Sensor values
    kiwisiot.send("0", temperature);
    kiwisiot.send("1", humidity);
    kiwisiot.send("2", gasValue);

    // Combined alert only
    // 0 = NORMAL
    // 1 = ALERT
    kiwisiot.send("3", alertState ? 1 : 0);


    Serial.println("Data sent to KiwisIoT");
    Serial.println("=================================");
  }
}
