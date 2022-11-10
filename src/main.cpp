#include <Arduino.h>
#include <U8g2lib.h>

#define LCD_PINS_RS 16
#define LCD_PINS_ENABLE 17
#define LCD_PINS_D4 23
#define BTN_ENC 35
#define BEEPER_PIN 37
#define BTN_EN1 31
#define BTN_EN2 33

#define MSG_BOOTING "Booting up..."
#define MSG_READY "Ready"


U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0, LCD_PINS_D4, LCD_PINS_ENABLE, LCD_PINS_RS, BTN_ENC);

int currentTemp, targetTemp;
char temps[10];

int counter, state, lastState;

void setup() {
	// Serial Monitor
	Serial.begin( 115200 );
	Serial.println( MSG_BOOTING );

	// Screen init
	u8g2.begin();

	// Init temps
	currentTemp = 100;
	targetTemp = 120;

	// Rotary encoder
	pinMode(BTN_EN1, INPUT);
	pinMode(BTN_EN2, INPUT);
	pinMode(BTN_ENC, INPUT);

	lastState = digitalRead(BTN_EN1);
	counter = 0;

	// Beeper
	//pinMode(BEEPER_PIN, OUTPUT);
	//digitalWrite(BEEPER_PIN, HIGH);

	Serial.println( MSG_READY );
}

// Rotary encoder
void rotaryEncoder() {
	state = digitalRead(BTN_EN1);
	if(state != lastState) {
		if(digitalRead(BTN_EN2) != state) {
			counter--;
		} else {
			counter++;
		}
		Serial.print("Counter: ");
		Serial.println(counter);
	}
	lastState = state;
}

// Screen display
void display() {
	u8g2.firstPage();
	do {
		u8g2.setFont(u8g2_font_ncenB12_tr);
    	u8g2.drawStr(0, 20,"Temperatures:");
		sprintf(temps, "%d / %d", currentTemp, targetTemp);
    	u8g2.drawStr(0, 40, temps);
	} while ( u8g2.nextPage() );
}

void loop() {
	rotaryEncoder();
	display();
}
