#include <Servo.h>

// Motor A
#define ENA 6
#define IN1 2
#define IN2 3

// Motor B
#define ENB 11
#define IN3 4
#define IN4 5

// Ultrasonic
#define TRIG A0
#define ECHO A1

Servo radarServo;

int speedMotor = 200;

long readDistance() {
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH, 30000);

  if (duration == 0)
    return 250;

  return duration * 0.034 / 2;
}

void forward() {
  analogWrite(ENA, speedMotor);
  analogWrite(ENB, speedMotor);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void backward() {
  analogWrite(ENA, speedMotor);
  analogWrite(ENB, speedMotor);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void left() {
  analogWrite(ENA, speedMotor);
  analogWrite(ENB, speedMotor);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);

  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void right() {
  analogWrite(ENA, speedMotor);
  analogWrite(ENB, speedMotor);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopRobot() {
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

int lookLeft() {
  radarServo.write(170);
  delay(500);
  return readDistance();
}

int lookRight() {
  radarServo.write(9);
  delay(500);
  return readDistance();
}

void setup() {
  Serial.begin(9600);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  radarServo.attach(10);
  radarServo.write(90);

  delay(1000);
}

void loop() {

  radarServo.write(90);
  delay(50);

  int front = readDistance();

  Serial.print("Front: ");
  Serial.println(front);

  if (front > 20) {

    forward();

  } else {

    stopRobot();
    delay(200);

    backward();
    delay(400);

    stopRobot();
    delay(200);

    int leftDist = lookLeft();

    radarServo.write(90);
    delay(300);

    int rightDist = lookRight();

    radarServo.write(90);
    delay(300);

    Serial.print("Left: ");
    Serial.println(leftDist);

    Serial.print("Right: ");
    Serial.println(rightDist);

    if (leftDist > rightDist) {
      left();
      delay(600);
    } else {
      right();
      delay(600);
    }

    stopRobot();
    delay(200);
  }
}
