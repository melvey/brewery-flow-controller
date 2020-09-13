
static bool open = false;  // Flag indicating if the solenoid (flow) is open
static unsigned long totalVolume = 0; // The total volume of liquid in microlitres that has passed
static unsigned long flowRate = 0;        // Smoothed flow rate in ml per hour
static unsigned int volumeLimit = 50000;  // Limit in ml of volume that will flow before it is closed
static unsigned long currentVolume = 0;  // The volume of water that has flowed when tracking the volumeLimit
static unsigned long lastSampleTime = 0; // The last time the volume was sampled


bool isOpen() {
	return open;
}

unsigned long getTotalVolume() {
	return totalVolume;
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

bool pastLimit() {
	return currentVolume >= volumeLimit;
}

unsigned long getLastSampleTime() {
  return lastSampleTime;
}

void setOpen(bool valveOpen) {
	open = valveOpen;
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

void setLimit(unsigned int limit) {
	volumeLimit = limit;
}

void setLastSampleTime(unsigned long sampleTime) {
  lastSampleTime = sampleTime;
}
