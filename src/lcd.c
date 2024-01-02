#include "lcdpcf8574.h"

void displayVolume(unsigned long volume) {
	lcd_gotoxy(1, 0);
	char tmp[21];
	lcd_puts("Vol:");
	lcd_gotoxy(5, 0);
	ultoa(volume, tmp, 10);
	lcd_puts(tmp);
	lcd_gotoxy(1, 1);
	lcd_puts("Ticks:");
	lcd_gotoxy(7, 1);
	utoa(oldTicks, tmp, 10);
	lcd_puts(tmp);
}

void setupLCD(void) {

	char tmp = LCD_DISP_ON_BLINK;
	lcd_init(LCD_DISP_ON_BLINK);

	//lcd go home
	lcd_home();

	lcd_led(0); //turn on LED
	int line = 0;
	lcd_gotoxy(1, line);
	lcd_puts("Starting");
}


