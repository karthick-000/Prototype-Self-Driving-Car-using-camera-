#include <Wire.h>

// Motor Control Pins
#define IN1 9
#define IN2 8
#define IN3 7
#define IN4 6

// Ultrasonic Sensor Pins
#define TRIG1 5
#define ECHO1 4
#define TRIG2 3
#define ECHO2 2

// Optional IR Sensors (not used here)
#define IR_LEFT A0
#define IR_RIGHT A1

String currentCommand = "STOP";  // Holds last command

void setup() {
  Serial.begin(9600);
  Wire.begin(8);  // I2C slave address
  Wire.onReceive(receiveCommand);

  // Setup Motor Pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  // Setup Ultrasonic Sensor Pins
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);

  // Optional IR Pins
  pinMode(IR_LEFT, INPUT);
  pinMode(IR_RIGHT, INPUT);

  Serial.println("✅ System Initialized");
}

void loop() {
  int distance1 = getDistance(TRIG1, ECHO1);
  int distance2 = getDistance(TRIG2, ECHO2);

  // Display distance
  Serial.print("Distance1: ");
  Serial.print(distance1);
  Serial.print(" cm | Distance2: ");
  Serial.print(distance2);
  Serial.println(" cm");

  // Stop if object detected within 10cm
  if (distance1 < 10 || distance2 < 10) {
    stopMotors();
    Serial.println("🚨 Obstacle <10cm! Stopping.");
  } else {
    // Follow the last received command
    if (currentCommand == "FORWARD") {
      moveForward();
      Serial.println("⬆️ Moving Forward...");
    } else if (currentCommand == "LEFT") {
      turnLeft();
      Serial.println("⬅️ Turning Left...");
    } else if (currentCommand == "RIGHT") {
      turnRight();
      Serial.println("➡️ Turning Right...");
    } else if (currentCommand == "STOP") {
      stopMotors();
      Serial.println("⛔ Stopped.");
    } else {
      stopMotors();
      Serial.println("❓ Unknown Command: " + currentCommand);
    }
  }

  delay(300);  // Loop delay
}

// Handle I2C command
void receiveCommand(int numBytes) {
  String command = "";
  while (Wire.available()) {
    char c = Wire.read();
    command += c;
  }
  command.trim();
  if (command.length() > 0) {
    currentCommand = command;
    Serial.println("📥 I2C Command Received: " + currentCommand);
  }
}

// Motor control functions
void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// Distance measurement using ultrasonic
int getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000); // 30ms timeout
  int distance = duration * 0.034 / 2;

  // Return max range if no object detected
  if (distance == 0 || distance > 400) return 400;
  return distance;
}
