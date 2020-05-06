
const int solenoidPin = 2;
const int flowPin = 4;
const int buttonPin = 3;
const int updateInterval = 1000;

volatile int flowTicks = 0;
int totalVolume = 0;

float oldTime = 0;
float loopRunTime = 0;
float isOpen = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.print("Starting\n");

  pinMode(solenoidPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  loopRunTime = millis(); // A bit moot but it keeps our initial delta accurate

}

void loop() {
   float oldTime = loopRunTime;
   loopRunTime = millis();
   float deltaTime = loopRunTime - oldTime;
   handleButtonPress();
//   calculateFlow();

}

void onFlow() {
  flowTicks++;
}

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

void resetVolume() {
  totalVolume = 0;
}




void calculateFlow () {
  // https://wiki.seeedstudio.com/G1_and_2_inch_Water_Flow_Sensor/
    flowTicks = 0;   //Set NbTops to 0 ready for calculations
  sei();      //Enables interrupts
  delay (1000);   //Wait 1 second
  cli();      //Disable interrupts
  int flow = (flowTicks * 60 / 7.5); //(Pulse frequency x 60) / 7.5Q, = flow rate in L/hour
  Serial.print (flow, DEC); //Prints the number calculated above
  Serial.print (" L/hour\r\n"); //Prints "L/hour" and returns a  new line
}
