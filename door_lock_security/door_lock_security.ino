#include <ESP8266WiFi.h>
#include <KiwisIoT.h>

// WIFI
 char* ssid  = "YOUR_WIFI_SSID";
 char* pass  = "YOUR_WIFI_PASSWORD";
 char* topic = "YOUR_DASHBOARD_TOPIC";

KiwisIoT kiwisiot(ssid, pass, topic);

// PINS
#define PIR_PIN    D1
#define RELAY_PIN  D5
#define LED_PIN    D3
#define BUZZER_PIN D4

unsigned long lastSend = 0;
const long interval = 2000;


// ---------- CLOUD CONTROL ----------
void onControl(String ch, String val)
{
  Serial.print("Channel: ");
  Serial.print(ch);
  Serial.print(" Value: ");
  Serial.println(val);

  int channel = ch.toInt();

  // Door lock control from cloud
  if(channel == 3)
  {
    if(val == "1")
    {
      digitalWrite(RELAY_PIN, LOW);   // lock
      Serial.println("Door Locked");
    }
    else
    {
      digitalWrite(RELAY_PIN, HIGH);  // unlock
      Serial.println("Door Unlocked");
    }
  }

  // LED control
  if(channel == 4)
  {
    digitalWrite(LED_PIN, val == "1" ? HIGH : LOW);
  }

  // Buzzer control
  if(channel == 5)
  {
    digitalWrite(BUZZER_PIN, val == "1" ? HIGH : LOW);
  }
}


void setup()
{
  Serial.begin(115200);

  pinMode(PIR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, HIGH); // door unlocked
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  kiwisiot.begin();
  kiwisiot.onReceive(onControl);

  Serial.println("Door Security System Started");
}


void loop()
{
  kiwisiot.run();

  unsigned long now = millis();

  if(now - lastSend > interval)
  {
    lastSend = now;

    int motion = digitalRead(PIR_PIN);

    // Send PIR status to cloud
    kiwisiot.send("0", motion);

if(motion == HIGH)
{
  digitalWrite(RELAY_PIN, LOW);   // lock door
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(BUZZER_PIN, HIGH);

  kiwisiot.send("4", 1);
  kiwisiot.send("5", 1);

  Serial.println("Motion Detected - Door Locked");
}
else
{
  digitalWrite(RELAY_PIN, HIGH);  // unlock door
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  kiwisiot.send("4", 0);
  kiwisiot.send("5", 0);

  Serial.println("No Motion - Door Unlocked");
}
  }
}
