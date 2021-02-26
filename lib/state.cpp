
static bool valveIsOpen = false;  // Flag indicating if the solenoid (flow) is open
static unsigned long totalVolume = 0; // The total volume of liquid in microlitres that has passed
static unsigned long flowRate = 0;        // Smoothed flow rate in ml per hour
static unsigned long volumeLimit = 500000;  // Limit in ml of volume that will flow before it is closed (3l for some reason)
static unsigned long currentVolume = 0;  // The volume of water that has flowed when tracking the volumeLimit in microlitres
static unsigned long lastSampleTime = 0; // The last time the volume was sampled
static unsigned long flowTicks = 0;		// The number of ticks the have passed (to replace volume measurements)


bool isOpen() {
	return valveIsOpen;
}

unsigned long getTotalVolume() {
	return totalVolume;
}

unsigned long getTotalVolumeInMl() {
	return totalVolume / 1000;
}

unsigned long getRate() {
	return flowRate;
}

unsigned long getLimit() {
	return volumeLimit;
}

unsigned long getCurrentVolume() {
	return currentVolume;
}

unsigned long getCurrentVolumeInMl() {
	return currentVolume / 1000;
}

bool pastLimit() {
	return currentVolume / 1000 >= volumeLimit;
}

unsigned long getLastSampleTime() {
  return lastSampleTime;
}

void setOpen(bool valveOpen) {
	valveIsOpen = valveOpen;
}

void setRate(unsigned long rate) {
	flowRate = rate;
}

void addVolume(unsigned long volume) {
	currentVolume += volume;
	totalVolume += volume;
}

void resetVolume() {
	currentVolume = 0;
}

void resetTotalVolume() {
	totalVolume = 0;
	resetVolume();
}

void setLimit(unsigned long limit) {
	volumeLimit = limit;
}

void setLastSampleTime(unsigned long sampleTime) {
  lastSampleTime = sampleTime;
}

void addTick() {
	flowTicks++;
}
