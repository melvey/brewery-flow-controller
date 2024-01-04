#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usart.h"
#include "ioFunctions.h"
#include "flow.h"
#include "lcd.h"

#define ENABLE_LCD 1

#define HOLD_COUNT 100
#define TAP_THRESHOLD 10
#define TIMER0_OVERFLOW_COUNT 125
#define INCREASE_VOLUME 1000
#define DEBOUNCE_DELAY 3 // Debounce delay in timer 0 overflows (16ms intervals)
const int BUTTON_PIN = PD6;

int updateDisplay = 0;
int updateTicks = 0;
int newTicks = 0;
unsigned int overflows = 0;
unsigned int pressStart = 0;
unsigned long lastDebounceTime = 0;
uint8_t lastButtonState = 0;
uint8_t buttonState = 0;
const char SERIAL_DATA_CMD = 'D';

union LongByte
{
    unsigned long value;
    char chars[4];
};

// Timer0 Overflow ISR
// TODO: Handle tick overflow
ISR(TIMER0_OVF_vect){
	//Timer 0 overflows every 16ms. Capturing 125 of these gives us a 2 s interval.
    overflows++;
    if(overflows >= TIMER0_OVERFLOW_COUNT){
		//Get timer 1 count and record it
        overflows = 0;
        newTicks = TCNT1;
        updateTicks = 1;
    }
}

void configureTimer0(void) {
    //Set Timer 0 to normal mode, 1/1024 prescaler
	TCCR0A = 0x00; //Set register to default value (probably not required)
	TCCR0B = 0x00; //Set register to default value (probably not required)
	TCCR0B |= (1 << CS02) | (1 << CS00); //prescaler set to 1/1024. Counter will overflow ever 16 ms

	TIMSK0 |= (1 << TOIE0); // Enable Timer 0 overflow interupt
}

void configureTimer1(void) {
	// Setup flow controller counter 
	// init counter to count up and increment on external pin T1 rising edge
	TCCR1A = 0x00;
	TCCR1B = 0x07; // 0x00 | (1<<CS12) | (1<<CS11) | (1<<CS10)
	// Zero counter
	TCNT1 = 0x0000;
}

void configureTimer2(void) {
    // Set Timer 2 to phase correct PWM, clear ouput on count up match, no prescaler (31kHz)
	TCCR2A = 0x00; //Set register to default value (probably not required)
	TCCR2B = 0x00; //Set register to default value (probably not required)
	TCCR2A |= (1 << WGM20); //Timer 2 set for phase corrected PWM
	TCCR2A |= (1 << COM2A1)|(1 << COM2B1); //OC2A and OC2B clear on match counting up set when counting down
	TCCR2B |= (1 << CS20); //No prescale on timer source
}

void setupPinModes(void) {
	DDRB |= 0b00001010;//set PORTB so that D9 & D11 are outputs
	DDRD = 0b00001010; //set PORTD so that D1 & D3 are outputs, D0, D2, D4-D7 are inputs
}


void serialUpdate(void) {
	// Write to serial
	union LongByte flowBytes;
	flowBytes.value = getFlowRate();
	union LongByte volumeBytes;
	volumeBytes.value = getVolume();
	union LongByte tickBytes;
	tickBytes.value = newTicks;
	union LongByte targetBytes;
	targetBytes.value = getTargetVolume();
	union LongByte status;
	status.value = getFlowStatus();

	int msgLength = 20;
	char message[msgLength];
	//message[3] = SERIAL_DATA_CMD;
	memcpy(&message[0], &flowBytes.chars, 4);
	memcpy(&message[4], &volumeBytes.chars, 4);
	memcpy(&message[8], &tickBytes.chars, 4);
	memcpy(&message[12], &targetBytes.chars, 4);
	memcpy(&message[16], &status.chars, 4);

	sendCommand(SERIAL_DATA_CMD, msgLength, message);
}



int main(void) {
    setupPinModes();
    configureTimer0();
    configureTimer1();
    configureTimer2();

    initializeSerial();

    sei();

    openFlow();

    sendMessage("Starting");
    setupLCD();
    serialUpdate();
    while(1) {
        readInput();

		if(updateTicks) {
			updateTicks = 0;
			updateVolume(newTicks);
            serialUpdate();
            displayVolume(getVolume(), getFlowRate());

        }

        _delay_ms(100);
    }
    return 1;
}
