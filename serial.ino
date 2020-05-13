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
