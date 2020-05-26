
void openFlow() {
  if(isOpen == false) {
//    Serial.print("Open flow\n");
    digitalWrite(solenoidPin, HIGH);
    isOpen = true;
    sendStatus(isOpen, rate, totalVolume, volumeLimit, tempVolume);
    showVolumeAndStatus(totalVolume, isOpen);
  }
}

void closeFlow() {
  if(isOpen == true) {
//    Serial.print("Close flow\n");
    digitalWrite(solenoidPin, LOW);
    isOpen = false;
    sendStatus(isOpen, rate, totalVolume, volumeLimit, tempVolume);
    showVolumeAndStatus(totalVolume, isOpen);
  }
}

void toggleFlow() {
  if(isOpen == false) {
    openFlow();
  } else {
    closeFlow();
  }
}
