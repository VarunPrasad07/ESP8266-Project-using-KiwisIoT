A#include <ESP8266WiFi.h>
#include <KiwisIoT.h>
#include <Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// WIFI
 char* ssid  = "YOUR_WIFI_SSID";
 char* pass  = "YOUR_WIFI_PASSWORD";
 char* topic = "YOUR_DASHBOARD_TOPIC";

KiwisIoT kiwisiot(ssid, pass, topic);

LiquidCrystal_I2C lcd(0x27,16,2);
Servo lid;

// Ultrasonic
#define TRIG D5
#define ECHO D6

// Output
#define LED_PIN D4
#define SERVO_PIN D7

long duration;
float distance;

unsigned long lastSend = 0;
const long interval = 2000;

void setup()
{
  Serial.begin(115200);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(LED_PIN, OUTPUT);

  lid.attach(SERVO_PIN);

  lcd.init();
  lcd.backlight();

  kiwisiot.begin();

  lid.write(0);

  lcd.setCursor(0,0);
  lcd.print("Smart Dustbin");
  delay(2000);
  lcd.clear();

  Serial.println("Smart Dustbin Started");
}

void loop()
{
  kiwisiot.run();

  unsigned long now = millis();

  if(now - lastSend > interval)
  {
    lastSend = now;

    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);

    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);

    duration = pulseIn(ECHO, HIGH);
    distance = duration * 0.034 / 2;

    Serial.print("Distance: ");
    Serial.println(distance);

    // LCD line 1 → distance
    lcd.setCursor(0,0);
    lcd.print("Dist:");
    lcd.print(distance);
    lcd.print(" cm   ");

    if(distance < 15)
    {
      lid.write(90);
      digitalWrite(LED_PIN, HIGH);

      lcd.setCursor(0,1);
      lcd.print("Dustbin OPEN   ");

      kiwisiot.send("0",1);
      kiwisiot.send("4",1);

      Serial.println("Dustbin Open");
    }
    else
    {
      lid.write(0);
      digitalWrite(LED_PIN, LOW);

      lcd.setCursor(0,1);
      lcd.print("Dustbin CLOSED ");

      kiwisiot.send("0",0);
      kiwisiot.send("4",0);

      Serial.println("Dustbin Closed");
    }
  }
}
