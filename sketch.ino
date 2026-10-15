#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// 1. Initialize the I2C LCD (Address 0x27)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// 2. Pin Definitions 
int buzzer = A0;   // RED WIRE MUST BE IN THE A0 HOLE
int trigPin = A3;  // Orange wire
int echoPin = 10;  // Purple wire
int redLED = A1;
int greenLED = A2;

// 3. Keypad Setup
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {2, 3, 4, 5}; 
byte colPins[COLS] = {6, 7, 8, 9};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

bool isArmed = false;
String input = "";

void setup() {
  lcd.init();
  lcd.backlight();
  pinMode(buzzer, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(redLED, OUTPUT);
  pinMode(greenLED, OUTPUT);

  lcd.print("SYSTEM ONLINE");
  delay(1500);
  lcd.clear();
}

void loop() {
  char key = keypad.getKey();

  // --- KEYPAD & PIN LOGIC ---
  if (key) {
    input += key;
    lcd.setCursor(0, 1);
    lcd.print("PIN: "); lcd.print(input);

    if (input.length() == 4) {
      if (input == "1234") { // CODE TO ARM (ON)
        isArmed = true;
        lcd.clear();
        lcd.print("SYSTEM ARMED");
        tone(buzzer, 1000, 200); 
      } 
      else if (input == "0000") { // CODE TO DISARM (OFF)
        isArmed = false;
        noTone(buzzer);
        lcd.clear();
        lcd.print("SYSTEM OFF");
        tone(buzzer, 600, 500); 
      } 
      else {
        lcd.clear();
        lcd.print("INVALID PIN");
      }
      input = "";
      delay(1500);
      lcd.clear();
    }
  }

  // --- SECURITY LOGIC ---
  if (isArmed) {
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, HIGH);

    // Get distance from Ultrasonic Sensor
    digitalWrite(trigPin, LOW); delayMicroseconds(2);
    digitalWrite(trigPin, HIGH); delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    long dist = pulseIn(echoPin, HIGH) * 0.034 / 2;

    lcd.setCursor(0, 0);
    lcd.print("MONITORING...   ");

    if (dist > 0 && dist < 30) { // If someone is closer than 30cm
      tone(buzzer, 2000); 
      lcd.setCursor(0, 1);
      lcd.print("!! INTRUDER !! ");
    } else {
      noTone(buzzer);
    }
  } else {
    // System Disarmed State
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
    noTone(buzzer);
    lcd.setCursor(0, 0);
    lcd.print("ENTER PIN...    ");
  }
}