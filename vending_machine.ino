#define VRX_PIN  A1
#define VRY_PIN  A2

#define LEFT_THRESHOLD  400
#define RIGHT_THRESHOLD 800

#define UP_THRESHOLD    400
#define DOWN_THRESHOLD  800
#define TOLERANCE       50 // Adjust tolerance as needed

#define COMMAND_NO     0x00
#define COMMAND_LEFT   0x01
#define COMMAND_RIGHT  0x02
#define COMMAND_UP     0x04
#define COMMAND_DOWN   0x08

#define RPWM1 5
#define LPWM1 6
#define REN1  8
#define LEN1  9

#define RPWM2 10
#define LPWM2 11
#define REN2  13
#define LEN2  122

int xValue = 0;
int yValue = 0;
int command = COMMAND_NO;
int motorSpeedLeftRight = 100; // Maximum speed for left/right movements
int motorSpeedUpDown = 75;     // Maximum speed for up/down movements
int rampUpSteps = 20;           // Number of steps for ramping up
int motorSpeed = 0;

void setup() {
  Serial.begin(9600);
  pinMode(RPWM1, OUTPUT);
  pinMode(LPWM1, OUTPUT);
  pinMode(LEN1, OUTPUT);
  pinMode(REN1, OUTPUT);
  digitalWrite(REN1, HIGH);
  digitalWrite(LEN1, HIGH);

  pinMode(RPWM2, OUTPUT);
  pinMode(LPWM2, OUTPUT);
  pinMode(LEN2, OUTPUT);
  pinMode(REN2, OUTPUT);
  digitalWrite(REN2, HIGH);
  digitalWrite(LEN2, HIGH);
}

void loop() {
  xValue = analogRead(VRX_PIN);
  yValue = analogRead(VRY_PIN);

  command = COMMAND_NO;

  if (xValue < LEFT_THRESHOLD - TOLERANCE)
    command = command | COMMAND_LEFT;
  else if (xValue > RIGHT_THRESHOLD + TOLERANCE)
    command = command | COMMAND_RIGHT;

  if (yValue < UP_THRESHOLD - TOLERANCE)
    command = command | COMMAND_UP;
  else if (yValue > DOWN_THRESHOLD + TOLERANCE)
    command = command | COMMAND_DOWN;

  // Update motor speed based on the joystick position
  if (command & COMMAND_LEFT || command & COMMAND_RIGHT) {
    rampUpSpeed(motorSpeedLeftRight);
  } else if (command & COMMAND_UP || command & COMMAND_DOWN) {
    rampUpSpeed(motorSpeedUpDown);
  } else {
    motorSpeed = 0; // If no command, stop the motor immediately
  }

  // Apply motor control
  applyMotorControl();

  delay(50); // Adjust delay as needed
}

// Function to gradually increase motor speed from 0 to targetSpeed
void rampUpSpeed(int targetSpeed) {
  if (motorSpeed < targetSpeed) {
    motorSpeed += targetSpeed / rampUpSteps;
  }
}

// Function to apply motor control based on the current command and motor speed
void applyMotorControl() {
  if (command & COMMAND_LEFT) {
    Serial.println("COMMAND LEFT");
    analogWrite(LPWM1, 0);
    analogWrite(RPWM1, motorSpeed);
    analogWrite(LPWM2, 0);
    analogWrite(RPWM2, motorSpeed);
  } else if (command & COMMAND_RIGHT) {
    Serial.println("COMMAND RIGHT");
    analogWrite(LPWM1, motorSpeed);
    analogWrite(RPWM1, 0);
    analogWrite(LPWM2, motorSpeed);
    analogWrite(RPWM2, 0);
  } else if (command & COMMAND_UP) {
    Serial.println("COMMAND UP");
    analogWrite(LPWM1, motorSpeed);
    analogWrite(RPWM1, 0);
    analogWrite(LPWM2, 0);
    analogWrite(RPWM2, motorSpeed);
  } else if (command & COMMAND_DOWN) {
    Serial.println("COMMAND DOWN");
    analogWrite(LPWM1, 0);
    analogWrite(RPWM1, motorSpeed);
    analogWrite(LPWM2, motorSpeed);
    analogWrite(RPWM2, 0);
  } else {
    Serial.println("STOP MOTORS");
    analogWrite(LPWM1, 0);
    analogWrite(RPWM1, 0);
    analogWrite(LPWM2, 0);
    analogWrite(RPWM2, 0);
  }
}
