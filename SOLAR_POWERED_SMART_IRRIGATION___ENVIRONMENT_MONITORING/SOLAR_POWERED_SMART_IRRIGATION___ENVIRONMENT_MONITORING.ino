

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <DHT.h>

// ============================================================
// PIN CONFIGURATION
// ============================================================

#define SOIL_PIN       A0
#define AIR_PIN        A1

#define DHT_PIN        4
#define DHT_TYPE       DHT11

#define RELAY_PIN      7

// ============================================================
// OBJECTS
// ============================================================

DHT dht(DHT_PIN, DHT_TYPE);

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ============================================================
// SETTINGS
// ============================================================

// Soil moisture threshold
// Lower value generally means drier soil for many resistive
// sensors. Check your sensor and calibrate this value.

#define SOIL_DRY_THRESHOLD 40

// Air quality threshold
#define AIR_ALERT_THRESHOLD 500

// ============================================================
// VARIABLES
// ============================================================

int soilRaw;
int soilPercent;

int airValue;

float temperature;
float humidity;

bool pumpStatus = false;



void setup()
{
  Serial.begin(9600);

  // Sensor pins
  pinMode(SOIL_PIN, INPUT);
  pinMode(AIR_PIN, INPUT);

  // Relay
  pinMode(RELAY_PIN, OUTPUT);

  // Initially pump OFF
  digitalWrite(RELAY_PIN, HIGH);

  // DHT11
  dht.begin();

  // LCD
  lcd.init();
  lcd.backlight();

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("SMART IRRIGATION");

  lcd.setCursor(0, 1);
  lcd.print("SYSTEM STARTING");

  delay(2000);

  lcd.clear();

  Serial.println("=================================");
  Serial.println("SOLAR SMART IRRIGATION SYSTEM");
  Serial.println("=================================");
}


void readSoilMoisture()
{
  soilRaw = analogRead(SOIL_PIN);


  soilPercent = map(soilRaw, 1023, 300, 0, 100);

  soilPercent = constrain(soilPercent, 0, 100);
}



void readDHT()
{
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();

  if (isnan(humidity) || isnan(temperature))
  {
    Serial.println("DHT11 ERROR");

    temperature = 0;
    humidity = 0;
  }
}


void readAirQuality()
{
  airValue = analogRead(AIR_PIN);
}


void controlPump()
{

  if (soilPercent <= SOIL_DRY_THRESHOLD)
  {
    pumpStatus = true;
    digitalWrite(RELAY_PIN, LOW);
  }

  else if (soilPercent >= 60)
  {
    pumpStatus = false;
    digitalWrite(RELAY_PIN, HIGH);
  }
}



void displayMainStatus()
{
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temperature, 1);

  lcd.print("C H:");
  lcd.print(humidity, 0);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("Soil:");
  lcd.print(soilPercent);
  lcd.print("%");

  delay(2500);
}

void displayAirQuality()
{
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Air:");
  lcd.print(airValue);

  lcd.setCursor(0, 1);

  if (airValue >= AIR_ALERT_THRESHOLD)
  {
    lcd.print("AIR QUALITY BAD");
  }
  else
  {
    lcd.print("AIR QUALITY GOOD");
  }

  delay(2000);
}


void displayPumpStatus()
{
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Water Pump:");

  lcd.setCursor(0, 1);

  if (pumpStatus)
  {
    lcd.print("ON - IRRIGATION");
  }
  else
  {
    lcd.print("OFF");
  }

  delay(2000);
}


void printSerialData()
{
  Serial.println("---------------------------------");

  Serial.print("Soil Raw       : ");
  Serial.println(soilRaw);

  Serial.print("Soil Moisture  : ");
  Serial.print(soilPercent);
  Serial.println("%");

  Serial.print("Temperature    : ");
  Serial.print(temperature);
  Serial.println(" C");

  Serial.print("Humidity       : ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Air Quality    : ");
  Serial.println(airValue);

  Serial.print("Water Pump     : ");

  if (pumpStatus)
  {
    Serial.println("ON");
  }
  else
  {
    Serial.println("OFF");
  }

  Serial.println("---------------------------------");
}

void loop()
{
  // Read all sensors
  readSoilMoisture();
  readDHT();
  readAirQuality();

  // Automatic irrigation
  controlPump();

  // Serial monitor
  printSerialData();

  // LCD pages
  displayMainStatus();
  displayAirQuality();
  displayPumpStatus();

  delay(500);
}
