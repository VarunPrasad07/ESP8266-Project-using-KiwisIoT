#include <ESP8266WiFi.h>
#include <KiwisIoT.h>
#include <Servo.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// -------- WIFI --------
 char* ssid  = "YOUR_WIFI_SSID";
 char* pass  = "YOUR_WIFI_PASSWORD";
 char* topic = "YOUR_DASHBOARD_TOPIC";

KiwisIoT kiwisiot(ssid, pass, topic);

// -------- PINS --------
#define IR_PIN     D5
#define METAL_PIN  D6
#define TRIG       D7
#define ECHO       D8
#define SERVO_PIN  D3
#define BUZZER     D0
#define DHTPIN     D4
#define GAS_PIN    A0

#define DHTTYPE DHT11

// -------- SERVO --------
Servo myServo;
#define LEFT_US   1000
#define CENTER_US 1450
#define RIGHT_US  2000
#define STEP_DELAY 8

// -------- OBJECT CONTROL --------
bool detected = false;
unsigned long lastActionTime = 0;
#define COOLDOWN 2000

// -------- SENSORS --------
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

float distance = 0;
int binPercent = 0;
int gasValue = 0;
float temperature = 0;

// -------- SMOOTH MOVE --------
void smoothMove(int startPos, int endPos) {
  if (startPos < endPos) {
    for (int pos = startPos; pos <= endPos; pos += 5) {
      myServo.writeMicroseconds(pos);
      delay(STEP_DELAY);
    }
  } else {
    for (int pos = startPos; pos >= endPos; pos -= 5) {
      myServo.writeMicroseconds(pos);
      delay(STEP_DELAY);
    }
  }
}

// -------- BUZZER --------
void beepTwice() {
  for (int i = 0; i < 2; i++) {
    digitalWrite(BUZZER, HIGH);
    delay(150);
    digitalWrite(BUZZER, LOW);
    delay(150);
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(IR_PIN, INPUT);
  pinMode(METAL_PIN, INPUT_PULLUP);   // stable input
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(BUZZER, OUTPUT);

  myServo.attach(SERVO_PIN);
  myServo.writeMicroseconds(CENTER_US);

  dht.begin();

  Wire.begin(D2, D1);
  lcd.init();
  lcd.backlight();
  lcd.print("System Ready");

  kiwisiot.begin();
}

void loop() {

  kiwisiot.run();

  // -------- IR DETECTION --------
  int ir = digitalRead(IR_PIN);

  if (ir == LOW && !detected && millis() - lastActionTime > COOLDOWN) {

    detected = true;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Object Found");

    int metal = digitalRead(METAL_PIN);

    Serial.print("Metal: ");
    Serial.println(metal);

    // ✅ INVERTED SERVO LOGIC
    if (metal == HIGH) {
      lcd.setCursor(0, 1);
      lcd.print("Metal");

      beepTwice();

      // 👉 NOW goes RIGHT
      smoothMove(CENTER_US, RIGHT_US);
      delay(300);
      smoothMove(RIGHT_US, CENTER_US);

      kiwisiot.send("metal", "1");

    } else {
      lcd.setCursor(0, 1);
      lcd.print("Non-Metal");

      // 👉 NOW goes LEFT
      smoothMove(CENTER_US, LEFT_US);
      delay(300);
      smoothMove(LEFT_US, CENTER_US);

      kiwisiot.send("0", "Metal");
    }

    lastActionTime = millis();
  }

  // Reset detection
  if (ir == HIGH) {
    detected = false;
  }

  // -------- ULTRASONIC --------
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 30000);
  distance = duration * 0.034 / 2;

  if (distance > 0 && distance < 30) {
    binPercent = map(distance, 30, 5, 0, 100);
    binPercent = constrain(binPercent, 0, 100);
  }

  // -------- GAS + TEMP --------
  gasValue = analogRead(GAS_PIN);

  float t = dht.readTemperature();
  if (!isnan(t)) temperature = t;

  // -------- ALERT --------
  if (binPercent > 90 || gasValue > 600 || temperature > 45) {
    digitalWrite(BUZZER, HIGH);
  } else {
    digitalWrite(BUZZER, LOW);
  }

  // -------- LCD --------
  lcd.setCursor(0, 0);
  lcd.print("Bin:");
  lcd.print(binPercent);
  lcd.print("%   ");

  lcd.setCursor(0, 1);
  lcd.print("T:");
  lcd.print(temperature);
  lcd.print(" G:");
  lcd.print(gasValue);
  lcd.print("   ");

  // -------- SERIAL --------
  Serial.println("----");
  Serial.print("Bin: "); Serial.println(binPercent);
  Serial.print("Gas: "); Serial.println(gasValue);
  Serial.print("Temp: "); Serial.println(temperature);

  // -------- IOT --------
  kiwisiot.send("1", String(binPercent));
  kiwisiot.send("2", String(gasValue));
  kiwisiot.send("3", String(temperature));
kiwisiot.send("0","Non-Metal");
  delay(500);
}
