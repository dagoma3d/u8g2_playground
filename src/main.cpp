#include <Arduino.h>
#include <U8g2lib.h>

#define LCD_PINS_RS 16
#define LCD_PINS_ENABLE 17
#define LCD_PINS_D4 23
#define BTN_ENC 35

#define MSG_BOOTING "Booting up..."
#define MSG_READY "Ready"


U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0, LCD_PINS_D4, LCD_PINS_ENABLE, LCD_PINS_RS, BTN_ENC);

void setup() {
	// Serial Monitor
	Serial.begin( 115200 );
	Serial.println( MSG_BOOTING );

	// Screen init
	u8g2.begin();

	Serial.println( MSG_READY );
}

void loop() {
	// Screen display
	u8g2.firstPage();
	do {
		u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.drawStr(0,20,"Hello World!");
	} while ( u8g2.nextPage() );
}
