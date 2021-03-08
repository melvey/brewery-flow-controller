#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usart.h"
#include "lcdpcf8574.h"

#define HOLD_COUNT 100
#define TAP_THRESHOLD 10
#define TICKS_PER_LITRE 420 // F=7Q(L/MIN). F*60=420
#define OVERFLOW_TICKS 65535

const int BUTTON_PIN = PD6;
const int SOLENOID_PIN = PINB1; // pin 4 = PB1
const char DEVICE_ID[2] = "F1";
const char SERIAL_DATA_CMD = 'D';
const char SERIAL_START = 0x02;
const char SERIAL_END = 0x03;

unsigned int oldTicks = 0;
unsigned int newTicks = 0;
unsigned long flowRate = 0;
int updateDisplay = 0;
int updateTicks = 0;
int flowStatus = 0;
unsigned int overflows = 0;
unsigned long volume = 0;
unsigned int flowOverflow = 0;
unsigned int pressStart = 0;
unsigned int targetVolume = 1000;

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
unsigned long calculateVolume(int ticks, int volOverflows) {
	return (((volOverflows * (unsigned long)OVERFLOW_TICKS) + ticks) * 1000) / TICKS_PER_LITRE;
}

void onTap(void) {
	// If we are chasing a target volume increase that
	// Otherwise set a target volume above the current
	int increaseVolume = 1000;
	if(targetVolume > volume) {
		targetVolume += increaseVolume;
	} else {
		targetVolume = volume + increaseVolume;
	}
	return;
}

void onHold(void) {
	targetVolume = 0;
	return;
}

void onRelease(void) {
	targetVolume = volume;
	return;
}

void openFlow(void) {
	flowStatus = 1;
	PORTB = PORTB | (1<<SOLENOID_PIN); // Write high value to SOLENOID PIN (digitalWrite)
}

void closeFlow(void) {
	flowStatus = 0;
	PORTB = PORTB & (0<<SOLENOID_PIN); // Write low value to SOLENOID PIN (digitalWrite)
}

int main(void) {
	//setting up timer 0 as event loop timer and timer 2 as phase correct PWM

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

	// Setup counter 
	// init counter to count up and increment on external pin T1 rising edge
	TCCR1A = 0x00;
	TCCR1B = 0x07; // 0x00 | (1<<CS12) | (1<<CS11) | (1<<CS10)
	// Zero counter
	TCNT1 = 0x0000;

	// forward OCR2A = 0-255, OCR2B = 0
	// reverse OCR2A = 0, OCR2B = 0-255

	// Enable serial
	USART_Init();
	USART_Flush();

	sei(); //Enable (unmask) interupts

	openFlow();

	char tmp = LCD_DISP_ON_BLINK;
	lcd_init(LCD_DISP_ON_BLINK);

	//lcd go home
	lcd_home();

	lcd_led(0); //turn on LED
	int line = 0;
	lcd_gotoxy(1, line);
	lcd_puts("Starting");

	while(1) {

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

		// Update flow volume/rate
		if(updateTicks) {
			updateTicks = 0;

			if(newTicks != oldTicks) {
				if(newTicks > oldTicks) {
					flowRate = calculateFlowRate(newTicks - oldTicks);
				} else {
					flowRate = calculateFlowRate((newTicks + OVERFLOW_TICKS) - oldTicks);
				}
				volume = calculateVolume(newTicks, flowOverflow);
				oldTicks = newTicks;

				if(targetVolume > 0 && volume >= targetVolume) {
					closeFlow();
				} else if(targetVolume == 0 || volume < targetVolume) {
					openFlow();
				}

				updateDisplay = 1;
			} else {
				flowRate = 0;
			}

			// Check volume
			// If close to target volume slow rate
			// If >= target volume close solenoid & updateDisplay = true

			// Control flow rate
			// if flow rate is high - close the gate
			// if flow rate is low - open the gate


			// update display
			// if updateDisplay is true update it & updateDisplay = false
			
			// Write to serial
			union LongByte flowBytes;
			flowBytes.value = flowRate;
			union LongByte volumeBytes;
			volumeBytes.value = volume;
			union LongByte tickBytes;
			tickBytes.value = newTicks;
			union LongByte targetBytes;
			targetBytes.value = targetVolume;
			union LongByte status;
			status.value = flowStatus;

			int msgLength = 25;
			char message[msgLength];
			message[0] = SERIAL_START;
			memcpy(&message[1], &DEVICE_ID, 2);
			message[3] = SERIAL_DATA_CMD;
			memcpy(&message[4], &flowBytes.chars, 4);
			memcpy(&message[8], &volumeBytes.chars, 4);
			memcpy(&message[12], &tickBytes.chars, 4);
			memcpy(&message[16], &targetBytes.chars, 4);
			memcpy(&message[20], &status.chars, 4);
			message[24] = SERIAL_END;
			for(int i = 0; i < msgLength; i++) {
				USART_Write(message[i]);
			}

		}


		//delay something like 100ms
		_delay_ms(100);
	}
	return 1;
}
