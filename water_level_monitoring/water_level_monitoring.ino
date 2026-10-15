#include <ESP8266WiFi.h>
#include <KiwisIoT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// WIFI
 char* ssid  = "YOUR_WIFI_SSID";
 char* pass  = "YOUR_WIFI_PASSWORD";
 char* topic = "YOUR_DASHBOARD_TOPIC";

KiwisIoT kiwisiot(ssid, pass, topic);

// LCD
LiquidCrystal_I2C lcd(0x27,16,2);

// Ultrasonic
#define TRIG D5
#define ECHO D6

// Water sensor
#define WATER_PIN A0

// Alerts
#define LED_PIN D7
#define BUZZER_PIN D8

unsigned long lastSend = 0;
const long interval = 3000;

long duration;
float distance;

void setup()
{
  Serial.begin(115200);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  lcd.init();
  lcd.backlight();

  kiwisiot.begin();

  lcd.setCursor(0,0);
  lcd.print("Water Monitor");
  delay(2000);
  lcd.clear();

  Serial.println("Water Level Monitor Started");
}

void loop()
{
  kiwisiot.run();

  unsigned long now = millis();

  if(now - lastSend > interval)
  {
    lastSend = now;

    // Ultrasonic distance
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);

    duration = pulseIn(ECHO, HIGH);
    distance = duration * 0.034 / 2;

    // Water sensor reading
    int waterValue = analogRead(WATER_PIN);

    // Send data to IoT
    kiwisiot.send("0", distance);
    kiwisiot.send("1", waterValue);

    // LCD display
    lcd.setCursor(0,0);
    lcd.print("Dist:");
    lcd.print(distance);
    lcd.print("cm   ");

    lcd.setCursor(0,1);
    lcd.print("Water:");
    lcd.print(waterValue);
    lcd.print("   ");

    // Alert logic
    if(distance < 10) // tank full
    {
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(BUZZER_PIN, HIGH);

      lcd.setCursor(10,0);
      lcd.print("FULL");

      kiwisiot.send("4",1);
      kiwisiot.send("5",1);

      Serial.println("Tank Full");
    }
    else
    {
      digitalWrite(LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);

      lcd.setCursor(10,0);
      lcd.print("    ");

      kiwisiot.send("4",0);
      kiwisiot.send("5",0);
    }

    Serial.print("Distance: ");
    Serial.println(distance);

    Serial.print("Water Sensor: ");
    Serial.println(waterValue);
  }
}
