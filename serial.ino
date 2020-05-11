char[] deviceId = "FL01";

char marker = ';';
char[] toggleAction = ['O','N']; // Value is 0 or 1
char[] setLimitAction = ['L', 'M']; // Value is the limit in ml
char[] turnOnForLimit = ['L','O']; // Value is the limit in ml

// Format :DDDDAAVVVV:
// : indicates start/end
// DDDD is device ID
// AA is action code
// VVVV is the value

void readSerial() {

	char[] command = char[2];
	
	char readByte = null;
	while(Serial.readBytes(readByte, 1) > 0)
		if(readByte = marker && Serial.peek() != marker) {
			char[] incomingID = char[4];
			char[] incomingAction = char[2];
			union incomingValue
			{
				 unsigned long value;
				 byte bytes[4];
			};

			Serial.readBytes(incomingId, 4);
			if(strcmp(incomingId, deviceId) != 0) {
				return;
			}

			Serial.readBytes(incomingAction, 2);

			if(strCmp(incomingAction, toggleAction) == 0) {
				Serial.readBytes(incomingValue.bytes, 4);
				if(incomingValue.value > 0) {
					// turn on
					openValve();
				} else {
					// turn off
					closeValve();
				}
				return;
			}
			if(strCmp(incomingAction, setLimitAction == 0) {
				Serial.readBytes(incomingValue.bytes, 4);
				turnOffAfter(incomingValue.value);
				return;
			}
			if(strCmp(incomingAction, turnOnForLimit == 0) {
				Serial.readBytes(incomingValue.bytes, 4);
				turnOffAfter(incomingValue.value);
				openValve();
				return
			}

		}
		// discard unexpected data
	}
	
}
