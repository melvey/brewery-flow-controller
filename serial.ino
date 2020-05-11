char deviceId[] = "FL01";

char marker = ';';
char toggleAction[] = "ON"; // Value is 0 or 1
char setLimitAction[] = "LM"; // Value is the limit in ml
char turnOnForLimit[] = "LO"; // Value is the limit in ml

// Format :DDDDAAVVVV:
// : indicates start/end
// DDDD is device ID
// AA is action code
// VVVV is the value

void readSerial() {

	char command[2];
	
	char readByte[1];
	while(Serial.readBytes(readByte, 1) > 0)
		if(readByte == marker && Serial.peek() != marker) {
			char incomingId[4];
			char incomingAction[2];
			union LongByte
			{
				 unsigned long value;
				 byte bytes[4];
			};
      LongByte incomingValue;

      
			Serial.readBytes(incomingId, 4);
			if(strcmp(incomingId, deviceId) != 0) {
				return;
			}

			Serial.readBytes(incomingAction, 2);

			if(strcmp(incomingAction, toggleAction) == 0) {
				Serial.readBytes(incomingValue.bytes, 4);
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
			}

		}
		// discard unexpected data
	
}
