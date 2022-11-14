#include <Arduino.h>
#include <U8g2lib.h>

#define LCD_PINS_RS 16
#define LCD_PINS_ENABLE 17
#define LCD_PINS_D4 23
#define BEEPER_PIN 37
#define BTN_EN1 31
#define BTN_EN2 33
#define BTN_ENC 35

#define MSG_BOOTING "Booting up..."
#define MSG_READY "Ready"

#define SERIAL_LOG false

U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0, LCD_PINS_D4, LCD_PINS_ENABLE, LCD_PINS_RS/*, BTN_ENC*/);

int currentTemp, targetTemp, humidity;
char message[25];

int counter, state, lastState;

void setup() {
  // Serial Monitor
  #if SERIAL_LOG
    Serial.begin(115200);
    Serial.println(MSG_BOOTING);
  #endif

  // Screen init
  u8g2.begin();
  u8g2.setFont(u8g2_font_6x13_te); // 9 pixels height font

  // Init temps
  currentTemp = 20;
  targetTemp = currentTemp;
  humidity = 50;
  counter = 0;

  // Rotary encoder
  pinMode(BTN_EN1, INPUT_PULLUP);
  pinMode(BTN_EN2, INPUT_PULLUP);
  pinMode(BTN_ENC, INPUT_PULLUP);

  // Beeper
  //pinMode(BEEPER_PIN, OUTPUT);
  //digitalWrite(BEEPER_PIN, HIGH);

  lastState = digitalRead(BTN_EN1);

  /*
  Example code with timer interrupt that will create an interruption each 1ms using timer1 and prescalar of 256.
  Calculations (for 1ms): 
  System clock 16 Mhz and Prescalar 256;
  Timer 1 speed = 16Mhz/256 = 62.5 Khz    
  Pulse time = 1/62.5 Khz =  16us  
  Count up to = 1ms / 16us = 62.5 (so this is the value the OCR register should have)
  */

  cli();                      //stop interrupts for till we make the settings
  /*1. First we reset the control register to amke sure we start with everything disabled.*/
  TCCR1A = 0;                 // Reset entire TCCR1A to 0 
  TCCR1B = 0;                 // Reset entire TCCR1B to 0

  /*2. We set the prescalar to the desired value by changing the CS10 CS12 and CS12 bits. */  
  TCCR1B |= B00000100;        //Set CS12 to 1 so we get prescalar 256  

  /*3. We enable compare match mode on register A*/
  TIMSK1 |= B00000010;        //Set OCIE1A to 1 so we enable compare match A 

  /*4. Set the value of register A to 62*/
  OCR1A = 62;                //Finally we set compare register A to this value  
  sei();                     //Enable back the interrupts

  Serial.println( MSG_READY );
}

// Rotary encoder
void rotaryEncoder() {
  state = digitalRead(BTN_EN1);
  if(state != lastState) {
    if(digitalRead(BTN_EN2) != state) {
      targetTemp--;
    } else {
      targetTemp++;
    }
    #if SERIAL_LOG
      Serial.print("Target temperature: ");
      Serial.println(targetTemp);
    #endif
  }
  lastState = state;
}

// Screen display
void display() {
  u8g2.firstPage();
  do {
      sprintf(message, "Current temp: %d \xb0\x43", currentTemp);
      u8g2.drawStr(4, 13, message);
      sprintf(message, "Target temp : %d \xb0\x43", targetTemp);
      u8g2.drawStr(4, 24, message);
      sprintf(message, "Humidity    : %d %%", humidity);
      u8g2.drawStr(4, 35, message);
  } while ( u8g2.nextPage() );
}

void loop() {
  if(!digitalRead(BTN_ENC)) currentTemp = targetTemp;
  rotaryEncoder();
  //display();
}

//With the settings above, this IRS will trigger each 500ms.
ISR(TIMER1_COMPA_vect) {
  display();
}
