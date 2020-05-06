
const int solenoidPin = 2;
const int flowPin = 3;
const int buttonPin = 4;
const int updateInterval = 1000;

volatile int flowTicks = 0;
int totalVolume = 0;
float oldTime = 0;
float loopRunTime = 0;
int buttonPressed = 0;

void setup() {
  // put your setup code here, to run once:

  pinMode(solenoidPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  loopRunTime = millis(); // A bit moot but it keeps our initial delta accurate

}

void loop() {
   float oldTime = loopRunTime;
   loopRunTime = millis();
   float deltaTime = loopRunTime - oldTime;
   handleButtonPress();
   calculateFlow();

}

void onFlow() {
  flowTicks++;
}

void openFlow() {
  digitalWrite(solenoidPin, HIGH);
}

void closeFlow() {
  digitalWrite(solenoidPin, LOW);
}

void resetVolume() {
  totalVolume = 0;
}

void handleButtonPress() {
  if(digitalRead(buttonPin) == HIGH) {
    if(buttonPressed != 1) {
      buttonPressed = 1;
      openFlow();
    }
  } else {
    if(buttonPressed == 1) {
      buttonPressed = 0;
      closeFlow();
    }
  }
}


void calculateFlow () {
  // https://wiki.seeedstudio.com/G1_and_2_inch_Water_Flow_Sensor/
    flowTicks = 0;   //Set NbTops to 0 ready for calculations
  sei();      //Enables interrupts
  delay (1000);   //Wait 1 second
  cli();      //Disable interrupts
  int flow = (flowTicks * 60 / 7.5); //(Pulse frequency x 60) / 7.5Q, = flow rate

in L/hour
  Serial.print (flow, DEC); //Prints the number calculated above
  Serial.print (" L/hour\r\n"); //Prints "L/hour" and returns a  new line
}
