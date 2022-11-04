#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include <config.h>
#include <pins.h>
#include <messages.h>

U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0, LCD_PINS_D4, LCD_PINS_ENABLE, LCD_PINS_RS, BTN_ENC);

volatile unsigned long nbStopX = 0;
int lastStopXState;
char bStopX[10];

volatile unsigned long nbStopY = 0;
int lastStopYState;
char bStopY[10];

const unsigned long DEBOUNCE_DELAY = 50UL;
unsigned long debounceTimeX = 0UL;
unsigned long debounceTimeY = 0UL;

const unsigned long Y_DELAY = 2000UL;
unsigned long motorYTime;

void setupMotorX() {
	if(ENABLE_X_MOT) {
		// X_MOT init
		pinMode(X_STEP_PIN, OUTPUT);
		pinMode(X_DIR_PIN, OUTPUT);
		pinMode(X_ENABLE_PIN, OUTPUT);
		// X enstop
		pinMode(X_MAX_PIN, INPUT_PULLUP);

		digitalWrite(X_DIR_PIN, LOW);
		digitalWrite(X_ENABLE_PIN, LOW);
	}
}

void setupMotorY() {
	if(ENABLE_Y_MOT) {
		// Y_MOT init
		pinMode(Y_STEP_PIN, OUTPUT);
		pinMode(Y_DIR_PIN, OUTPUT);
		pinMode(Y_ENABLE_PIN, OUTPUT);
		// X enstop
		pinMode(Y_MAX_PIN, INPUT_PULLUP);

		digitalWrite(Y_DIR_PIN, HIGH);
		digitalWrite(Y_ENABLE_PIN, LOW);
	}
}

void countStopX() {
	int reading = digitalRead(X_MAX_PIN);

	if (reading != lastStopXState) debounceTimeX = millis();

	if ((millis() - debounceTimeX) > DEBOUNCE_DELAY) nbStopX++;

	lastStopXState = reading;
}

void countStopY() {
	int reading = digitalRead(Y_MAX_PIN);

	if (reading != lastStopYState) debounceTimeY = millis();

	if ((millis() - debounceTimeY) > DEBOUNCE_DELAY) nbStopY++;

	lastStopYState = reading;
}

void setup() {
	// Serial Monitor
	Serial.begin( 115200 );
	Serial.println( MSG_BOOTING );

	// Screen init
	u8g2.begin();
	u8g2.setFont( u8g2_font_ncenB12_tr );

	// Motors setup
	setupMotorX();
	setupMotorY();

	lastStopXState = digitalRead(X_MAX_PIN);
	lastStopYState = digitalRead(Y_MAX_PIN);

	motorYTime = millis();

	// initialize Timer1
	noInterrupts();          // disable global interrupts
	TCCR1A = 0;     // set entire TCCR1A register to 0
	TCCR1B = 0;     // same for TCCR1B

	// set compare match register to desired timer count:
	OCR1A = 5;

	// turn on CTC mode:
	TCCR1B |= (1 << WGM12);

	// Set CS10 and CS12 bits for 1024 prescaler
	// Set only CS12 bit for 256 prescaler
	TCCR1B |= (1 << CS12);

	// enable timer compare interrupt:
	TIMSK1 |= (1 << OCIE1A);
	interrupts();          // enable global interrupts

	attachInterrupt(digitalPinToInterrupt(X_MAX_PIN), countStopX, CHANGE);
	attachInterrupt(digitalPinToInterrupt(Y_MAX_PIN), countStopY, CHANGE);

	Serial.println( MSG_READY );
}

void loop() {
	unsigned long now = millis();
	if((now - motorYTime) > Y_DELAY) {
		digitalWrite(Y_DIR_PIN, !digitalRead(Y_DIR_PIN));
		motorYTime = millis();
	}
	// Screen display
	u8g2.firstPage();
	do {
		u8g2.drawStr( 0, 13, "Nb StopX :" );
		sprintf (bStopX, "%lu", nbStopX);
		u8g2.drawStr( 0, 30, bStopX );

		u8g2.drawLine( 0, 31, 127, 31 );

		u8g2.drawStr( 0, 45, "Nb Stop Y :" );
		sprintf (bStopY, "%lu", nbStopY);
		u8g2.drawStr( 0, 63, bStopY );
	} while ( u8g2.nextPage() );
}

ISR(TIMER1_COMPA_vect) {
	digitalWrite(X_STEP_PIN, !digitalRead(X_STEP_PIN));
	digitalWrite(Y_STEP_PIN, !digitalRead(Y_STEP_PIN));
}
