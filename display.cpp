#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "state.h"


#define I2C_ADDR  0x27

// Construct an LCD object and pass it the 
// I2C address, width (in characters) and
// height (in characters). Depending on the
// Actual device, the IC2 address may change.
static LiquidCrystal_I2C *lcd;

static int startMsg = 0;
static int volumeMsg = 1;
static int statusMsg = 2;
static int rateMsg = 3;
static int progressMsg = 4;

static int lcdCols = 16;
static int lcdLines = 2;

static int line1Display = startMsg;
static int line2Display = startMsg;

void printStartMessage() {
  // Move the cursor characters to the right and
  // zero characters down (line 1).
  lcd->setCursor(4, 0);

  // Print HELLO to the screen, starting at 5,0.
  lcd->print("Starting");

  // Move the cursor to the next line and print
  // WORLD.
  lcd->setCursor(0, 1);      
  lcd->print("Flow Controller");  

  line1Display = startMsg;
  line2Display = startMsg;
}

void setupLCD() {
  lcd = new LiquidCrystal_I2C (I2C_ADDR, 16, 2);
  // The begin call takes the width and height. This
  // Should match the number provided to the constructor.
  lcd->begin(lcdCols, lcdLines);
//  Wire.begin(D2, D1);
  lcd->init();

  // Turn on the backlight.
  lcd->backlight();

  printStartMessage();
}

void clearLCDLine(int line, int startIndex)
{               
  lcd->setCursor(startIndex ,line);
  for(int n = startIndex; n < lcdCols; n++)
  {
    lcd->print(" ");
  }
}


/**
 * totalVolume: The total volume in microletres that has passed
 */
void showVolume() {
  unsigned long totalVolume = getTotalVolume();
  // Print first line
  int col = 0;
  if(line1Display != volumeMsg) {
    lcd->setCursor(0, 0);
    col += lcd->print("Volume: ");
  } else {
    lcd->setCursor(8, 0);
    col = 8;
  }
  
  if(totalVolume < 1000000) {
    // convert microlitres to mililitres
     col += lcd->print(totalVolume/1000);
     col += lcd->print("ml");
  } else {
    // Convert microlitres to litres with one decimal point
    col += lcd->print(totalVolume/1000000);
    col += lcd->print(".");
    col += lcd->print((totalVolume/10000)%100); // convert to two decimal points
    col += lcd->print("l");
  }
  for(;col < lcdCols; col++) {
    lcd->print(" ");
  }

  line1Display = volumeMsg;
}

void showStatus() {
  int col = 0;
  lcd->setCursor(0, 1);
  if(line2Display != statusMsg) {
    col += lcd->print("Valve ");    
  } else {
    lcd->setCursor(6, 1);    
    col = 6;
  }
  if(isOpen()) {
    col += lcd->print("Open");
  } else {
    col += lcd->print("Closed");
  }
  for(;col < lcdCols; col++) {
    lcd->print(" ");
  }

  line1Display = statusMsg;  
}

void showFlowRate() {
  unsigned long rate = getRate();
  int col = 0;
  lcd->setCursor(0, 1);
  if(line2Display != rateMsg) {
    col += lcd->print("Rate: ");    
  } else {
    lcd->setCursor(5, 1);    
    col = 5;
  }

  col += lcd->print(rate / 1000);
  col += lcd->print(".");
  col += lcd->print((rate / 100) % 10);
  col += lcd->print("L/H");

  for(;col < lcdCols; col++) {
    lcd->print(" ");
  }
  
  line1Display = rateMsg;  
  
}

void showProgress() {
  unsigned long progress = getCurrentVolume();
  unsigned int limit = getLimit();
  // Print first line
  lcd->setCursor(0, 0);
  int col = 0;
  if(limit > 1000) {
    // Show litres
    col += lcd->print(progress / 1000);
    col += lcd->print("/");
    col += lcd->print(limit / 1000);
    col += lcd->print("L (");
  } else {
    // Show ml
    col += lcd->print((int)progress);
    col += lcd->print("/");
    col += lcd->print(limit);
    col += lcd->print("ml (");
  }
  
  col += lcd->print(round(progress/limit * 100));
  col += lcd->print("%)");
  
  for(;col < lcdCols; col++) {
    lcd->print(" ");
  }

  line1Display = progressMsg;  
}

void showVolumeAndStatus() {
  showVolume();
  showStatus();
}

void showVolumeAndFlow() {
  showVolume();
  showFlowRate();
}

void showProgressAndFlow() {
  showProgress();
  showFlowRate();
}
