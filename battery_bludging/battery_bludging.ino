#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ===== TEMP SENSOR =====
#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// ===== ULTRASONIC =====
#define TRIG_PIN 9
#define ECHO_PIN 10

// ===== OUTPUTS =====
#define GLED_PIN 3
#define RLED_PIN 4
#define BUZZER_PIN 5 

// ===== LCD =====
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ===== VARIABLES =====
float temperature = 0;
float distance = 0;
float baseDistance = 0;

#define TEMP_THRESHOLD 45  

// ===== SETUP =====
void setup() {
  Serial.begin(9600);

  sensors.begin();

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(GLED_PIN, OUTPUT);
  pinMode(RLED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  lcd.init();
  lcd.backlight();

  lcd.setCursor(0,0);
  lcd.print("Calibrating...");
  delay(3000);

  baseDistance = getDistance();

  lcd.clear();
  lcd.print("Base Dist:");
  lcd.setCursor(0,1);
  lcd.print(baseDistance);
  lcd.print(" cm");
  delay(2000);
}

// ===== LOOP =====
void loop() {

  // --- READ SENSORS ---
  sensors.requestTemperatures();
  temperature = sensors.getTempCByIndex(0);

  distance = getDistance();

  bool tempAlert = (temperature > TEMP_THRESHOLD);
  bool bulgeAlert = ((baseDistance - distance) >= 1.0);

  // ===== NORMAL STATE =====
  if (!tempAlert && !bulgeAlert) {
    digitalWrite(GLED_PIN, HIGH);
    digitalWrite(RLED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Normal");
    lcd.setCursor(0,1);
    lcd.print("T:");
    lcd.print(temperature);
    lcd.print(" D:");
    lcd.print(distance);
  }

  // ===== ALERT STATE =====
  else {
    digitalWrite(GLED_PIN, LOW);
    digitalWrite(RLED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);

    lcd.clear();

    if (tempAlert) {
      lcd.setCursor(0,0);
      lcd.print("TEMP HIGH!");
      lcd.setCursor(0,1);
      lcd.print(temperature);
    }

    if (bulgeAlert) {
      lcd.setCursor(0,0);
      lcd.print("BATTERY");
      lcd.setCursor(0,1);
      lcd.print("BULGING!");
    }
  }

  delay(1000);
}

// ===== FUNCTION =====
float getDistance() {
  long duration;

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);

  float dist = duration * 0.034 / 2;

  return dist;
}
