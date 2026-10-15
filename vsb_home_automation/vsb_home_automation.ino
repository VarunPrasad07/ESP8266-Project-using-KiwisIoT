#include <ESP8266WiFi.h>
#include <KiwisIoT.h>

// -------- WIFI --------
 char* ssid  = "YOUR_WIFI_SSID";
 char* pass  = "YOUR_WIFI_PASSWORD";
 char* topic = "YOUR_DASHBOARD_TOPIC";

KiwisIoT kiwisiot(ssid, pass, topic);

// -------- PINS --------
#define LIGHT_RELAY D2
#define FAN_RELAY   D4

// -------- FACE --------
unsigned long lastRecognisedTime = 0;
const unsigned long FACE_TIMEOUT = 7000;

// -------- COMMAND --------
String lastCommand = "";

// -------- CONTROL --------
void onControl(String ch, String val) {

  val.trim();
  val.toLowerCase();

  Serial.print("CH=");
  Serial.print(ch);
  Serial.print(" VAL=");
  Serial.println(val);

  // -------- FACE CHANNEL --------
if (ch == "1") {

  if (val.indexOf("recognised") >= 0) {
    lastRecognisedTime = millis();
    Serial.println("Face Recognised");
  } 
  else if (val.indexOf("unrecognised") >= 0) {
    Serial.println("Face Not Recognised");
  } 
  else {
    Serial.println("Unknown Face Status");
  }

  return;
}
  // -------- COMMAND CHANNEL --------
  if (ch == "0") {

    bool faceValid = (millis() - lastRecognisedTime < FACE_TIMEOUT);

    if (!faceValid) {
      Serial.println("Blocked: Face not valid");
      return;
    }

    // -------- EXECUTE --------
    if (val == "fan on") {
      digitalWrite(FAN_RELAY, LOW);
      Serial.println("Fan ON");
    } 
    else if (val == "fan off"|| "fan of") {
      digitalWrite(FAN_RELAY, HIGH);
      Serial.println("Fan OFF");
    }

    if (val == "light on") {
      digitalWrite(LIGHT_RELAY, LOW);
      Serial.println("Light ON");
    } 
    else if (val == "light off "|| "light of") {
      digitalWrite(LIGHT_RELAY, HIGH);
      Serial.println("Light OFF");
    }
  }
}

// -------- SETUP --------
void setup() {
  Serial.begin(115200);

  pinMode(LIGHT_RELAY, OUTPUT);
  pinMode(FAN_RELAY, OUTPUT);

  digitalWrite(LIGHT_RELAY, HIGH);
  digitalWrite(FAN_RELAY, HIGH);

  kiwisiot.begin();
  kiwisiot.onReceive(onControl);

  Serial.println("System Ready");
}

// -------- LOOP --------
void loop() {
  kiwisiot.run();
}
