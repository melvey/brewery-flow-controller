char deviceId[] = "FL01";

char marker = ';';
char toggleAction[] = "ON"; // Value is 0 or 1
char setLimitAction[] = "LM"; // Value is the limit in ml
char turnOnForLimit[] = "LO"; // Value is the limit in ml

char seperator = ':';
char startMarker = 0x02;
char endMarker = 0x03;
//https://arduino.stackexchange.com/questions/4214/communication-protocol-best-practices-and-patterns

// Format :DDDDAAVVVV:
// : indicates start/end
// DDDD is device ID
// AA is action code
// VVVV is the value

void sendStatus(bool isOpen, float rate, unsigned int totalVolume, unsigned int volumeLimit, unsigned int tempVolume) {
  // format ;DDDD:TTTT:1:2222:3333:4444:5555;
  union LongByte
  {
     unsigned long value;
     byte bytes[4];
  };
  union IntByte
  {
     unsigned int value;
     byte bytes[4];
  };
  union FloatByte
  {
     float value;
     byte bytes[4];
  };

  LongByte now;
  now.value = millis();


  FloatByte rateBytes;
  rateBytes.value = rate;

  
  char message[33];



  message[0] = marker;
  message[1] = deviceId[0];
  message[2] = deviceId[1];
  message[3] = deviceId[2];
  message[4] = deviceId[3];
  message[5] = seperator;
  message[6] = now.bytes[0];
  message[7] = now.bytes[1];
  message[8] = now.bytes[2];
  message[9] = now.bytes[4];
  message[10] = seperator;
  message[11] = isOpen ? 1 : 0;
  message[12] = seperator;
  message[13] = rateBytes.bytes[0];
  message[14] = rateBytes.bytes[1];
  message[15] = rateBytes.bytes[2];
  message[16] = rateBytes.bytes[3];
  message[17] = seperator;
  message[18] = (totalVolume >> 24) & 0xFF;
  message[19] = (totalVolume >> 16) & 0xFF;
  message[20] = (totalVolume >> 8) & 0xFF;
  message[21] = totalVolume & 0xFF;
  message[22] = seperator;
  message[23] = (volumeLimit >> 24) & 0xFF;
  message[24] = (volumeLimit >> 16) & 0xFF;
  message[25] = (volumeLimit >> 8) & 0xFF;
  message[26] = volumeLimit & 0xFF;
  message[27] = seperator;
  message[28] = (tempVolume >> 24) & 0xFF;
  message[29] = (tempVolume >> 16) & 0xFF;
  message[30] = (tempVolume >> 8) & 0xFF;
  message[31] = tempVolume & 0xFF;
  message[32] = marker;

  Serial.write(message);
  Serial.print(message);
}


void readSerial() {

	char command[2];
	
	char readByte[1];
	while(Serial.readBytes(readByte, 1) > 0) {
    Serial.print("Read: ");
    Serial.print(readByte[0]);
    Serial.print("?!\n");
		if(readByte[0] == marker && Serial.peek() != marker) {
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
			if(strcmp(incomingId, deviceId) != 0) {
        Serial.print("Invalid deviceID ");
        Serial.println(incomingId);
				return;
			}

			Serial.readBytes(incomingAction, 2);
      Serial.print("Action: ");
      Serial.println(incomingAction);

			if(strcmp(incomingAction, toggleAction) == 0) {
				Serial.readBytes(incomingValue.bytes, 4);
        Serial.print("On value:");
        Serial.println(incomingValue.value);
				if(incomingValue.value > 0) {
					// turn on
					openFlow();
				} else {
					// turn off
					closeFlow();
				}
			} else if(strcmp(incomingAction, setLimitAction) == 0) {
				Serial.readBytes(incomingValue.bytes, 4);
				turnOffAfter(incomingValue.value);
			} else if(strcmp(incomingAction, turnOnForLimit) == 0) {
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
	
}
