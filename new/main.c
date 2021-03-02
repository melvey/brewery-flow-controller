#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usart.h"

#define HOLD_COUNT 100
#define TAP_THRESHOLD 10
#define TICKS_PER_LITRE 420 // F=7Q(L/MIN). F*60=420
#define OVERFLOW_TICKS 65535
#define SERIAL_START 0x02
#define SERIAL_END 0x03
#define DEVICE_ID "F1"

unsigned int oldTicks = 0;
unsigned int newTicks = 0;
unsigned long flowRate = 0;
int updateDisplay = 0;
int updateTicks = 0;
unsigned int overflows = 0;
unsigned long volume = 0;
unsigned int flowOverflow = 0;
unsigned int pressStart = 0;
const int BUTTON_PIN = PD6;

union LongByte
{
  unsigned long value;
	char chars[4];
};

ISR(TIMER0_OVF_vect){
	//Timer 0 overflows every 16ms. Capturing 125 of these gives us a 2 s interval.
	overflows ++;
	if(overflows >= 125){
		//Get timer 1 count and record it
		overflows = 0;

		newTicks = TCNT1;
		updateTicks = 1;
	}
}

/** 
 * Calculate the current flow rate in ml
 **/
unsigned long calculateFlowRate(int ticks) {
	// This runs at 2 second intervals so ticks * 30 is ticks per minute
	// Multiply by 1000 to get ml
	return (ticks * 30000L) / TICKS_PER_LITRE;
}

/**
 * Calculate the current volume from tick count in ml
 **/
unsigned long calculateVolume(int ticks, int overflows) {
	return (((overflows * (unsigned long)OVERFLOW_TICKS) + ticks) * 1000) / TICKS_PER_LITRE;
}

int onTap(void) {
	return 1;
}

int onHold(void) {
	return 1;
}

int onRelease(void) {
	return 1;
}

int main(void) {
	//Psudo-code for setting up timer 0 as event loop timer and timer 2 as phase correct PWM
	volatile unsigned int overflows = 0;

	//Set pin-modes
	DDRB |= 0b00001010;//set PORTB so that D9 & D11 are outputs
	DDRD = 0b00001010; //set PORTD so that D1 & D3 are outputs, D0, D2, D4-D7 are inputs

	//setup Timer 0
	//Set Timer 0 to normal mode, 1/1024 prescaler
	TCCR0A = 0x00; //Set register to default value (probably not required)
	TCCR0B = 0x00; //Set register to default value (probably not required)
	TCCR0B |= (1 << CS02) | (1 << CS00); //prescaler set to 1/1024. Counter will overflow ever 16 ms

	TIMSK0 |= (1 << TOIE0); // Enable Timer 0 overflow interupt


	//setup Timer 2
	// Set Timer 2 to phase correct PWM, clear ouput on count up match, no prescaler (31kHz)
	TCCR2A = 0x00; //Set register to default value (probably not required)
	TCCR2B = 0x00; //Set register to default value (probably not required)
	TCCR2A |= (1 << WGM20); //Timer 2 set for phase corrected PWM
	TCCR2A |= (1 << COM2A1)|(1 << COM2B1); //OC2A and OC2B clear on match counting up set when counting down
	TCCR2B |= (1 << CS20); //No prescale on timer source

	// forward OCR2A = 0-255, OCR2B = 0
	// reverse OCR2A = 0, OCR2B = 0-255

	// Enable serial
	USART_Init();
	USART_Flush();

	sei(); //Enable (unmask) interupts

	while(1) {
		// Update flow volume/rate
		if(updateTicks) {
			if(newTicks != oldTicks) {
				if(newTicks > oldTicks) {
					flowRate = calculateFlowRate(newTicks - oldTicks);
				} else {
					flowRate = calculateFlowRate((newTicks + OVERFLOW_TICKS) - oldTicks);
				}
				volume = calculateVolume(newTicks, flowOverflow);
				oldTicks = newTicks;
				updateDisplay = 1;
			} else {
				flowRate = 0;
			}
			updateTicks = 0;
		}

		// Check volume
		// If close to target volume slow rate
		// If >= target volume close solenoid & updateDisplay = true

		// Control flow rate
		// if flow rate is high - close the gate
		// if flow rate is low - open the gate

		// Check button
		int buttonPress = PIND & 1<<BUTTON_PIN;
		if(buttonPress) {
			pressStart ++;
			if(pressStart > HOLD_COUNT) {
				onHold();
			}
		} else {
			if(pressStart > HOLD_COUNT) {
				onRelease();
			} else if(pressStart > TAP_THRESHOLD) {
				onTap();
			}
			pressStart = 0;
		}

		// update display
		// if updateDisplay is true update it & updateDisplay = false
		
		// Write to serial
		union LongByte flowBytes;
		flowBytes.value = flowRate;
		union LongByte volumeBytes;
		volumeBytes.value = volume;
		USART_Write(SERIAL_START);
		USART_WriteString(DEVICE_ID);
		USART_Write(flowBytes.chars[0]);
		USART_Write(flowBytes.chars[1]);
		USART_Write(flowBytes.chars[2]);
		USART_Write(flowBytes.chars[3]);
		USART_Write(volumeBytes.chars[0]);
		USART_Write(volumeBytes.chars[1]);
		USART_Write(volumeBytes.chars[2]);
		USART_Write(volumeBytes.chars[3]);
		USART_Write(SERIAL_END);

		//delay something like 100ms
		_delay_ms(100);
	}
	return 1;
}
