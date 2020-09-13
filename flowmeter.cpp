#include <Arduino.h>
#include "state.h"
#include "flowcontroller.h";


const int flowPin = 2; // Should be define
const int ticksPerCl = 750; // 7.5 ticks per litres per hour
volatile int flowTicks = 0;


void flowTick() {
  flowTicks++;
    unsigned long now = millis();
}

void enableFlow() {
  Serial.print("Track flow\n");
  pinMode(flowPin, INPUT);
  flowTicks = 0;
  attachInterrupt(digitalPinToInterrupt(flowPin), flowTick, CHANGE);
//  sei();
}


/**
 * Calculate the flow rate for a number of ticks
 * @param {long} ticks The number of ticks that have been recorded
 * @param {unsigned } delta The number of miliseconds the ticks were recorded in
 * @return {unsigned long} The flow rate in mL/h
 */
unsigned long calculateFlowRate(long ticks, unsigned long delta) {  
    unsigned long unitConversion = 100000000; // there are 100000 ml in a cl and 1000ms in a second
    unsigned long flow = (ticks * unitConversion / ticksPerCl) / delta;
    return flow;  
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
  if(flowTicks > 0) {
    unsigned long now = millis();
    unsigned long delta = now - getLastSampleTime();

    unsigned long rate = calculateFlowRate(flowTicks, delta);
    unsigned long volume = calculateVolume(rate, delta);
    addVolume(volume);
    setRate(rate);

    if(getLimit() > 0) {
      addVolume(volume);
    }

    if(getCurrentVolume() >= getLimit()) {
      closeFlow();
      resetVolume();
      //setLimit(0);
    }
    flowTicks = 0;
    return true;
  }
  return false;
}

void turnOffAfter(unsigned int targetVolume) {
  Serial.print("Target: ");
  Serial.print(targetVolume);
  setLimit(10000);
  Serial.print("\nLimit: ");
  Serial.print(getLimit());
  Serial.print("\n");
  resetVolume();
}
