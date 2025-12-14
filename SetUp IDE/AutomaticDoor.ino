#include "arduino_secrets.h"
#include "thingProperties.h"
#include <ESP32Servo.h>

// SERVO ROTATION ANGLE CONFIGURATION 
const int CLOSE_ANGLE_LEFT = 180; 
const int OPEN_ANGLE_LEFT = 0; 

const int CLOSE_ANGLE_RIGHT = 0; 
const int OPEN_ANGLE_RIGHT = 180; 

// PIN CONFIGURATION 
const int PIN_SERVO_LEFT = 12; 
const int PIN_SERVO_RIGHT = 13; 

Servo ServoLeft;
Servo ServoRight;

// TIME MANAGEMENT VARIABLES 
unsigned long startTimerLeft = 0; 
unsigned long durationLeft = 0; 
bool isTimerLeftActive = false; 
int lastMinuteLeft = -1; 

unsigned long startTimerRight = 0; 
unsigned long durationRight = 0;
bool isTimerRightActive = false; 
int lastMinuteRight = -1;

void setup() {
  Serial.begin(9600);
  delay(1500); 

  initProperties();
  ArduinoCloud.begin(ArduinoIoTPreferredConnection);

  setDebugMessageLevel(2);
  ArduinoCloud.printDebugInfo();

  // pinMode(PIN_LED, OUTPUT); // LED Pin (commented out in original)

  ServoLeft.attach(PIN_SERVO_LEFT);
  ServoRight.attach(PIN_SERVO_RIGHT);

  // Close the door on startup (reset to initial state)
  ServoLeft.write(CLOSE_ANGLE_LEFT);
  ServoRight.write(CLOSE_ANGLE_RIGHT);

  speedDoor.setSwitch(false); 
  speedDoor.setBrightness(0); 

  Serial.println("--- SYSTEM READY ---");
}

void loop() {
  ArduinoCloud.update();
  unsigned long currentMillis = millis();

  // LEFT DOOR LOGIC 
  if (isTimerLeftActive) {
    unsigned long elapsed = currentMillis - startTimerLeft;
    if (elapsed >= durationLeft) {
      Serial.println("[AUTO] LeftDoor timer expired -> Closing door!");
      closeDoorLeft(); 
      isTimerLeftActive = false;
      leftDoor.setSwitch(false); leftDoor.setBrightness(0); 
    } else {
      long remainingTime = durationLeft - elapsed;
      int remainingMinutes = (remainingTime / 60000) + 1; 
      if (remainingMinutes != lastMinuteLeft) {
        lastMinuteLeft = remainingMinutes;
        leftDoor.setBrightness(remainingMinutes);
      }
    }
  }

  // RIGHT DOOR LOGIC 
  if (isTimerRightActive) {
    unsigned long elapsed = currentMillis - startTimerRight;
    if (elapsed >= durationRight) {
      Serial.println("[AUTO] RightDoor timer expired -> Closing door!");
      closeDoorRight(); 
      isTimerRightActive = false;
      rightDoor.setSwitch(false); rightDoor.setBrightness(0);
    } else {
      long remainingTime = durationRight - elapsed;
      int remainingMinutes = (remainingTime / 60000) + 1; 
      if (remainingMinutes != lastMinuteRight) {
        lastMinuteRight = remainingMinutes;
        rightDoor.setBrightness(remainingMinutes);
      }
    }
  }
}

// DELAY CALCULATION FUNCTION 
int getSpeedDelay() {
  if (speedDoor.getSwitch() == false) return 0;

  float val = speedDoor.getBrightness();

  if (val <= 0.1) return 0;

  if (val >= 100) return 0;

  return map((int)val, 1, 99, 50, 1);
}

// MOVEMENT FUNCTIONS 

void openDoorLeft() {
  int d = getSpeedDelay();
  if (d == 0) {
    // Fast Mode: Write directly -> 2 doors run simultaneously
    ServoLeft.write(OPEN_ANGLE_LEFT);
  } else {
    // Slow Mode: Run loop (Left: 180 -> 0)
    for (int pos = CLOSE_ANGLE_LEFT; pos >= OPEN_ANGLE_LEFT; pos--) {
      ServoLeft.write(pos); delay(d);
    }
  }
}

void closeDoorLeft() {
  int d = getSpeedDelay();
  if (d == 0) {
    ServoLeft.write(CLOSE_ANGLE_LEFT);
  } else {
    // Slow Mode (Left: 0 -> 180)
    for (int pos = OPEN_ANGLE_LEFT; pos <= CLOSE_ANGLE_LEFT; pos++) {
      ServoLeft.write(pos); delay(d);
    }
  }
}

void openDoorRight() {
  int d = getSpeedDelay();
  if (d == 0) {
    ServoRight.write(OPEN_ANGLE_RIGHT);
  } else {
    // Slow Mode (Right: 0 -> 180)
    for (int pos = CLOSE_ANGLE_RIGHT; pos <= OPEN_ANGLE_RIGHT; pos++) {
      ServoRight.write(pos); delay(d);
    }
  }
}

void closeDoorRight() {
  int d = getSpeedDelay();
  if (d == 0) {
    ServoRight.write(CLOSE_ANGLE_RIGHT);
  } else {
    // Slow Mode (Right: 180 -> 0)
    for (int pos = OPEN_ANGLE_RIGHT; pos >= CLOSE_ANGLE_RIGHT; pos--) {
      ServoRight.write(pos); delay(d);
    }
  }
}

// CALLBACKS 

void onLeftDoorChange() {
  if (leftDoor.getSwitch()) {
    Serial.println("[LEFT] OPEN");
    openDoorLeft(); 

    // Timer Logic
    float timer = leftDoor.getBrightness();
    if (timer > 0) {
      int minutes = (int)timer;
      durationLeft = minutes * 60000; 
      startTimerLeft = millis();
      isTimerLeftActive = true;
      lastMinuteLeft = minutes;
      Serial.print(" -> Timer (minutes): "); Serial.println(minutes);
    } else { isTimerLeftActive = false; }
  } else {
    Serial.println("[LEFT] CLOSE");
    closeDoorLeft();
    isTimerLeftActive = false;
  }
}

void onRightDoorChange() {
  if (rightDoor.getSwitch()) {
    Serial.println("[RIGHT] OPEN");
    openDoorRight(); 

    float timer = rightDoor.getBrightness();
    if (timer > 0) {
      int minutes = (int)timer;
      durationRight = minutes * 60000;
      startTimerRight = millis();
      isTimerRightActive = true;
      lastMinuteRight = minutes;
      Serial.print(" -> Timer (minutes): "); Serial.println(minutes);
    } else { isTimerRightActive = false; }
  } else {
    Serial.println("[RIGHT] CLOSE");
    closeDoorRight();
    isTimerRightActive = false;
  }
}

void onSpeedDoorChange() {
  if (speedDoor.getSwitch()) {
    Serial.print("Speed ON: "); Serial.println(speedDoor.getBrightness());
  } else {
    Serial.println("Speed OFF");
  }
}