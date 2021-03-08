#include <Arduino.h>
#include "state.h";
#include "serial.h";
#include "display.h";
#include "flowcontroller.h"

const int SOLENOID_PIN = PINB1; // pin 4 = PB1
#define VALVE_OPEN_PIN 5
#define VALVE_CLOSE_PIN 6


void enableSolenoid() {
	DDRB = DDRB | (1<<SOLENOID_PIN); // Set solenoid pin direction register to 1 for output (pinMode OUTPUT)
}


void openFlow() {
//  if(isOpen == false) {
    Serial.print("Open flow\n");

		PORTB = PORTB | (1<<SOLENOID_PIN); // Write high value to SOLENOID PIN (digitalWrite)
		/*
    openValve();
    setOpen(true);
    sendStatus();
  Serial.print(isOpen() ? "success" : "still closed");
    showVolumeAndStatus();
		*/
//  }
}

void closeFlow() {
//  if(isOpen == true) {
    Serial.print("Close flow\n");
		PORTB = PORTB & (0<<SOLENOID_PIN); // Write low value to SOLENOID PIN (digitalWrite)
		/*
    closeValve();
    Serial.print("Closed:");
    setOpen(false);
  Serial.print(isOpen() ? "still open" : "success");
    sendStatus();
    showVolumeAndStatus();
		*/
//  }
}

void toggleFlow() {
  Serial.print("  Toggle flow  ");
  Serial.print(isOpen() ? "close it" : "open it");
  if(isOpen() == false) {
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
