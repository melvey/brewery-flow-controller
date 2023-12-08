#include <avr/interrupt.h>
#include "standard.h"
#include <avr/io.h>
#include "usart.h"

// *********************************************************************************
// usart.c
// *********************************************************************************

#define BUFFER_SIZE 64

unsigned char serial_buffer[BUFFER_SIZE];
unsigned int write_index = 0;
unsigned int read_index = 0;
unsigned int buffer_overflow = TRUE;


void USART_Init(void)
{
	// Set Baud Rate to 1 Mbps (16 MHz Clock Rate)
	UBRR0H = 0;
	UBRR0L = 1;			// From PG. 199 of the ATMega328s Manual
	// Alternate baud rate settings for future reference
	//UBRR0L = 3;				// 500 kbps
    //UBRR0L = 7;				// 250 kbps
    //UBRR0L = 25;				// 76.8 kbps
    //UBRR0L = 34;				// 57.6 kbps
    //UBRR0L = 207;				// 96 kbps


	// High Speed Mode ON
	UCSR0A = (1 << U2X0);
	
	// Enable transmit and receive (no interrupts)
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	
	// Enable received data interupt
	UCSR0B |= (1 << RXCIE0);
}

ISR(USART_RX_vect)
{
    // Read the recieved data
    unsigned char data = UDR0;

    // Calculate the next write index
    unsigned char next_write_index = (write_index + 1) % BUFFER_SIZE;

    //USART_Write(data);

    // Store the data in the buffer if it is not full
    if (next_write_index != read_index)
    {
        serial_buffer[write_index] = data;
        write_index = next_write_index;
    } else {
        buffer_overflow = TRUE;
    }
}
    


void USART_Write(unsigned char data)
{
	// Wait Until The Transmit Buffer Is Empty
	while (!(UCSR0A & (1 << UDRE0)));

	// Write the Byte
	UDR0 = data;
}

void USART_WriteString(char *string)
{
	// Write characters out until we reach a NULL character
	while (*string != 0)
	{
		// Write the current character
		USART_Write((unsigned char)*string);
		// Next character
		string++;
	}
}

void USART_WriteCRLF(void)
{
	USART_Write(CR);
	USART_Write(LF);
}

void USART_WriteUnsignedChar(unsigned char number)
{
	// This number is at most 255 (3 digits)
	unsigned char ones = number % 10;
	number /= 10;
	unsigned char tens = number % 10;
	number /= 10;
	unsigned char hundreds = number % 10;
	
    if (hundreds > 0) 
    {
        USART_Write('0' + hundreds);
    }
    if (hundreds > 0 || tens > 0) 
    {
        USART_Write('0' + tens);
    }
    USART_Write('0' + ones);
}

void USART_WriteUnsignedInt(unsigned int number)
{
	unsigned char write_data = FALSE;
	// This number is at most 65535 (5 digits)
	unsigned char digit1 = number % 10;
	number /= 10;
	unsigned char digit2 = number % 10;
	number /= 10;
	unsigned char digit3 = number % 10;
	number /= 10;
	unsigned char digit4 = number % 10;
	number /= 10;
	unsigned char digit5 = number % 10;

	if (digit5 > 0) write_data = TRUE;
	if (write_data) USART_Write('0' + digit5);
	if (digit4 > 0) write_data = TRUE;
	if (write_data) USART_Write('0' + digit4);
	if (digit3 > 0) write_data = TRUE;
	if (write_data) USART_Write('0' + digit3);
	if (digit2 > 0) write_data = TRUE;
	if (write_data) USART_Write('0' + digit2);
	USART_Write('0' + digit1);
}

void USART_WriteSignedInt(int number)
{
	// This number is at most +32768 and at least -32767 (5 digits + sign)s

	// Print the sign if applicable
	if (number < 0)
	{
		USART_Write('-');
		// Convert the number into an unsigned int
		number *= -1;
	}

	// Print out the positive number result
	USART_WriteUnsignedInt((unsigned int)number);
}

unsigned char USART_Receive(void)
{
    // If the buffer is empty do not do anything
    if (read_index == write_index)
    {
        return 0;
    }

    // Read the data from the buffer
    unsigned char data = serial_buffer[read_index];

    // Calculate the next read index
    read_index = (read_index + 1) % BUFFER_SIZE;

    return data;

}

unsigned int USART_Ready(void) {
    if (read_index == write_index)
    {
		return FALSE;
	} else {
		return TRUE;
	}
}

void USART_Flush(void)
{
	unsigned char dummy;
	
	while (UCSR0A & (1 << RXC0))
		dummy = UDR0;
}

unsigned int USART_HasOverflow(void) {
    return buffer_overflow;
}

void USART_ClearOverflow(void) {
    buffer_overflow = FALSE;
}
