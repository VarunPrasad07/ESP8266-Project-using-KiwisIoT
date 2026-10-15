#include <ESP8266WiFi.h>
#include <KiwisIoT.h>
#include <DHT.h>

// WIFI
 char* ssid  = "YOUR_WIFI_SSID";
 char* pass  = "YOUR_WIFI_PASSWORD";
 char* topic = "YOUR_DASHBOARD_TOPIC";

KiwisIoT kiwisiot(ssid, pass, topic);

// PINS
#define SOIL_PIN   A0
#define LDR_PIN    D5
#define RELAY_PIN  D1   // Pump Relay
#define LIGHT_PIN  D6   // Light / LED
#define DHT_PIN    D2
#define DHTTYPE    DHT11

DHT dht(DHT_PIN, DHTTYPE);

// sensor sending timer
unsigned long lastSend = 0;
const long interval = 3000;

// -------- CLOUD CONTROL --------
void onControl(String ch, String val)
{
  Serial.print("Channel: ");
  Serial.print(ch);
  Serial.print(" Value: ");
  Serial.println(val);

  int channel = ch.toInt();

  // -------- Pump Relay (Channel 3) --------
  if(channel == 3)
  {
    if(val == "1")
    {
      digitalWrite(RELAY_PIN, LOW);   // relay ON
      Serial.println("Pump ON");
    }
    else
    {
      digitalWrite(RELAY_PIN, HIGH);  // relay OFF
      Serial.println("Pump OFF");
    }
  }

  // -------- Light Control (Channel 5) --------
  if(channel == 5)
  {
    if(val == "1")
    {
      digitalWrite(LIGHT_PIN, HIGH);  // light ON
      Serial.println("Light ON");
    }
    else
    {
      digitalWrite(LIGHT_PIN, LOW);   // light OFF
      Serial.println("Light OFF");
    }
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(SOIL_PIN, INPUT);
  pinMode(LDR_PIN, INPUT);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LIGHT_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, HIGH); // pump OFF
  digitalWrite(LIGHT_PIN, LOW);  // light OFF

  dht.begin();

  kiwisiot.begin();
  kiwisiot.onReceive(onControl);

  Serial.println("System Started");
}

void loop()
{
  kiwisiot.run();

  unsigned long now = millis();

  if(now - lastSend > interval)
  {
    lastSend = now;

    int soil = analogRead(SOIL_PIN);
    int ldr  = digitalRead(LDR_PIN);

    float temp = dht.readTemperature();
    float hum  = dht.readHumidity();

    kiwisiot.send("0", soil);
    kiwisiot.send("6", ldr);

    if(!isnan(temp) && !isnan(hum))
    {
      kiwisiot.send("1", temp);
      kiwisiot.send("2", hum);
    }
  }
}
