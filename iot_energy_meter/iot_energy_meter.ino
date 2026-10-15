#include <KiwisIoT.h>

// -------- WIFI --------
 char* ssid  = "YOUR_WIFI_SSID";
 char* pass  = "YOUR_WIFI_PASSWORD";
 char* topic = "YOUR_DASHBOARD_TOPIC";

KiwisIoT kiwisiot(ssid, pass, topic);

// -------- PINS --------
#define CURRENT_PIN 34
#define VOLTAGE_PIN 35
#define GAS_PIN     32
#define RELAY_PIN   25
#define BUZZER_PIN  26

// -------- RELAY LOGIC --------
#define RELAY_ON  LOW
#define RELAY_OFF HIGH

// -------- PARAMETERS --------
float sensitivity = 0.185;   // ACS712 5A
float offsetVoltage = 0;

float voltageCalibration = 277.0;
float tariff = 6.0;

// -------- ENERGY --------
float energy = 0;
unsigned long lastTime = 0;

// -------- OFFSET CALIBRATION --------
float calibrateOffset() {
  long sum = 0;

  Serial.println("Calibrating... NO LOAD");

  for (int i = 0; i < 2000; i++) {
    sum += analogRead(CURRENT_PIN);
    delay(1);
  }

  float avg = sum / 2000.0;
  float offset = (avg / 4095.0) * 3.3;

  Serial.print("Offset Voltage: ");
  Serial.println(offset, 4);

  return offset;
}

// -------- CURRENT RMS --------
float readCurrentRMS() {
  float sumSq = 0;
  int samples = 500;

  for (int i = 0; i < samples; i++) {
    float raw = analogRead(CURRENT_PIN);
    float voltage = (raw / 4095.0) * 3.3;

    float current = (voltage - offsetVoltage) / sensitivity;

    sumSq += current * current;
    delayMicroseconds(200);
  }

  float rms = sqrt(sumSq / samples);

  // noise removal
  if (rms < 0.08) rms = 0;

  return rms;
}

// -------- VOLTAGE RMS --------
float readVoltageRMS() {
  float sumSq = 0;
  int samples = 500;

  for (int i = 0; i < samples; i++) {
    float raw = analogRead(VOLTAGE_PIN);
    float voltage = (raw / 4095.0) * 3.3;

    voltage -= 1.65;

    sumSq += voltage * voltage;
    delayMicroseconds(200);
  }

  float rms = sqrt(sumSq / samples);

  return rms * voltageCalibration;
}

// -------- SETUP --------
void setup() {
  Serial.begin(115200);

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, RELAY_OFF);
  digitalWrite(BUZZER_PIN, LOW);

  kiwisiot.begin();

  delay(2000);

  offsetVoltage = calibrateOffset();

  lastTime = millis();
}

// -------- LOOP --------
void loop() {

  kiwisiot.run();

  float current = readCurrentRMS();
  float voltage = readVoltageRMS();
  float power = voltage * current;

  // -------- ENERGY (FIXED) --------
  unsigned long now = millis();
  float seconds = (now - lastTime) / 1000.0;

  energy += (power * seconds) / 3600000.0;  // kWh

  lastTime = now;

  float cost = energy * tariff;

  float runtimeHours = millis() / 3600000.0;
  float predictedDaily = (runtimeHours > 0) ? energy * (24.0 / runtimeHours) : 0;
  float predictedMonthly = predictedDaily * 30;

  int gasValue = analogRead(GAS_PIN);

  // -------- SAFETY --------
  if (current > 5.0 || gasValue > 2000) {
    digitalWrite(RELAY_PIN, RELAY_OFF);
    digitalWrite(BUZZER_PIN, HIGH);
    kiwisiot.send("alert", "1");
  } else {
    digitalWrite(RELAY_PIN, RELAY_ON);
    digitalWrite(BUZZER_PIN, LOW);
    kiwisiot.send("alert", "0");
  }

  // -------- SEND (WITH PRECISION) --------
  kiwisiot.send("1", voltage);
  kiwisiot.send("2", current);
  kiwisiot.send("3", power);
  kiwisiot.send("4", String(energy, 6));   // 🔥 FIXED
  kiwisiot.send("5", cost);
  kiwisiot.send("6", gasValue);
  kiwisiot.send("7", predictedDaily);
  kiwisiot.send("8", predictedMonthly);

  // -------- DEBUG --------
  Serial.print("V: "); Serial.print(voltage);
  Serial.print(" | I: "); Serial.print(current);
  Serial.print(" | P: "); Serial.print(power);
  Serial.print(" | E: "); Serial.print(energy, 6);
  Serial.print(" | Gas: "); Serial.print(gasValue);
  Serial.print(" | Relay: ");
  Serial.println((digitalRead(RELAY_PIN) == RELAY_ON) ? "ON" : "OFF");

  delay(1500);
}
