#include <Arduino.h>
#include "state.h";
#include "serial.h";
#include "display.h";

#define SOLENOID_PIN 4
#define VALVE_OPEN_PIN 5
#define VALVE_CLOSE_PIN 6


void enableSolenoid() {
  pinMode(SOLENOID_PIN, OUTPUT);
}


void openFlow() {
  if(isOpen == false) {
//    Serial.print("Open flow\n");
    digitalWrite(SOLENOID_PIN, HIGH);
    setOpen(true);
    sendStatus();
    showVolumeAndStatus();
  }
}

void closeFlow() {
  if(isOpen == true) {
//    Serial.print("Close flow\n");
    digitalWrite(SOLENOID_PIN, LOW);
    setOpen(false);
    sendStatus();
    showVolumeAndStatus();
  }
}

void toggleFlow() {
  if(isOpen == false) {
    openFlow();
  } else {
    closeFlow();
  }
}


void openValve() {
  analogWrite(VALVE_OPEN_PIN, 0);
  analogWrite(VALVE_CLOSE_PIN, 255);
}

void closeValve() {
  analogWrite(VALVE_CLOSE_PIN, 0);
  analogWrite(VALVE_OPEN_PIN, 255);
}
