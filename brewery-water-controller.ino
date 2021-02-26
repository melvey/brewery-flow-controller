#include <Arduino.h>
#include "lib/state.h"
#include "lib/serial.h"
#include "lib/display.h"
#include "lib/flowmeter.cpp";
#include "lib/flowcontroller.cpp";
//#include "button.h";


const int updateInterval = 1000;

// Throttling for flow samples
const int sampleInterval = 300;
// Throttling for serial reads



float oldTime = 0;
float loopRunTime = 0;


void setup() {
  enableFlow();
  Serial.begin(115200);
  Serial.println("Starting ");
  //setupLCD();
  Serial.print("LCD setup \n");
  enableSolenoid();
/*
  // put your setup code here, to run once:
  randomSeed(analogRead(0));
  Serial.begin(115200);
  Serial.print("Starting ");
  Serial.print(random(100));
  Serial.print("\n");
  setupButton();
  loopRunTime = millis(); // A bit moot but it keeps our initial delta accurate
  enableFlow();
  enableSolenoid();
  sendStatus();
  openFlow();
  
  showVolumeAndStatus();
	*/
}


void loop() {
	Serial.println(getTicks());
	delay(1000);
	openFlow();
	delay(1000);
	closeFlow();

/*
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
	*/
}
