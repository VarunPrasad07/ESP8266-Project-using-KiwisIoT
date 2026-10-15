  #include <ESP8266WiFi.h>
#include <KiwisIoT.h>

// WIFI
 char* ssid  = "YOUR_WIFI_SSID";
 char* pass  = "YOUR_WIFI_PASSWORD";
 char* topic = "YOUR_DASHBOARD_TOPIC";

KiwisIoT kiwisiot(ssid, pass, topic);

// PINS
#define IR_PIN   D5
#define RELAY D1
#define LED_PIN  D6
#define BUZZER   D7

unsigned long lastSend = 0;
const long interval = 2000;


// -------- MOTOR CONTROL ------


void setup()
{
  Serial.begin(115200);

  pinMode(IR_PIN, INPUT);

pinMode(RELAY,OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER, OUTPUT);


  kiwisiot.begin();

  Serial.println("Driver Drowsiness System Started");
}


void loop()
{
  kiwisiot.run();

  unsigned long now = millis();

  if(now - lastSend > interval)
  {
    lastSend = now;

    int drowsy = digitalRead(IR_PIN);

    kiwisiot.send("0", drowsy);

    if(drowsy == LOW)
    {
    digitalWrite(RELAY,LOW);
      digitalWrite(LED_PIN, LOW);
      digitalWrite(BUZZER, LOW);

      Serial.println("Driver Sleeping - Motor Stopped");
    }
    else
    {
     digitalWrite(RELAY,HIGH);
      digitalWrite(LED_PIN, HIGH);
      digitalWrite(BUZZER, HIGH);
    }
  }
}
