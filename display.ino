
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
  lcd.setCursor(0, 1);
  if(line2Display != statusMsg) {
    col += lcd.print("Valve ");    
  } else {
    lcd.setCursor(6, 1);    
    col = 6;
  }
  if(isOpen) {
    col += lcd.print("Open");
  } else {
    col += lcd.print("Closed");
  }
  for(;col < lcdCols; col++) {
    lcd.print(" ");
  }

  line1Display = statusMsg;  
}

void showFlowRate(double rate) {
  int col = 0;
  lcd.setCursor(0, 1);
  if(line2Display != rateMsg) {
    col += lcd.print("Rate: ");    
  } else {
    lcd.setCursor(5, 1);    
    col = 5;
  }

  col += lcd.print(rate);
  col += lcd.print("L/H");

  for(;col < lcdCols; col++) {
    lcd.print(" ");
  }
  
  line1Display = rateMsg;  
  
}

void showProgress(double progress, int limit) {
  // Print first line
  lcd.setCursor(0, 0);
  int col = 0;
  if(limit > 1000) {
    // Show litres
    col += lcd.print(progress / 1000);
    col += lcd.print("/");
    col += lcd.print(limit / 1000);
    col += lcd.print("L (");
  } else {
    // Show ml
    col += lcd.print((int)progress);
    col += lcd.print("/");
    col += lcd.print(limit);
    col += lcd.print("ml (");
  }
  
  col += lcd.print(round(progress/limit * 100));
  col += lcd.print("%)");
  
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
