// avec deux boutons à la place du rotary encoder

#include <U8g2lib.h> //libraire ecran
//#include "DHT.h"

#define T0_PIN A13
#define BUTTON_PLUS 14
#define BUTTON_MINUS 15


int rawTemp,currentTemp, targetTemp;

int counter, state, lastState;


void setup() {
  Serial.begin(9600);
  pinMode(T0_PIN, INPUT);
}

void loop() {
  rawTemp=analogRead(T0_PIN);
  Serial.println(rawTemp);
  delay(200);
}