#include <ESP8266WiFi.h>
#include <KiwisIoT.h>

// WIFI
 char* ssid  = "YOUR_WIFI_SSID";
 char* pass  = "YOUR_WIFI_PASSWORD";

// KiwisIoT topic
 char* topic = "YOUR_DASHBOARD_TOPIC";

// Relay pin
#define RELAY_PIN D6

KiwisIoT kiwisiot(ssid, pass, topic);

// Receive data from dashboard
void onControl(String ch, String val)
{
  Serial.print("Channel: ");
  Serial.print(ch);
  Serial.print("  Value: ");
  Serial.println(val);

  if(ch == "0")   // dashboard channel
  {
    if(val == "1")
    {
      digitalWrite(RELAY_PIN, LOW);   // Relay ON
      Serial.println("Relay ON");
    }
    else if(val == "0")
    {
      digitalWrite(RELAY_PIN, HIGH);  // Relay OFF
      Serial.println("Relay OFF");
    }
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);   // Relay OFF at start

  kiwisiot.begin();
  kiwisiot.onReceive(onControl);

  Serial.println("Relay IoT Control Started");
}

void loop()
{
  kiwisiot.run();
}
