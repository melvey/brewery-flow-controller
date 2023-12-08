#ifndef IO_FUNCTIONS_H
#define IO_FUNCTIONS_H


#include <avr/io.h>

void initializeSerial(void);

void sendMessage(const char *message);

void sendCommand(const char cmd, unsigned int cmdLength, const char *body);

void readInput(void);


//int hasMessage(void);

//char* getMessage(void);

#endif
