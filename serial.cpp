#define byte uint8_t
#include <stdint.h>
#include <Arduino.h>
#include "flowcontroller.h"
#include "flowmeter.h"
#include "state.h";

char deviceId[] = "FL01";

char toggleAction[] = "ON"; // Value is 0 or 1
char setLimitAction[] = "LM"; // Value is the limit in ml
char turnOnForLimit[] = "LO"; // Value is the limit in ml
char sendDataAction[] = "DT";

char seperator = 0x3a;
char startMarker = 0x02;
char endMarker = 0x03;
//https://arduino.stackexchange.com/questions/4214/communication-protocol-best-practices-and-patterns

unsigned long lastSerialRead = 0;
long serialReadInterval = 500;


// Format :DDDDAAVVVV:
// : indicates start/end
// DDDD is device ID
// AA is action code
// VVVV is the value

union LongByte
{
  unsigned long value;
  byte bytes[4];
};


/**

*/
void sendStatus() {
  unsigned long rate = getRate();
  unsigned long totalVolume = getTotalVolume();
  unsigned int volumeLimit = getLimit();
  unsigned long tempVolume = getCurrentVolume();
  
  // format ;DDDDAATTTT12222333344445555;

  LongByte now;
  now.value = millis();

  LongByte rateBytes;
  rateBytes.value = rate;

  LongByte totalVolBytes;
  totalVolBytes.value = totalVolume;
  
  LongByte tmpVolBytes;
  tmpVolBytes.value = tempVolume;

  LongByte volLimitBytes;
  volLimitBytes.value = volumeLimit;

  char message[29];


  message[0] = startMarker;
  memcpy(&message[1], &deviceId, 4);
  memcpy(&message[5], &sendDataAction, 2);
  memcpy(&message[7], &now.bytes, 4);
  message[11] = isOpen() ? 1 : 0;
  memcpy(&message[12], &rateBytes.bytes, 4);
  memcpy(&message[16], &totalVolBytes.bytes, 4);
  memcpy(&message[20], &volLimitBytes.bytes, 4);
  memcpy(&message[24], &tmpVolBytes.bytes, 4);
  message[28] = endMarker;

  Serial.write(message, sizeof(message));
}


void readSerial() {

  char command[2];

  char readByte[1];
  while (Serial.available()) {
    Serial.readBytes(readByte, 1);
    Serial.print("Read: ");
    Serial.print(readByte[0]);
    Serial.print("?!\n");
    if (readByte[0] == startMarker && Serial.peek() != startMarker) {
      Serial.println("Read on");
      char incomingId[5] = "****\0";
      char incomingAction[3] = "**\0";
      union LongByte
      {
        unsigned long value;
        byte bytes[4];
      };
      LongByte incomingValue;


      Serial.readBytes(incomingId, 4);
      if (strcmp(incomingId, deviceId) != 0) {
        Serial.print("Invalid deviceID ");
        Serial.println(incomingId);
        return;
      }

      Serial.readBytes(incomingAction, 2);
      Serial.print("Action: ");
      Serial.println(incomingAction);

      if (strcmp(incomingAction, toggleAction) == 0) {
        Serial.readBytes(incomingValue.bytes, 4);
        Serial.print("On value:");
        Serial.println(incomingValue.value);
        if (incomingValue.value > 0) {
          // turn on
          openFlow();
        } else {
          // turn off
          closeFlow();
        }
      } else if (strcmp(incomingAction, setLimitAction) == 0) {
        Serial.readBytes(incomingValue.bytes, 4);
        turnOffAfter(incomingValue.value);
      } else if (strcmp(incomingAction, turnOnForLimit) == 0) {
        Serial.readBytes(incomingValue.bytes, 4);
        turnOffAfter(incomingValue.value);
        openFlow();
      } else {
        Serial.print("Unknown action: ");
        Serial.println(incomingAction);
      }

    }
    // discard unexpected data
  }
  
  lastSerialRead = millis();
}

bool readSerialThrottled() {
   if(millis() > lastSerialRead + serialReadInterval) {
     // Don't read serial and update flow in the same loop - it may take a long time
     readSerial();
     return true;
   }
   return false;
}
