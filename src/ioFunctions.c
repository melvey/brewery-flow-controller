#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "usart.h"
#include "flow.h"

#define SERIAL_BUFFER_SIZE 128
#define SERIAL_DEVICE_INDEX 1
#define SERIAL_DEVICE_LENGTH 2
#define SERIAL_CMD_INDEX 3

const char DEVICE_ID[2] = "F1";
const char SERIAL_MSG_CMD = 'M';
const char SERIAL_SET_TARGET_CMD = 'T';
const int SERIAL_SET_TARGET_CMD_LENGTH = 9;
const char SERIAL_START = 0x02;
const char SERIAL_END = 0x03;

char serialBuffer[SERIAL_BUFFER_SIZE];
unsigned short int bufferIndex = 0;


void initializeSerial(void) {
    USART_Init();
    USART_Flush();
}

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

void sendCommand(const char cmd, unsigned int cmdLength, const char *body) {

    USART_Write(SERIAL_START);
    USART_Write(DEVICE_ID[0]);
    USART_Write(DEVICE_ID[1]);
    USART_Write(cmd);

    for(int i = 0; i < cmdLength; i++) {
        USART_Write(body[i]);
    }
    USART_Write(SERIAL_END);
}

void readInput(void) {
    // track status of read to avoid writing while reading
    // This should later be split off to be the return value of a processCommand function
    int status = 0;
    char readChar;
    while (USART_Ready() == 1) {
        readChar = USART_Receive();

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
                        if(bufferIndex + 1 == SERIAL_SET_TARGET_CMD_LENGTH) {
                            status = 3;
                            unsigned int newVolume = serialBuffer[4];
                            memcpy(&newVolume, serialBuffer + 4, 4);
                            setTargetVolume(newVolume);

                            int msgLength = 20;
                            char message[msgLength];
                            snprintf(message, msgLength, "Set volume to %u", newVolume);
                            sendMessage(message);
                        } else {
                            status = 4;
                            serialBuffer[bufferIndex] = readChar;
                            bufferIndex++;

                            int msgLength = 45;
                            char message[msgLength];
                            snprintf(message, msgLength, "Invalid length: expected %d chars but got %d", SERIAL_SET_TARGET_CMD_LENGTH, bufferIndex);
                            sendMessage(message);
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
                    sendMessage(message);
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

    // return data in tmp buffer
 
    if(status == 1) {
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
   
}

