#include <Arduino.h>
#include "state.h";
#include "serial.h";
#include "display.h";

const int solenoidPin = 4;
const int valveOpenPin = 5;
const int valveClosePin = 6;


void enableSolenoid() {
  pinMode(solenoidPin, OUTPUT);
}


void openFlow() {
  if(isOpen == false) {
//    Serial.print("Open flow\n");
    digitalWrite(solenoidPin, HIGH);
    setOpen(true);
    sendStatus();
    showVolumeAndStatus();
  }
}

void closeFlow() {
  if(isOpen == true) {
//    Serial.print("Close flow\n");
    digitalWrite(solenoidPin, LOW);
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
  analogWrite(valveOpenPin, 0);
  analogWrite(valveClosePin, 255);
}

void closeValve() {
  analogWrite(valveClosePin, 0);
  analogWrite(valveOpenPin, 255);
}
