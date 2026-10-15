#include <ESP8266WiFi.h>
#include <KiwisIoT.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ===== WIFI =====
 char* ssid  = "YOUR_WIFI_SSID";
 char* pass  = "YOUR_WIFI_PASSWORD";
 char* topic = "YOUR_DASHBOARD_TOPIC";

KiwisIoT kiwisiot(ssid, pass, topic);

// ===== LCD =====
LiquidCrystal_I2C lcd(0x27,16,2);

// ===== DHT =====
#define DHT_PIN D4
#define DHTTYPE DHT11
DHT dht(DHT_PIN, DHTTYPE);

// ===== PINS =====
#define SOUND_PIN  D5
#define LED_PIN    D6
#define BUZZER_PIN D7

unsigned long lastSend = 0;
const long interval = 3000;


// -------- CLOUD CONTROL --------
void onControl(String ch, String val)
{
  int channel = ch.toInt();

  if(channel == 4)
  {
    digitalWrite(LED_PIN, val == "1" ? HIGH : LOW);
  }

  if(channel == 5)
  {
    digitalWrite(BUZZER_PIN, val == "1" ? HIGH : LOW);
  }
}


void setup()
{
  Serial.begin(115200);

  pinMode(SOUND_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  lcd.init();
  lcd.backlight();

  dht.begin();

  kiwisiot.begin();
  kiwisiot.onReceive(onControl);

  lcd.setCursor(0,0);
  lcd.print("Baby Monitor");
  lcd.setCursor(0,1);
  lcd.print("Starting...");
  delay(2000);

  lcd.clear();

  Serial.println("Baby Monitor Started");
}


void loop()
{
  kiwisiot.run();

  unsigned long now = millis();

  if(now - lastSend > interval)
  {
    lastSend = now;

    int sound = digitalRead(SOUND_PIN);

    float temp = dht.readTemperature();
    float hum  = dht.readHumidity();

    // -------- Send data to IoT --------
    kiwisiot.send("0", sound);

    if(!isnan(temp) && !isnan(hum))
    {
      kiwisiot.send("1", temp);
      kiwisiot.send("2", hum);
    }

    // -------- LCD Display --------
    lcd.setCursor(0,0);
    lcd.print("T:");
    lcd.print(temp);
    lcd.print("C ");

    lcd.print("H:");
    lcd.print(hum);
    lcd.print("  ");

    lcd.setCursor(0,1);

    // -------- Cry Detection --------
    if(sound == LOW)   // sound detected
    {
      lcd.print("Baby Crying     ");

      digitalWrite(LED_PIN, HIGH);
      digitalWrite(BUZZER_PIN, HIGH);

      kiwisiot.send("4",1);
      kiwisiot.send("5",1);

      Serial.println("Baby Cry Detected");
    }
    else
    {
      lcd.print("Baby Sleeping   ");

      digitalWrite(LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);

      kiwisiot.send("4",0);
      kiwisiot.send("5",0);

      Serial.println("Baby Sleeping");
    }
  }
}
