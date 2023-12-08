
//Psudo-code for setting up timer 0 as event loop timer and timer 2 as phase correct PWM

#include <avr/io.h>
#include <avr/interrupt.h>

volatile unsigned int overflows = 0;

//Set pin-modes
DDRB |= 0b00001010;//set PORTB so that D9 & D11 are outputs
DDRD = 0b00001010; //set PORTD so that D1 & D3 are outputs, D0, D2, D4-D7 are inputs

//setup Timer 0
//Set Timer 0 to normal mode, 1/1024 prescaler
TCCR0A = 0x00; //Set register to default value (probably not required)
TCCR0B = 0x00; //Set register to default value (probably not required)
TCCR0B |= (1 << CS02) | (1 << CS00); //prescaler set to 1/1024. Counter will overflow ever 16 ms

TIMSK0 |= (1 << TIOE0) // Enable Timer 0 overflow interupt

sei(); //Enable (unmask) interupts

//setup Timer 2
// Set Timer 2 to phase correct PWM, clear ouput on count up match, no prescaler (31kHz)
TCCR2A = 0x00; //Set register to default value (probably not required)
TCCR2B = 0x00; //Set register to default value (probably not required)
TCCR2A |= (1 << WGM20); //Timer 2 set for phase corrected PWM
TCCR2A |= (1 << COM2A1)|(1 << COM2B1); //OC2A and OC2B clear on match counting up set when counting down
TCCR2B |= (1 << CS20); //No prescale on timer source

// forward OCR2A = 0-255, OCR2B = 0
// reverse OCR2A = 0, OCR2B = 0-255

unsigned int tickCount = 0;
ISR(TIMER0_OVF_vect){
	//Timer 0 overflows every 16ms. Capturing 125 of these gives us a 2 s interval.
	overflows ++
	if(overflows >= 125){
		//Get timer 1 count and record it
		overflows = 0;

		tickCount = TCNT1;
	}
}
