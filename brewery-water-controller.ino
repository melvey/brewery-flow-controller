#include <Arduino.h>
#include "state.h"
#include "serial.h"
#include "display.h"
#include "flowmeter.h";
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
  Serial.begin(115200);
  Serial.print("Starting ");
  Serial.print(random(100));
  Serial.print("\n");
  setupLCD();
  Serial.print("LCD setup \n");
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
