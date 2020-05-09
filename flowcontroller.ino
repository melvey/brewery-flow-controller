float isOpen = 0;

void openFlow() {
  if(isOpen == 0) {
    Serial.print("Open flow\n");
    digitalWrite(solenoidPin, HIGH);
    isOpen = 1;
  }
}

void closeFlow() {
  if(isOpen == 1) {
    Serial.print("Close flow\n");
    digitalWrite(solenoidPin, LOW);
    isOpen = 0;
  }
}

void toggleFlow() {
  if(isOpen == 0) {
    openFlow();
  } else {
    closeFlow();
  }
}
