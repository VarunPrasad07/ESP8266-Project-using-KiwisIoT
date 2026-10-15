#include <ESP8266WiFi.h>
#include <KiwisIoT.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ================= WIFI =================
 char* ssid  = "YOUR_WIFI_SSID";
 char* pass  = "YOUR_WIFI_PASSWORD";
 char* topic = "YOUR_DASHBOARD_TOPIC";

KiwisIoT kiwisiot(ssid, pass, topic);

// ================= LCD =================
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ================= DHT =================
#define DHTPIN D4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// ================= SENSORS =================
#define GAS_PIN A0
#define RAIN_PIN D5

// ================= OUTPUTS =================
#define LED_PIN D6
#define BUZZER_PIN D7

unsigned long lastSend = 0;
const long interval = 3000;

void setup()
{
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(GAS_PIN, INPUT);
  pinMode(RAIN_PIN, INPUT);

  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  lcd.init();
  lcd.backlight();

  dht.begin();

  kiwisiot.begin();

  lcd.setCursor(0, 0);
  lcd.print("Weather Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");
  delay(2000);
  lcd.clear();
}

void loop()
{
  kiwisiot.run();

  unsigned long now = millis();

  if (now - lastSend >= interval)
  {
    lastSend = now;

    float temp = dht.readTemperature();
    float hum  = dht.readHumidity();

    int gasValue = analogRead(GAS_PIN);
    int rainStatus = digitalRead(RAIN_PIN);

    // ================= SEND DATA =================
    if (!isnan(temp) && !isnan(hum))
    {
      kiwisiot.send("0", temp);
      kiwisiot.send("1", hum);
    }

    kiwisiot.send("2", gasValue);
    kiwisiot.send("3", rainStatus);

    // ================= LCD DISPLAY =================
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("T:");
    lcd.print(temp, 1);
    lcd.print(" H:");
    lcd.print((int)hum);

    lcd.setCursor(0, 1);

    if (rainStatus == LOW)
    {
      lcd.print("RAIN ");
    }
    else
    {
      lcd.print("NORAIN ");
    }

    lcd.print("G:");
    lcd.print(gasValue);

    // ================= ALERT LOGIC =================
    bool alert = false;

    if (temp > 35)
    {
      alert = true;
    }

    if (rainStatus == LOW)   // Rain detected
    {
      alert = true;
    }

    if (alert)
    {
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(BUZZER_PIN, HIGH);

      kiwisiot.send("4", 1);
      kiwisiot.send("5", 1);
    }
    else
    {
      digitalWrite(LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);

      kiwisiot.send("4", 0);
      kiwisiot.send("5", 0);
    }

    // ================= SERIAL MONITOR =================
    Serial.println("====================================");

    Serial.print("Temperature : ");
    Serial.print(temp);
    Serial.println(" C");

    Serial.print("Humidity    : ");
    Serial.print(hum);
    Serial.println(" %");

    Serial.print("Gas Value   : ");
    Serial.println(gasValue);

    Serial.print("Rain Status : ");
    if (rainStatus == LOW)
      Serial.println("RAIN DETECTED");
    else
      Serial.println("NO RAIN");

    Serial.print("Alert       : ");
    Serial.println(alert ? "ON" : "OFF");
  }
}
