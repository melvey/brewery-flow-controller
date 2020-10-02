#include <Arduino.h>
#include "flowcontroller.h";

#define BUTTON_PIN  3
#define HOLD_TIME   500

int holding = 0;
unsigned long pressStart = 0;
unsigned long lastPress = 0;

void setupButton() {
  pinMode(BUTTON_PIN, INPUT);
}

void startHold() {
  Serial.print("Start hold");
  openFlow();
  openValve();  
}

void onHold() {
//  Serial.print("Holding");
  openFlow();
  openValve();
}

void onRelease() {
  Serial.print("Release");
  closeFlow();
  closeValve();
}

void onTap() {
  Serial.print("Tap");
//  turnOffAfter(50000);
  toggleFlow();
}


void handleButtonPress() {
  int buttonState = digitalRead(BUTTON_PIN );
  unsigned long now = millis();
  if(buttonState == LOW) {
    
    if(pressStart == 0) {
      pressStart = now;
    }
    
    if(now - pressStart > HOLD_TIME) {
      holding = 1;
      onHold();
    }
  } else if(pressStart != 0) {
    pressStart = 0;
    if(holding) {
      holding = 0;
      onRelease();
    } else {
      onTap();
    }
  }
}
