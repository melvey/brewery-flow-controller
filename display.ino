
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// Construct an LCD object and pass it the 
// I2C address, width (in characters) and
// height (in characters). Depending on the
// Actual device, the IC2 address may change.
LiquidCrystal_I2C lcd(0x27, 16, 2);

int startMsg = 0;
int volumeMsg = 1;
int statusMsg = 2;
int rateMsg = 3;
int progressMsg = 4;

int lcdCols = 16;
int lcdLines = 2;

int line1Display = startMsg;
int line2Display = startMsg;

void setupLCD() {

  // The begin call takes the width and height. This
  // Should match the number provided to the constructor.
  lcd.begin(lcdCols, lcdLines);
//  Wire.begin(D2, D1);
  lcd.init();

  // Turn on the backlight.
  lcd.backlight();

  printStartMessage();
}

void clearLCDLine(int line, int startIndex)
{               
  lcd.setCursor(startIndex ,line);
  for(int n = startIndex; n < lcdCols; n++)
  {
    lcd.print(" ");
  }
}

void printStartMessage() {
  // Move the cursor characters to the right and
  // zero characters down (line 1).
  lcd.setCursor(4, 0);

  // Print HELLO to the screen, starting at 5,0.
  lcd.print("Starting");

  // Move the cursor to the next line and print
  // WORLD.
  lcd.setCursor(0, 1);      
  lcd.print("Flow Controller");  

  line1Display = startMsg;
  line2Display = startMsg;
}

void showVolume(double totalVolume) {
  // Print first line
  int col = 0;
  if(line1Display != volumeMsg) {
    lcd.setCursor(0, 0);
    col += lcd.print("Volume: ");
  } else {
    lcd.setCursor(8, 0);
    col = 8;
  }
  
  if(totalVolume < 1000) {
     col += lcd.print(round(totalVolume));
     col += lcd.print("ml");
  } else {
    col += lcd.print(round(totalVolume)/1000);
    col += lcd.print("l");
  }
  for(;col < lcdCols; col++) {
    lcd.print(" ");
  }

  line1Display = volumeMsg;
}

void showStatus(boolean isOpen) {
  int col = 0;
  if(line2Display != statusMsg) {
    lcd.setCursor(0, 1);
    lcd.print("Valve ");    
  } else {
    lcd.setCursor(6, 1);    
  }
  col = 6;
  if(isOpen) {
    lcd.print("Open");
    col = col + 4;
  } else {
    lcd.print("Closed");
    col = col + 6;
  }
  for(;col < lcdCols; col++) {
    lcd.print(" ");
  }

  line1Display = statusMsg;  
}

void showFlowRate(double rate) {
  int col = 0;
  if(line2Display != rateMsg) {
    lcd.setCursor(0, 1);
    lcd.print("Rate: ");    
  } else {
    lcd.setCursor(5, 1);    
  }
  col = 5;

  int bytes = lcd.print(rate);
  lcd.print("L/H");
  col = col + bytes + 3;

  line1Display = rateMsg;  
  
}

void showProgress(double progress, int limit) {
  // Print first line
  lcd.setCursor(0, 0);
  int col = 0;
  int bytes;
  if(limit > 1000) {
    // Show litres
    bytes = lcd.print(progress / 1000);
    lcd.print("/");
    col = col + bytes + 1;
    bytes = lcd.print(limit / 1000);
    lcd.print("L ");
    col = col + bytes  + 2;
  } else {
    // Show ml
    bytes = lcd.print((int)progress);
    lcd.print("/");
    col = col + bytes + 1;
    bytes = lcd.print(limit);
    lcd.print("ml ");
    col = col + bytes + 3;    
  }
  
  lcd.print("(");
  bytes = lcd.print(round(progress/limit * 100));
  lcd.print("%)");
  col = col + bytes + 3;
  
  for(;col < lcdCols; col++) {
    lcd.print(" ");
  }

  line1Display = progressMsg;  
}

void showVolumeAndStatus(double totalVolume, boolean isOpen) {
  showVolume(totalVolume);
  showStatus(isOpen);
}

void showVolumeAndFlow(double totalVolume, double flow) {
  showVolume(totalVolume);
  showFlowRate(flow);
}

void showProgressAndFlow(double progress, int limit, double flow) {
  Serial.print("Show progress and flow\n");
  Serial.print(progress);
  showProgress(progress, limit);
  showFlowRate(flow);
}
