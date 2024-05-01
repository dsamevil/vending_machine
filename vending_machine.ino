#include <EEPROM.h>

#define COIN_ACCEPTOR_PIN 2
#define MOTOR_PIN_1 7
#define MOTOR_PIN_2 6
#define LED_PIN 12 // Define LED pin
#define BUTTON_PIN 10 // Define button pin
#define GROUND_PIN 9 // Define ground pin
#define RELAY_PIN 4 // Define relay pin for the machine

#define MOTOR_ROTATION_TIME 1060
#define EEPROM_ADDRESS 0 // Starting EEPROM address for storing data

int impulsCount = 0;
float total_amount = 0.0;
int items_remaining = 23; // Change the initial value to 24
int i = 0;
bool buttonPressed = false;

void setup() {
  Serial.begin(9600);
  pinMode(COIN_ACCEPTOR_PIN, INPUT);
  pinMode(MOTOR_PIN_1, OUTPUT);
  pinMode(MOTOR_PIN_2, OUTPUT);
  pinMode(LED_PIN, OUTPUT); // Set LED pin as output
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Set button pin as input with internal pull-up resistor
  pinMode(GROUND_PIN, OUTPUT); // Set ground pin as output
  pinMode(RELAY_PIN, OUTPUT); // Set relay pin for the machine

  digitalWrite(GROUND_PIN, LOW); // Set ground pin to LOW

  digitalWrite(MOTOR_PIN_1, LOW);
  digitalWrite(MOTOR_PIN_2, LOW);
  digitalWrite(RELAY_PIN, LOW); // Turn on vending machine initially
  digitalWrite(LED_PIN, HIGH); // Turn on LED initially

  attachInterrupt(0, incomingImpuls, FALLING);

  // Restore data from EEPROM if it's valid
  if (EEPROM.read(EEPROM_ADDRESS) != 255 && EEPROM.read(EEPROM_ADDRESS + sizeof(total_amount)) != 255) {
    EEPROM.get(EEPROM_ADDRESS, total_amount);
    EEPROM.get(EEPROM_ADDRESS + sizeof(total_amount), items_remaining);
  } else {
    // Initialize EEPROM data if it's not valid
    total_amount = 0.0;
    items_remaining = 23; // Change the initial value to 24
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
  // Serial.print("\tTotal Amount: ");
  // Serial.print(total_amount);
  Serial.print("\tItems Remaining: ");
  Serial.print(items_remaining);
  Serial.print("\tImpuls Count: ");
  Serial.println(impulsCount);
  i = i + 1;

  if (i >= 30) {
    if (impulsCount == 10 && items_remaining > 0) {
      total_amount = total_amount + 5;
      dispense();
      items_remaining--; // Decrease the count of items remaining
    }
    impulsCount = 0; // Reset impulsCount to zero after every 30 iterations
    i = 0; // Reset i to zero after every 30 iterations
  }

  if (items_remaining == 0 && !buttonPressed) {
    // Blink the LED if items count is zero and button is not pressed
    blinkLED();
    digitalWrite(RELAY_PIN, HIGH); // Turn off vending machine
  } else {
    digitalWrite(RELAY_PIN, LOW); // Turn on vending machine
  }

  // Check if button is pressed
  if (digitalRead(BUTTON_PIN) == LOW) {
    resetMachine();
  }

  // Save data to EEPROM every time the items_remaining changes
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
  // Blink the LED (500ms ON, 500ms OFF)
  digitalWrite(LED_PIN, HIGH);
  delay(400);
  digitalWrite(LED_PIN, LOW);
  delay(400);
}

void blinkLED2() {
  // Blink the LED (500ms ON, 500ms OFF)
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
  items_remaining = 23; // Reset items count to 24
  buttonPressed = true; // Set buttonPressed flag to true
  digitalWrite(LED_PIN, HIGH); // Turn off LED blinking
  digitalWrite(GROUND_PIN, HIGH); // Set ground pin to HIGH
  delay(1000); // Wait for debounce
  digitalWrite(GROUND_PIN, LOW); // Set ground pin to LOW
  buttonPressed = false; // Reset buttonPressed flag
}
