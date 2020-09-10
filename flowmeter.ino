const int ticksPerCl = 750; // 7.5 ticks per litres per hour
volatile int flowTicks = 0;


void enableFlow() {
  Serial.print("Track flow\n");
  flowTicks = 0;
  attachInterrupt(digitalPinToInterrupt(flowPin), flowTick, CHANGE);
//  sei();
}

void flowTick() {
  flowTicks++;
    unsigned long now = millis();
}

boolean updateFlow() {
  if(flowTicks > 0) {
    unsigned long now = millis();
    unsigned long delta = now - lastSampleTime;

    unsigned long rate = calculateFlowRate(flowTicks, delta);
    unsigned long volume = calculateVolume(rate, delta);
    totalVolume += volume;
    flowRate = rate;

    if(volumeLimit > 0) {
      tempVolume += volume;
    }

    if(tempVolume >= volumeLimit) {
      closeFlow();
      tempVolume = 0;
      volumeLimit = 0;
    }
    flowTicks = 0;
    return true;
  }
  return false;
}

void turnOffAfter(unsigned int targetVolume) {
  Serial.print("Target: ");
  Serial.print(targetVolume);
  volumeLimit = 10000;
  Serial.print("\nLimit: ");
  Serial.print(volumeLimit);
  Serial.print("\n");
  tempVolume = 0;
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


void resetVolume() {
  totalVolume = 0;
}
