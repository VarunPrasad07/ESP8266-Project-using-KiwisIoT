/*
 * Kiwistron
 * Environmental Monitoring System
 * Arduino UNO -> NodeMCU -> KiwisIoT
 *
 * Data:
 * 1 = Gas
 * 2 = Noise
 * 3 = Temperature
 * 4 = Humidity
 * 5 = System Status
 */

#include <KiwisIoT.h>

// =====================================================
// KIWISIOT CONFIGURATION
// =====================================================

 char* ssid  = "YOUR_WIFI_SSID";
 char* pass  = "YOUR_WIFI_PASSWORD";
 char* topic = "YOUR_DASHBOARD_TOPIC";

KiwisIoT kiwisiot(ssid, pass, topic);

// =====================================================
// VARIABLES
// =====================================================

int gasValue = 0;
int noiseValue = 0;

float temperature = 0.0;
float humidity = 0.0;

String status = "NORMAL";

// =====================================================
// SERIAL INPUT BUFFER
// =====================================================

String receivedData = "";

// =====================================================
// SETUP
// =====================================================

void setup()
{
  Serial.begin(9600);

  delay(1000);

  Serial.println();
  Serial.println("================================");
  Serial.println(" Environmental Monitoring");
  Serial.println(" NodeMCU + KiwisIoT");
  Serial.println("================================");

  // Start KiwisIoT
  kiwisiot.begin();

  Serial.println("KiwisIoT Started");
}

// =====================================================
// MAIN LOOP
// =====================================================

void loop()
{
  // Maintain KiwisIoT connection
  kiwisiot.run();

  // Read data coming from Arduino UNO
  readArduinoData();

  // Small delay
  delay(100);
}

// =====================================================
// READ DATA FROM ARDUINO UNO
// =====================================================

void readArduinoData()
{
  if (Serial.available())
  {
    receivedData = Serial.readStringUntil('\n');

    receivedData.trim();

    if (receivedData.length() > 0)
    {
      Serial.print("Received: ");
      Serial.println(receivedData);

      parseData(receivedData);
    }
  }
}

// =====================================================
// PARSE DATA
// =====================================================

void parseData(String data)
{
  /*
     Expected data:

     GAS,450,NOISE,320,TEMP,29.5,HUM,65,STATUS,NORMAL
  */

  int gasIndex = data.indexOf("GAS,");
  int noiseIndex = data.indexOf(",NOISE,");
  int tempIndex = data.indexOf(",TEMP,");
  int humIndex = data.indexOf(",HUM,");
  int statusIndex = data.indexOf(",STATUS,");

  // Check if complete packet received
  if (gasIndex == -1 ||
      noiseIndex == -1 ||
      tempIndex == -1 ||
      humIndex == -1 ||
      statusIndex == -1)
  {
    Serial.println("Invalid data received");

    return;
  }

  // ===================================================
  // GAS
  // ===================================================

  String gasString = data.substring(
                       gasIndex + 4,
                       noiseIndex
                     );

  gasValue = gasString.toInt();

  // ===================================================
  // NOISE
  // ===================================================

  String noiseString = data.substring(
                         noiseIndex + 7,
                         tempIndex
                       );

  noiseValue = noiseString.toInt();

  // ===================================================
  // TEMPERATURE
  // ===================================================

  String tempString = data.substring(
                        tempIndex + 6,
                        humIndex
                      );

  temperature = tempString.toFloat();

  // ===================================================
  // HUMIDITY
  // ===================================================

  String humString = data.substring(
                       humIndex + 5,
                       statusIndex
                     );

  humidity = humString.toFloat();

  // ===================================================
  // STATUS
  // ===================================================

  status = data.substring(statusIndex + 8);

  status.trim();

  // ===================================================
  // PRINT DATA
  // ===================================================

  Serial.println("--------------------------------");

  Serial.print("Gas         : ");
  Serial.println(gasValue);

  Serial.print("Noise       : ");
  Serial.println(noiseValue);

  Serial.print("Temperature : ");
  Serial.print(temperature);
  Serial.println(" C");

  Serial.print("Humidity    : ");
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Status      : ");
  Serial.println(status);

  Serial.println("--------------------------------");

  // ===================================================
  // SEND TO KIWISIOT
  // ===================================================

  sendToKiwisIoT();
}

// =====================================================
// SEND DATA TO KIWISIOT
// =====================================================

void sendToKiwisIoT()
{
  /*
     Dashboard channel mapping

     Channel 1 -> Gas
     Channel 2 -> Noise
     Channel 3 -> Temperature
     Channel 4 -> Humidity
     Channel 5 -> Alert Status
  */

  kiwisiot.send("1", gasValue);

  kiwisiot.send("2", noiseValue);

  kiwisiot.send("3", temperature);

  kiwisiot.send("4", humidity);

  // Status converted to numerical value
  // 0 = NORMAL
  // 1 = ALERT

  if (status == "ALERT")
  {
    kiwisiot.send("5", 1);
  }
  else
  {
    kiwisiot.send("5", 0);
  }

  Serial.println("Data sent to KiwisIoT");
}
