/*
Detect motor specification

Wire Connection
1. OLED Display (SSD1306 128x64 I2C)
OLED Pin	Connect to Arduino
VCC	5V
GND	GND
SCL	A5 (for UNO)
SDA	A4 (for UNO)
2. Battery (7.4V):
  + → VCC (L298N)
  - → GND (L298N) + GND (Arduino)
3. Motor Driver (L298N / H-Bridge-like circuit)
Assuming a single DC motor:
Motor Driver Pin	Connect to Arduino
IN1	Pin 8
IN2	Pin 9
ENA	Pin 10 (PWM)
GND	GND
VCC	5V (or external 9-12V)
Motor Output A/B	Connected to motor

⚠️ Note: Ensure motor driver has sufficient power. 
For large motors, use an external power supply and 
common ground between Arduino and driver.
*/


#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Motor pins
int in1 = 8;
int in2 = 9;
int enA = 6;  // PWM pin for speed control

// OLED display size
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int count = 0;              // Count of direction changes
bool forward = true;        // Direction flag
int speedPWM = 150;         // Initial speed (PWM: 0–255)
int loopPhase = 0;          // 0 = forward, 1 = backward

void setup() {
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enA, OUTPUT);

  Serial.begin(9600);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.display();
}

void loop() {
  count++;  // Increment direction change count

  display.clearDisplay();

  // Motor specs and status
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Motor: DC Motor");

  display.setCursor(0, 10);
  display.print("Spec: 6V, 200RPM");

  display.setCursor(0, 20);
  display.print("Count: ");
  display.print(count);

  display.setCursor(0, 30);
  display.print("Speed: ");
  display.print(speedPWM);

  display.setCursor(0, 40);
  display.print("Dir: ");
  if (forward) {
    display.print("Forward (+)");
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  } else {
    display.print("Backward (-)");
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }

  analogWrite(enA, speedPWM);  // Apply speed
  display.display();

  delay(3000);  // Wait 3 seconds instead of 6

  forward = !forward;  // Toggle direction
  loopPhase++;

  // After every full loop (forward + backward), increase speed
  if (loopPhase >= 2) {
    loopPhase = 0;
    if (speedPWM <= 245) {
      speedPWM += 10;  // Gradually speed up
    }
  }
}
