#include <Wire.h>
#include <WiFi.h>

const char* ssid = "abinesh";      // Update with your WiFi SSID
const char* password = "12345678";  // Update with your WiFi Password

WiFiServer tcpServer(1234);         // TCP server on port 1234
#define ARDUINO_ADDRESS 8          // I2C address of the Arduino Uno

void setup() {
  Serial.begin(115200);
  Wire.begin();                    // Initialize I2C as master
  
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());
  
  tcpServer.begin();
}

void sendCommandToArduino(String cmd) {
  Wire.beginTransmission(ARDUINO_ADDRESS);
  // Send the command as a series of bytes:
  Wire.write((uint8_t*)cmd.c_str(), cmd.length());
  Wire.endTransmission();
}

void loop() {
  WiFiClient client = tcpServer.available();
  if (client && client.connected()) {
    String command = client.readStringUntil('\n');
    command.trim();
    if (command.length() > 0) {
      Serial.println("Received: " + command);
      sendCommandToArduino(command);
    }
  }
}
