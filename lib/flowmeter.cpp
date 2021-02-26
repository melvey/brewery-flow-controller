#include <Arduino.h>
#include "state.h"
#include "flowcontroller.h";


const int counterPin = 4;
const int ticksPerl = 420; // F=7Q(L/MIN). F*60=420
unsigned int overflowCount = 0;
unsigned int lastReadTime = 0;
unsigned int lastReadTicks = 0;
unsigned long countTicksFrom = 0;

unsigned long getTicks() {
	unsigned int tickCount = TCNT1;
	return (long)tickCount + ((long)overflowCount * 65535);
}

void enableFlow() {
  Serial.print("Track flow\n");
	DDRD = DDRD & (0<<counterPin); // Set counter pin direction register to 0 (pinMode INPUT)
	// init counter to count up and increment on external pin T1 rising edge
	TCCR1A = 0x00;
	TCCR1B = 0x07; // 0x00 | (1<<CS12) | (1<<CS11) | (1<<CS10)
	
	// Zero counter
	TCNT1 = 0x0000;
	
	// Add overflow interrupt
  //attachInterrupt(digitalPinToInterrupt(flowPin), flowTick, CHANGE);
//  sei();
}


/**
 * Calculate the flow rate for a number of ticks
 * @param {long} ticks The number of ticks that have been recorded
 * @param {unsigned } delta The number of miliseconds the ticks were recorded in
 * @return {unsigned long} The flow rate in mL/h
 */
unsigned long calculateFlowRate(long ticks, unsigned long delta) {  
	return 0;
}

/**
 * Calculate the volume of liquid from a flow rate
 * @param {unsigned long} flowRate the flow rate in mL/h
 * @param {unisnged long} delta The number of miliseconds that have passed
 * @param {unsgined long} The volume in ml
 */
unsigned long calculateVolume(unsigned long flowRate, unsigned long delta) {
    Serial.print("FPMS:");
    Serial.print(flowRate);
    unsigned long volume = flowRate * delta ; // Convert litres to mililitres
    return volume;
}

boolean updateFlow() {
  if(getTicks() > 0) {
    unsigned long now = millis();
    unsigned long delta = now - getLastSampleTime();

    unsigned long rate = calculateFlowRate(getTicks(), delta);
    unsigned long volume = calculateVolume(rate, delta);
    addVolume(volume);
    setRate(rate);

    if(getLimit() > 0) {
      addVolume(volume);
    }

    if(getLimit() > 0 &&  pastLimit()) {
      Serial.print("volume passed: ");
      Serial.print(getCurrentVolumeInMl());
      Serial.print(" >= ");
      Serial.print(getLimit());
      closeFlow();
      resetVolume();
      //setLimit(0);
    }
    return true;
  }
  return false;
}

void turnOffAfter(unsigned long targetVolume) {
  Serial.print("Target: ");
  Serial.print(targetVolume);
  long setLimit(targetVolume);
  Serial.print("\nLimit: ");
  Serial.print(getLimit());
  Serial.print("\n");
  resetVolume();
}
