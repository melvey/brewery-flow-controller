
const int solenoidPin = 4;
const int flowPin = 2;
const int buttonPin = 3;
const int updateInterval = 1000;


float oldTime = 0;
float loopRunTime = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.print("Starting\n");

  pinMode(solenoidPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(flowPin, INPUT);
  loopRunTime = millis(); // A bit moot but it keeps our initial delta accurate
  enableFlow();

}

void loop() {
   float oldTime = loopRunTime;
   loopRunTime = millis();
   float deltaTime = loopRunTime - oldTime;
   handleButtonPress();
   readSerial();

}
