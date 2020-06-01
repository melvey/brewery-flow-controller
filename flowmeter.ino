const float ticksPerLitres = 7.5; // 7.5 ticks per litres per hour
volatile int flowTicks = 0;


void enableFlow() {
  Serial.print("Track flow\n");
  flowTicks = 0;
  attachInterrupt(digitalPinToInterrupt(flowPin), flowTick, CHANGE);
//  sei();
}

void flowTick() {
  flowTicks++;
    long now = millis();
}

boolean updateFlow() {
  if(flowTicks > 0) {
    long now = millis();
    long delta = now - lastSampleTime;
    float secondsPassed = (float)delta / (float)1000; // Get delta in seconds

    double rate = calculateFlowRate(flowTicks, secondsPassed);
    double volume = calculateVolume(rate, secondsPassed);
    totalVolume += volume;

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

void turnOffAfter(long targetVolume) {
  volumeLimit = targetVolume;
  tempVolume = 0;
}

/**
 * Calculate the flow rate for a number of ticks
 * @param {long} ticks The number of ticks that have been recorded
 * @param {double} seconds The number of seconds the ticks were recorded in
 * @return {double} The flow rate in L/h
 */
double calculateFlowRate(long ticks, double seconds) {
    double flow = (ticks * 60.0 / ticksPerLitres) / seconds;
    return flow;  
}

/**
 * Calculate the volume of liquid from a flow rate
 * @param {double} flowRate the flow rate in L/h
 * @param {float} The number of seconds that have passed
 */
double calculateVolume(double flowRate, float seconds) {
    double volume = (flowRate / 3600.0) * seconds * 1000; // Convert litres to mililitres
    return volume;
}


void resetVolume() {
  totalVolume = 0;
}
