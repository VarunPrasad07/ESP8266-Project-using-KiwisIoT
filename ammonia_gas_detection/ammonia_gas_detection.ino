#include <WiFi.h>
#include <KiwisIoT.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// =================================================
// WIFI + KIWISIOT
// =================================================

 char* ssid  = "YOUR_WIFI_SSID";
 char* pass  = "YOUR_WIFI_PASSWORD";
 char* topic = "YOUR_DASHBOARD_TOPIC";

KiwisIoT kiwisiot(ssid, pass, topic);


// =================================================
// SENSOR PINS
// =================================================

#define DHT_PIN       4
#define DHT_TYPE      DHT11

#define MQ2_PIN       34

#define BUZZER_PIN    25
#define DANGER_LED    26
#define RELAY_PIN     27   // NEW: relay control


// =================================================
// OLED PINS
// =================================================

#define OLED_SDA      21
#define OLED_SCL      22

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);


// =================================================
// DHT
// =================================================

DHT dht(DHT_PIN, DHT_TYPE);


// =================================================
// GAS THRESHOLD
// =================================================

#define GAS_THRESHOLD 500       // updated from 2000
#define GAS_MAX       4095      // for bar graph scaling (ESP32 ADC 12-bit)


// =================================================
// RELAY LOGIC
// =================================================

#define RELAY_ACTIVE_HIGH true  // set false if your relay module is active-LOW

inline void setRelay(bool on)
{
  if (RELAY_ACTIVE_HIGH)
    digitalWrite(RELAY_PIN, on ? HIGH : LOW);
  else
    digitalWrite(RELAY_PIN, on ? LOW : HIGH);
}


// =================================================
// SETUP
// =================================================

void setup()
{
  Serial.begin(115200);

  // Sensor pins
  pinMode(MQ2_PIN, INPUT);

  // Output pins
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(DANGER_LED, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);

  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(DANGER_LED, LOW);
  setRelay(false);

  // DHT
  dht.begin();

  // OLED
  Wire.begin(OLED_SDA, OLED_SCL);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println("OLED NOT FOUND!");

    while (1);
  }

  // Starting display
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  display.setTextSize(2);
  display.setCursor(15, 5);
  display.println("AMMONIA");

  display.setTextSize(1);
  display.setCursor(25, 30);
  display.println("SAFETY SYSTEM");

  display.setCursor(30, 45);
  display.println("Connecting...");

  display.display();

  delay(2000);


  // =================================================
  // KIWISIOT START
  // =================================================

  kiwisiot.begin();

  Serial.println("==============================");
  Serial.println("AMMONIA SAFETY SYSTEM");
  Serial.println("KiwisIoT Started");
  Serial.println("==============================");
}


// =================================================
// OLED HELPER: draw a horizontal gas level bar
// =================================================

void drawGasBar(int x, int y, int w, int h, int value, int maxValue, int threshold)
{
  display.drawRect(x, y, w, h, SSD1306_WHITE);

  int fillW = map(constrain(value, 0, maxValue), 0, maxValue, 0, w - 2);
  if (fillW > 0)
  {
    display.fillRect(x + 1, y + 1, fillW, h - 2, SSD1306_WHITE);
  }

  // threshold marker as a small tick above the bar
  int tickX = x + map(threshold, 0, maxValue, 0, w - 2) + 1;
  display.drawFastVLine(tickX, y - 3, 3, SSD1306_WHITE);
}


// =================================================
// LOOP
// =================================================

void loop()
{
  // =================================================
  // READ DHT11
  // =================================================

  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();


  // =================================================
  // READ MQ-2
  // =================================================

  int gasValue = analogRead(MQ2_PIN);


  // =================================================
  // CHECK DHT
  // =================================================

  if (isnan(temperature) || isnan(humidity))
  {
    Serial.println("DHT11 ERROR");

    display.clearDisplay();

    display.setTextColor(SSD1306_WHITE);

    display.setTextSize(2);
    display.setCursor(20, 10);
    display.println("DHT ERROR");

    display.setTextSize(1);
    display.setCursor(15, 40);
    display.println("Check Sensor");

    display.display();

    delay(2000);

    return;
  }


  // =================================================
  // DANGER DETECTION
  // =================================================

  bool danger = (gasValue >= GAS_THRESHOLD);


  // =================================================
  // BUZZER + LED + RELAY
  // =================================================

  digitalWrite(BUZZER_PIN, danger ? HIGH : LOW);
  digitalWrite(DANGER_LED, danger ? HIGH : LOW);
  setRelay(danger);   // NEW: relay follows danger state


  // =================================================
  // SYSTEM STATUS
  // =================================================

  String status = danger ? "DANGER" : "NORMAL";


  // =================================================
  // SERIAL MONITOR
  // =================================================

  Serial.println("-----------------------------");

  Serial.print("Temperature : ");
  Serial.print(temperature);
  Serial.println(" C");

  Serial.print("Humidity    : ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("MQ-2 Gas    : ");
  Serial.println(gasValue);

  Serial.print("Danger      : ");
  Serial.println(danger);

  Serial.print("Buzzer      : ");
  Serial.println(danger);

  Serial.print("LED         : ");
  Serial.println(danger);

  Serial.print("Relay       : ");
  Serial.println(danger);

  Serial.print("Status      : ");
  Serial.println(status);


  // =================================================
  // OLED DISPLAY (redesigned layout)
  // =================================================

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  // ---- Top status banner (inverted when danger) ----
  if (danger)
  {
    display.fillRect(0, 0, SCREEN_WIDTH, 16, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
    display.setTextSize(2);
    display.setCursor(20, 2);
    display.println("DANGER!");
    display.setTextColor(SSD1306_WHITE);
  }
  else
  {
    display.setTextSize(2);
    display.setCursor(20, 2);
    display.println("NORMAL");
  }

  // ---- Divider line ----
  display.drawFastHLine(0, 18, SCREEN_WIDTH, SSD1306_WHITE);

  // ---- Temp / Humidity row ----
  display.setTextSize(1);
  display.setCursor(0, 24);
  display.print("T:");
  display.print(temperature, 1);
  display.print("C");

  display.setCursor(70, 24);
  display.print("H:");
  display.print(humidity, 1);
  display.print("%");

  // ---- Gas value ----
  display.setCursor(0, 36);
  display.print("Gas: ");
  display.print(gasValue);
  display.print(" / ");
  display.print(GAS_THRESHOLD);

  // ---- Gas level bar with threshold marker ----
  drawGasBar(0, 50, SCREEN_WIDTH - 1, 8, gasValue, GAS_MAX, GAS_THRESHOLD);

  // ---- Relay indicator (small corner icon) ----
  display.setCursor(100, 24);
  display.print(danger ? "R:ON" : "R:OF");

  display.display();


  // =================================================
  // SEND TO KIWISIOT
  // =================================================

  // Channel 0 = Temperature
  kiwisiot.send("0", temperature);

  // Channel 1 = Humidity
  kiwisiot.send("1", humidity);

  // Channel 2 = MQ-2 Gas
  kiwisiot.send("2", gasValue);

  // Channel 3 = Relay/Danger state (0/1)
  kiwisiot.send("3", danger ? 1 : 0);


  // Process KiwisIoT communication
  kiwisiot.run();


  delay(2000);
}
