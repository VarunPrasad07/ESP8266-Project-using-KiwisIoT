#include <KiwisIoT.h>

 char* ssid  = "YOUR_WIFI_SSID";
 char* pass  = "YOUR_WIFI_PASSWORD";
 char* topic = "YOUR_DASHBOARD_TOPIC";

#define IN3 D5
#define IN4 D6
#define IN1 D3
#define IN2 D4

#define LIGHT_PIN D1
#define HORN_PIN  D2

int powerState = 0;   // 0 = OFF, 1 = ON

KiwisIoT kiwisiot(ssid, pass, topic);

// -------- MOTOR --------

void motorForward() {
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void motorReverse() {
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void motorRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
}

void motorLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
}

void motorStop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// -------- CONTROL --------

void onControl(String ch, String val) {

  if (ch != "0") return;

  Serial.println("Value: " + val);

  // -------- POWER --------
  if (val == "power:1") {
    powerState = 1;
    Serial.println("Power ON");
    return;
  }

  if (val == "power:0") {
    powerState = 0;
    motorStop();
    Serial.println("Power OFF - Motor Stopped");
    return;
  }

  // -------- LIGHT --------
  if (val == "lights:1") {
    digitalWrite(LIGHT_PIN, HIGH);
    Serial.println("Lights ON");
    return;
  }

  if (val == "lights:0") {
    digitalWrite(LIGHT_PIN, LOW);
    Serial.println("Lights OFF");
    return;
  }

  // -------- HORN --------
  if (val == "horn") {
    digitalWrite(HORN_PIN, HIGH);
    delay(300);
    digitalWrite(HORN_PIN, LOW);
    return;
  }

  // -------- MOVEMENT (ONLY IF POWER ON) --------
  if (powerState == 0) {
    motorStop();
    return;
  }

  if (val == "forward") motorForward();
  else if (val == "back") motorReverse();
  else if (val == "right") motorRight();
  else if (val == "left") motorLeft();
  else if (val == "stop") motorStop();
}

void setup() {

  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(LIGHT_PIN, OUTPUT);
  pinMode(HORN_PIN, OUTPUT);

  motorStop();

  kiwisiot.begin();
  kiwisiot.onReceive(onControl);
}

void loop() {
  kiwisiot.run();
}
