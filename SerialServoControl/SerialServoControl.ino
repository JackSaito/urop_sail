#include <Servo.h>

Servo myServo;  // Create a servo object
const int ledPin = 13;  // Pin where LED is connected (usually built-in on pin 13)
int servoPos = 90;
int speed = 20;

void setup() {
  Serial.begin(9600);   // Start serial communication
  pinMode(ledPin, OUTPUT);  // Set the LED pin as an output
  myServo.attach(9);  // Attach servo to pin 9
  myServo.write(servoPos);
}

void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();  // Read the serial data
    switch (command) {
      case 'L':  // Clockwise
        servoPos -= speed;
        digitalWrite(ledPin, HIGH);
        break;
      case 'R':  // Counterclockwise
        servoPos += speed;
        digitalWrite(ledPin, HIGH);
        break;
      case 'O':  // Stop
         // Neutral position to stop movement
        digitalWrite(ledPin, LOW);
        break;
    }
    if (servoPos > 180) {
      servoPos = 180;
    }
    if (servoPos < 0) {
      servoPos = 0;
      
    }
    myServo.write(servoPos);  
    
  }
}
