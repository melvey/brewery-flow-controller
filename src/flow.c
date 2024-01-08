#include "flow.h"
#include "ioFunctions.h"


/*
const int VALVE_OPEN_REGISTER = OCR2B; // D3
const int VALVE_CLOSE_REGISTER = OCR2A; // D11 PWA
*/

const int SOLENOID_PIN = PINB1; // D9
unsigned int ticks = 0;
unsigned long flowRate = 0;
unsigned long volume = 0;
unsigned int targetVolume = 1500;
unsigned int flowOverflow = 0;
int flowStatus = 0;

void openFlow(void) {
	flowStatus = 1;
	PORTB = PORTB | (1<<SOLENOID_PIN); // Write high value to SOLENOID PIN (digitalWrite)

    OCR2B = 0;
    OCR2A = 128;
}

void closeFlow(void) {
	flowStatus = 0;
	PORTB = PORTB & (0<<SOLENOID_PIN); // Write low value to SOLENOID PIN (digitalWrite)

    OCR2B = 128;
    OCR2A = 0;
}


/** 
 * Calculate the current flow rate in ml
 * It would be nicer if this took a delta
 **/
unsigned long calculateFlowRate(int ticks) {
	// This runs at 2 second intervals so ticks * 30 is ticks per minute
	// Multiply by 1000 to get ml
	return (ticks * 30000L) / TICKS_PER_LITRE;
}

/**
 * Calculate the current volume from tick count in ml
 **/
unsigned long calculateVolume(int ticks, int volOverflows) {
	return (((volOverflows * (unsigned long)OVERFLOW_TICKS) + ticks) * 1000) / TICKS_PER_LITRE;
}

// This would benefit from some smoothing
void updateVolume(int newTicks) {
    if(newTicks != ticks) {
        if(newTicks > ticks) {
            flowRate = calculateFlowRate(newTicks - ticks);
        } else {
            flowRate = calculateFlowRate((newTicks + OVERFLOW_TICKS) - ticks);
        }
        volume = calculateVolume(newTicks, flowOverflow);
        ticks = newTicks;

        if(targetVolume > 0 && volume >= targetVolume) {
            closeFlow();
        } else if(targetVolume == 0 || volume < targetVolume) {
            openFlow();
        }
    } else {
        flowRate = 0;
    }
}

unsigned long getFlowRate(void) {
    return flowRate;
}

unsigned long getVolume(void) {
    return volume;
}

unsigned long getFlowStatus(void) {
    return flowStatus;
}

void setTargetVolume(unsigned int newVolume) {
    targetVolume = newVolume;
}

unsigned int getTargetVolume(void) {
    return targetVolume;
}

