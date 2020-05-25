
const int solenoidPin = 4;
const int flowPin = 2;
const int buttonPin = 3;
const int updateInterval = 1000;

// Globals
// flowcontroller
bool isOpen = false;  // Flag indicating if the solenoid (flow) is open
// flowmeter
double totalVolume = 0; // The total volume of liquid in ml that has passed
double rate = 0;        // Flow rate in litres per hour (I think)
int volumeLimit = 500;  // Limit in ml of volume that will flow before it is closed
double tempVolume = 0;  // The volume of water that has flowed when tracking the volumeLimit




float oldTime = 0;
float loopRunTime = 0;

void setup() {
  // put your setup code here, to run once:
  randomSeed(analogRead(0));
  Serial.begin(9600);
  setupLCD();
  Serial.print("Starting ");
  Serial.print(random(100));
  Serial.print("\n");

  pinMode(solenoidPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(flowPin, INPUT);
  loopRunTime = millis(); // A bit moot but it keeps our initial delta accurate
  enableFlow();
  sendStatus(isOpen, rate, totalVolume, volumeLimit, tempVolume);;
  showVolumeAndStatus(totalVolume, isOpen);

}

void loop() {
   float oldTime = loopRunTime;
   loopRunTime = millis();
   float deltaTime = loopRunTime - oldTime;
   handleButtonPress();
   readSerial();
}
