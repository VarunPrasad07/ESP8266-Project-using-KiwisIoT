#include <KiwisIoT.h>

// ===== WIFI / IOT =====
 char* ssid  ="YOUR_WIFI_SSID";
 char* pass  = "YOUR_WIFI_PASSWORD";
 char* topic = "YOUR_DASHBOARD_TOPIC";

KiwisIoT kiwisiot(ssid, pass, topic);

// ===== SENSOR DATA =====
String input = "";
bool presence = false;
int distance = 999;

// ===== CONFIG =====
#define THRESHOLD 30
#define LDR_PIN A0
#define RELAY_PIN D1
#define DARK_THRESHOLD 600

// ===== HOLD LOGIC =====
unsigned long lastDetectTime = 0;
int holdTime = 3000;
int manualMode

// ===== CLOUD CONTROL =====
bool cloudEnable = false;  // ch
bool cloudValue  = false;  // val

// ===== CALLBACK =====
void onControl(String ch, String val) {

  if (ch != "0") return;

  Serial.println(val);

  // ===== MANUAL MODE ON =====
  if (val == "1") {
    manualMode = true;
    manualRelay = true;
    Serial.println("Manual Relay ON");
  }

  else if (val == "0") {
    manualMode = true;
    manualRelay = false;
    Serial.println("Manual Relay OFF");
  }

  // ===== BACK TO AUTO =====
  else if (val == "auto") {
    manualMode = false;
    Serial.println("Back to AUTO mode");
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // OFF (active LOW)

  kiwisiot.init();
  kiwisiot.setCallback(callback);
}

void loop() {
  kiwisiot.run();

  // ===== READ mmWave (ASCII) =====
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '\n') {
      processData(input);
      input = "";
    } else {
      input += c;
    }
  }

  // ===== LDR =====
  int ldrValue = analogRead(LDR_PIN);
  bool isDark = (ldrValue > DARK_THRESHOLD);

  // ===== HOLD FILTER =====
  if (presence && distance < THRESHOLD) {
    lastDetectTime = millis();
  }

  bool personDetected = (millis() - lastDetectTime < holdTime);

  // ===== FINAL LOGIC =====
  bool autoCondition  = personDetected && isDark;
  bool cloudCondition = cloudEnable && cloudValue;

  bool finalRelay = autoCondition || cloudCondition;

  // ===== RELAY CONTROL =====
  digitalWrite(RELAY_PIN, finalRelay ? LOW : HIGH);

  // ===== DEBUG =====
  Serial.print("Presence: ");
  Serial.print(personDetected);
  Serial.print(" | Dist: ");
  Serial.print(distance);
  Serial.print(" | LDR: ");
  Serial.print(ldrValue);
  Serial.print(" | Relay: ");
  Serial.println(finalRelay);

  // ===== SEND TO CLOUD =====
  kiwisiot.publish("presence", personDetected);
  kiwisiot.publish("distance", distance);
  kiwisiot.publish("ldr", ldrValue);
  kiwisiot.publish("relay", finalRelay);

  delay(500);
}

// ===== PARSE SENSOR =====
void processData(String data) {
  data.trim();

  if (data == "ON") {
    presence = true;
  } 
  else if (data == "OFF") {
    presence = false;
  }
  else if (data.startsWith("Range")) {
    distance = data.substring(6).toInt();
  }
}
