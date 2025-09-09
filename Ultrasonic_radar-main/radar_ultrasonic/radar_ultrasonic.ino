#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

// Initialize I2C LCD (Default address: 0x27, adjust if necessary)
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define trigPin 6
#define echoPin 8

#define RGB_R 10
#define RGB_G 11
#define RGB_B 12

long duration;
int distance;

Servo myservo;

int calculateDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2; // Adjusted factor to match 20 cm distance
  return distance;
}

void setup() {
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  myservo.attach(3);
  Serial.begin(9600);
  
  pinMode(RGB_R, OUTPUT);
  pinMode(RGB_G, OUTPUT);
  pinMode(RGB_B, OUTPUT);
  
  // I2C LCD Initialization
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Pendulum Ready");
}

void loop() {
  for (int i = 15; i <= 165; i++) {
    if (checkAndHandleObject(i)) return; // Halt if an object is detected
    myservo.write(i);
    delay(15);
    updateRGBFromAngle(i);
  }
  for (int i = 165; i >= 15; i--) {
    if (checkAndHandleObject(i)) return; // Halt if an object is detected
    myservo.write(i);
    delay(15);
    updateRGBFromAngle(i);
  }
}

void updateRGBFromAngle(int angle) {
  int red, green, blue;

  if (angle <= 90) {
    // From 60° (blue) to 90° (green)
    blue = map(angle, 60, 90, 255, 0);
    green = map(angle, 60, 90, 0, 255);
    red = 0;
  } else {
    // From 90° (green) to 120° (red)
    green = map(angle, 90, 120, 255, 0);
    red = map(angle, 90, 120, 0, 255);
    blue = 0;
  }

  analogWrite(RGB_R, red);
  analogWrite(RGB_G, green);
  analogWrite(RGB_B, blue);
}

bool checkAndHandleObject(int angle) {
  int currentDistance = calculateDistance();
  Serial.print(angle);
  Serial.print(",");
  Serial.print(currentDistance);
  Serial.print(".");

  if (currentDistance < 20) { // Threshold distance in cm
    handleObjectDetected();
    while (calculateDistance() < 20) {
      // Stay halted while the object is detected
      delay(1000);
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Pendulum Ready");
    return true; // Exit the loop and stop further motion
  }
  return false;
}

void handleObjectDetected() {
  // Action when object is detected
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Object Detected!");
  
  // Set RGB LED to red
  analogWrite(RGB_R, 255);
  analogWrite(RGB_G, 0);
  analogWrite(RGB_B, 0);
}

