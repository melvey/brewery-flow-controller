
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

// Construct an LCD object and pass it the 
// I2C address, width (in characters) and
// height (in characters). Depending on the
// Actual device, the IC2 address may change.
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setupLCD() {

  // The begin call takes the width and height. This
  // Should match the number provided to the constructor.
  Serial.begin(9600);
  Serial.println("Starting...");
  lcd.begin(16,2);
//  Wire.begin(D2, D1);
  lcd.init();

  // Turn on the backlight.
  lcd.backlight();

  printStartMessage();
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
}

void showVolumeAndStatus(double totalVolume, boolean isOpen) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Volume: ");
  if(totalVolume < 1000) {
    lcd.print(totalVolume);
    lcd.print("ml");

  } else {
    lcd.print(totalVolume/1000);
    lcd.print("l");
  }
  
  lcd.setCursor(0, 1);      
  lcd.print("Valve ");
  if(isOpen) {
    lcd.print("Open");
  } else {
    lcd.print("Closed");
  }
  
}
