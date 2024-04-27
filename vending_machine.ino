#include <EEPROM.h>
#define COIN_ACCEPTOR_PIN 2
#define MOTOR_PIN_1 7
#define MOTOR_PIN_2 6
#define LED_PIN 12
#define BUTTON_PIN 10
#define GROUND_PIN 9
#define RELAY_PIN 4
#define MOTOR_ROTATION_TIME 1060
#define EEPROM_ADDRESS 0

int impulsCount = 0;
float total_amount = 0.0;
int items_remaining = 23;
int i = 0;
bool buttonPressed = false;

void setup() {
  Serial.begin(9600);
  pinMode(COIN_ACCEPTOR_PIN, INPUT);
  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(GROUND_PIN, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  
  digitalWrite(GROUND_PIN, LOW);
  digitalWrite(MOTOR_PIN_1, LOW);
  digitalWrite(MOTOR_PIN_2, LOW);
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_PIN, HIGH);

  attachInterrupt(0, incomingImpuls, FALLING);

  if (EEPROM.read(EEPROM_ADDRESS) != 255 && EEPROM.read(EEPROM_ADDRESS + sizeof(total_amount)) != 255) {
    EEPROM.get(EEPROM_ADDRESS, total_amount);
    EEPROM.get(EEPROM_ADDRESS + sizeof(total_amount), items_remaining);
  } else {
    total_amount = 0.0;
    items_remaining = 23;
    EEPROM.put(EEPROM_ADDRESS, total_amount);
    EEPROM.put(EEPROM_ADDRESS + sizeof(total_amount), items_remaining);
  }
}

void incomingImpuls() {
  impulsCount = impulsCount + 1;
  i = 0;
}

void loop() {
  Serial.print("i: ");
  Serial.print(i);
  Serial.print("\tItems Remaining: ");
  Serial.print(items_remaining);
  Serial.print("\tImpuls Count: ");
  Serial.println(impulsCount);
  i = i + 1;

  if (i >= 30) {
    if (impulsCount == 10 && items_remaining > 0) {
      total_amount = total_amount + 5;
      dispense();
      items_remaining--;
    }
    impulsCount = 0;
    i = 0;
  }

  if (items_remaining == 0 && !buttonPressed) {
    blinkLED();
    digitalWrite(RELAY_PIN, HIGH);
  } else {
    digitalWrite(RELAY_PIN, LOW);
  }

  if (digitalRead(BUTTON_PIN) == LOW) {
    resetMachine();
  }

  static int prevItems = items_remaining;
  if (items_remaining != prevItems) {
    EEPROM.put(EEPROM_ADDRESS, total_amount);
    EEPROM.put(EEPROM_ADDRESS + sizeof(total_amount), items_remaining);
    prevItems = items_remaining;
  }
}

void dispense() {
  digitalWrite(MOTOR_PIN_1, LOW);
  digitalWrite(MOTOR_PIN_2, HIGH);
  delay(MOTOR_ROTATION_TIME);
  digitalWrite(MOTOR_PIN_1, LOW);
  digitalWrite(MOTOR_PIN_2, LOW);
  blinkLED2();
  digitalWrite(LED_PIN, HIGH);
}

void blinkLED() {
  digitalWrite(LED_PIN, HIGH);
  delay(400);
  digitalWrite(LED_PIN, LOW);
  delay(400);
}

void blinkLED2() {
  digitalWrite(LED_PIN, LOW);
  delay(50);
  digitalWrite(LED_PIN, HIGH);
  delay(50);
  digitalWrite(LED_PIN, LOW);
  delay(50);
  digitalWrite(LED_PIN, HIGH);
  delay(50);
  digitalWrite(LED_PIN, LOW);
  delay(50);
  digitalWrite(LED_PIN, HIGH);
  delay(50);
  digitalWrite(LED_PIN, LOW);
  delay(50);
  digitalWrite(LED_PIN, HIGH);
  delay(50);
}

void resetMachine() {
  items_remaining = 23;
  buttonPressed = true;
  digitalWrite(LED_PIN, HIGH);
  digitalWrite(GROUND_PIN, HIGH);
  delay(1000);
  digitalWrite(GROUND_PIN, LOW);
  buttonPressed = false;
}
