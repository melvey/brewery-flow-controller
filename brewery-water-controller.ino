#include <Arduino.h>

#include "serial.h"
#include "state.h"
#include "flowmeter.h";
#include "display.h"
#include "button.h";

const int updateInterval = 1000;

// Throttling for flow samples
const int sampleInterval = 300;
// Throttling for serial reads



float oldTime = 0;
float loopRunTime = 0;

void setup() {
  // put your setup code here, to run once:
  randomSeed(analogRead(0));
  Serial.begin(9600);
  setupLCD();
  Serial.print("Starting ");
  Serial.print(random(100));
  Serial.print("\n");

  setupButton();
  loopRunTime = millis(); // A bit moot but it keeps our initial delta accurate
  enableFlow();
  sendStatus();
  showVolumeAndStatus();

}

void loop() {
   float oldTime = loopRunTime;
   loopRunTime = millis();
   float deltaTime = loopRunTime - oldTime;
   if(deltaTime > 100) {
   }
   handleButtonPress();

   if(loopRunTime - getLastSampleTime() > sampleInterval) {
    // Update flow status
    if(updateFlow()) {
      long startT = millis();
			sendStatus();
  
      if(getLimit() > 0) {
        showProgressAndFlow();
      } else {
        showVolumeAndFlow();    
      }
      setLastSampleTime(loopRunTime);

      
       long endT = millis();
    } else {
      readSerialThrottled();
    }
  }
}
