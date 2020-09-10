
const int solenoidPin = 4;
const int valveOpenPin = 5;
const int valveClosePin = 6;
const int flowPin = 2;
const int buttonPin = 3;
const int updateInterval = 1000;

// Globals
// flowcontroller
bool isOpen = false;  // Flag indicating if the solenoid (flow) is open
// flowmeter
unsigned long totalVolume = 0; // The total volume of liquid in microlitres that has passed
unsigned long flowRate = 0;        // Smoothed flow rate in ml per hour
unsigned int volumeLimit = 50000;  // Limit in ml of volume that will flow before it is closed
unsigned long tempVolume = 0;  // The volume of water that has flowed when tracking the volumeLimit

// Throttling for flow samples
unsigned long lastSampleTime = 0; // The last time the volume was sampled
const int sampleInterval = 300;
// Throttling for serial reads
unsigned long lastSerialRead = 0;
long serialReadInterval = 500;



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
  sendStatus(isOpen, flowRate, totalVolume, volumeLimit, tempVolume);
  showVolumeAndStatus(totalVolume, isOpen);

}

void loop() {
   float oldTime = loopRunTime;
   loopRunTime = millis();
   float deltaTime = loopRunTime - oldTime;
   if(deltaTime > 100) {
   }
   handleButtonPress();

   if(loopRunTime - lastSampleTime > sampleInterval) {
    // Update flow status
    if(updateFlow()) {
      long startT = millis();
      sendStatus(isOpen, flowRate, totalVolume, volumeLimit, tempVolume);
  
      if(volumeLimit > 0) {
        showProgressAndFlow(tempVolume, volumeLimit, flowRate);
      } else {
        showVolumeAndFlow(totalVolume, flowRate);    
      }
      lastSampleTime = loopRunTime;

      
       long endT = millis();
       
     } else if(loopRunTime > lastSerialRead + serialReadInterval) {
       // Don't read serial and update flow in the same loop - it may take a long time
       readSerial();
     }
      
    }
    

}
