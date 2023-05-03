#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usart.h"
#include "lcdpcf8574.h"

#define ENABLE_LCD 1

#define HOLD_COUNT 100
#define TAP_THRESHOLD 10
#define TICKS_PER_LITRE 356
#define OVERFLOW_TICKS 65535
#define SERIAL_BUFFER_SIZE 15
#define SERIAL_DEVICE_INDEX 2
#define SERIAL_DEVICE_LENGTH 2
#define SERIAL_CMD_INDEX 3
#define TIMER0_OVERFLOW_COUNT 125
#define INCREASE_VOLUME 1000
#define DEBOUNCE_DELAY 3 // Debounce delay in timer 0 overflows (16ms intervals)
const int BUTTON_PIN = PD6;
const int SOLENOID_PIN = PINB1;
const char DEVICE_ID[2] = "F1";
const char SERIAL_DATA_CMD = 'D';
const char SERIAL_MSG_CMD = 'M';
const char SERIAL_SET_TARGET_CMD = 'T';
const int SERIAL_SET_TARGET_CMD_LENGTH = 9;
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
unsigned int targetVolume = 1500;
char serialBuffer[SERIAL_BUFFER_SIZE];
unsigned short int bufferIndex = 0;
unsigned long lastDebounceTime = 0;
uint8_t lastButtonState = 0;
uint8_t buttonState = 0;

// A debugging buffer to check which data is being read from serial
// This can be deleted after debugging as the key information is in serialBuffer
char tmpBuffer[SERIAL_BUFFER_SIZE];
int tmpBufferIndex = 0;


void toHexString(char *dest, const char *src, size_t len) {
    for (size_t i = 0; i < len; i++) {
        snprintf(dest + i * 2, 3, "%02X", (uint8_t)src[i]);
    }
}

void sendMessage(const char *message) {
    uint8_t messageLength = strlen(message);
    uint8_t totalChunks = (messageLength + 19) / 20; // Calculate the number of chunks required to send the message

    for (uint8_t chunk = 0; chunk < totalChunks; chunk++) {
        USART_Write(SERIAL_START);
        USART_Write(DEVICE_ID[0]);
        USART_Write(DEVICE_ID[1]);
        USART_Write(SERIAL_MSG_CMD);

        for (uint8_t i = 0; i < 20; i++) {
            uint8_t index = chunk * 20 + i;
            if (index < messageLength) {
                USART_Write(message[index]);
            } else {
                USART_Write(' '); // Fill with spaces if there is no more data
            }
        }

        USART_Write(SERIAL_END);
    }
}

union LongByte
{
    unsigned long value;
    char chars[4];
};

// Timer0 Overflow ISR
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

void readInput(void) {
    // track status of read to avoid writing while reading
    // This should later be split off to be the return value of a processCommand function
    int status = 0;
    tmpBufferIndex = 0;
    char readChar;
    while (USART_Ready() == 1) {
        readChar = USART_Receive();


        if(tmpBufferIndex < SERIAL_BUFFER_SIZE) {
            tmpBuffer[tmpBufferIndex] = readChar;
            tmpBufferIndex++;
        } else {
            tmpBufferIndex = 0;
        }


        if (readChar == SERIAL_START) {
            serialBuffer[0] = readChar;
            bufferIndex = 1;
        } else if (readChar == SERIAL_END) {
                // Check for ETX character
                // If the device ID matches and the command and length are correct, update the target volume
                status = 1;

                if (strncmp(serialBuffer + SERIAL_DEVICE_INDEX, DEVICE_ID, SERIAL_DEVICE_LENGTH) == 0) {
                    status = 2;
                    if(serialBuffer[SERIAL_CMD_INDEX] == SERIAL_SET_TARGET_CMD) {
                        // Check this is the correct message length and either process the command or wait for another end char
                        if(bufferIndex == SERIAL_SET_TARGET_CMD_LENGTH) {
                            status = 3;
                            sendMessage("Got set target command");
                        } else {
                            status = 4;
                            serialBuffer[bufferIndex] = readChar;
                            bufferIndex++;
                        }
                    }
                } else {
                    status = 5;
                    //sendMessage("Invalid device id");

                    char deviceId[SERIAL_DEVICE_LENGTH];
                    strncpy(deviceId, serialBuffer + SERIAL_DEVICE_INDEX, SERIAL_DEVICE_LENGTH);
                    int msgLength = 20;
                    char message[msgLength];
                    snprintf(message, msgLength, "invalid DeviceID %s", deviceId);
                    //sendMessage(message);
                }
                
                // Reset the buffer index
                bufferIndex = 0;
        } else {
            serialBuffer[bufferIndex] = readChar;
            bufferIndex++;
            // Reset the buffer index if it exceeds the buffer size
            if (bufferIndex >= SERIAL_BUFFER_SIZE) {
                bufferIndex = 0;
                status = 6;
                //sendMessage("buffer exceeded");
            }
        }
    }

/*
    if(status == 0) {
        sendMessage("No data");
    } else if(status == 1) {
        sendMessage("got end char");
    } else if(status == 2) {
        sendMessage("Device ID matches");
    } else if(status == 3) {
        sendMessage("set volume cmd");
    } else if(status == 4) {
        sendMessage("false end in set volume");
    } else if(status == 5) {
        sendMessage("Invalid device id");
    } else if(status == 6) {
        sendMessage("Buffer exceeded");
    }
    */

    // return data in tmp buffer
    for(int i = 0; i < tmpBufferIndex; i++) {
        USART_Write(tmpBuffer[i]);
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

void initializeSerial(void) {
    USART_Init();
    USART_Flush();
}

int main(void) {
    setupPinModes();
    configureTimer0();
    configureTimer1();
    configureTimer2();

    initializeSerial();

    sei();

    sendMessage("Starting");
    while(1) {
        readInput();

        _delay_ms(100);
    }
    return 1;
}
