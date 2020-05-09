const float holdTime = 500;


int holding = 0;
float pressStart = 0;
float lastPress = 0;

void handleButtonPress() {
  int buttonState = digitalRead(buttonPin);
  float now = millis();
  if(buttonState == HIGH) {
    float now = millis();
    
    if(pressStart == 0) {
      pressStart = now;
    }
    
    if(now - pressStart > holdTime) {
      Serial.print("Holding:");
      Serial.print(now - pressStart);
      Serial.print("\n");
      holding = 1;
      onHold();
    }
  } else if(pressStart != 0) {
    pressStart = 0;
    if(holding) {
      holding = 0;
      onRelease();
    } else {
      onTap();
    }
  }
}


void onHold() {
  Serial.print("Holding");
  openFlow();
}

void onRelease() {
  Serial.print("Released");
  closeFlow();
}

void onTap() {
  Serial.print("Tap");
  toggleFlow();
  turnOffAfter(300);
}
