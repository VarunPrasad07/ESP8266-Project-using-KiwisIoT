#define PULSE_PIN A0

int signalValue = 0;

int threshold = 550;

bool pulseDetected = false;

unsigned long lastBeatTime = 0;
int heartRate = 0;

void setup()
{
  Serial.begin(115200);

  pinMode(PULSE_PIN, INPUT);

  Serial.println();
  Serial.println("==============================");
  Serial.println("   HEART RATE SENSOR");
  Serial.println("==============================");
  Serial.println("Place your finger on sensor");
}

void loop()
{
  signalValue = analogRead(PULSE_PIN);

  unsigned long currentTime = millis();

  // Detect heartbeat
  if (signalValue > threshold && !pulseDetected)
  {
    pulseDetected = true;

    // Calculate time between beats
    if (lastBeatTime > 0)
    {
      unsigned long beatInterval = currentTime - lastBeatTime;

      // Convert interval to BPM
      if (beatInterval > 300 && beatInterval < 2000)
      {
        heartRate = 60000 / beatInterval;

        Serial.print("Heart Rate: ");
        Serial.print(heartRate);
        Serial.println(" BPM");
      }
    }

    lastBeatTime = currentTime;
  }

  // Wait until signal goes below threshold
  if (signalValue < threshold)
  {
    pulseDetected = false;
  }

  // Show raw sensor value
  Serial.print("Signal: ");
  Serial.print(signalValue);
  Serial.print(" | BPM: ");
  Serial.println(heartRate);

  delay(10);
}
