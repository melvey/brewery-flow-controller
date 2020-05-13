bool isOpen = false;

void openFlow() {
  if(isOpen == false) {
    Serial.print("Open flow\n");
    digitalWrite(solenoidPin, HIGH);
    isOpen = true;
  }
}

void closeFlow() {
  if(isOpen == true) {
    Serial.print("Close flow\n");
    digitalWrite(solenoidPin, LOW);
    isOpen = false;
  }
}

void toggleFlow() {
  if(isOpen == false) {
    openFlow();
  } else {
    closeFlow();
  }
}
