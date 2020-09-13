#include <Arduino.h>
#include "flowcontroller.h";

const float holdTime = 500;

const int buttonPin = 3;

int holding = 0;
float pressStart = 0;
float lastPress = 0;

void setupButton() {
  pinMode(buttonPin, INPUT);
}

void onHold() {
//  Serial.print("Holding");
  openFlow();
  openValve();
}

void onRelease() {
//  Serial.print("Released");
  closeFlow();
  closeValve();
}

void onTap() {
//  turnOffAfter(50000);
  toggleFlow();
}


void handleButtonPress() {
  int buttonState = digitalRead(buttonPin);
  float now = millis();
  if(buttonState == LOW) {
    float now = millis();
    
    if(pressStart == 0) {
      pressStart = now;
    }
    
    if(now - pressStart > holdTime) {
//      Serial.print("Holding:");
//      Serial.print(now - pressStart);
 //     Serial.print("\n");
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
