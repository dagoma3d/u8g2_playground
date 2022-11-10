// avec deux boutons à la place du rotary encoder

#include <U8g2lib.h> //libraire ecran
#include "DHT.h"

#define LCD_PINS_RS 16
#define LCD_PINS_ENABLE 17
#define LCD_PINS_D4 23
#define BTN_ENC 35
#define BEEPER_PIN 37
#define BTN_EN1 31
#define BTN_EN2 33
#define DHTTYPE DHT11   
#define DHTPIN 11 // pin branchemebt du capteur 
#define MSG_BOOTING "Booting up..."
#define MSG_READY "Ready"

int currentTemp, targetTemp;
char temp[10];
char hum[10];
char val[10];

int counter, state, lastState;

int relais  = 10; // pin du relais (chauffe)
int pinBouton1 = A13; // Bouton poussoir 1 branché en pul up sur le pin A13 (diminuer)
int pinBouton2 = A14; // Bouton poussoir 2 branché en pul up sur le pin A14 (augmenter)
int a = 22; // variable pour selection de température
int b = 18; // variable pour selection de température
int ventilateur = 7; //pin de ventilateur 

U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0, LCD_PINS_D4, LCD_PINS_ENABLE, LCD_PINS_RS, BTN_ENC);

DHT dht(DHTPIN, DHTTYPE);


void setup() {
  Serial.begin(9600);
  Serial.println( MSG_BOOTING );
  dht.begin();
  u8g2.begin(); 

  pinMode(BTN_EN1, INPUT);
  pinMode(BTN_EN2, INPUT);
  pinMode(BTN_ENC, INPUT);
  
  pinMode(relais, OUTPUT);
  pinMode(pinBouton1, INPUT_PULLUP);
  pinMode(pinBouton2, INPUT_PULLUP);
  pinMode(ventilateur, OUTPUT);
  digitalWrite(ventilateur, HIGH);

  lastState = digitalRead(BTN_EN2);

  Serial.println( MSG_READY );
      do {
    u8g2.setFont(u8g2_font_ncenB12_tr);
      u8g2.drawStr(12, 35,"Dagoma 3D");
  } while ( u8g2.nextPage() );
  delay(2000);
}


void loop() {

  u8g2.firstPage();

  int h = dht.readHumidity();
  int t = dht.readTemperature();
  int valeur = ((a+b)/2);// valeur de température à régler avec les 2 boutons

  boolean etatBouton1 = digitalRead(pinBouton1);
  boolean etatBouton2 = digitalRead(pinBouton2);
  
  float hic = dht.computeHeatIndex(t, h, false);
  // Affichage dans le moniteur série pour vérification 
  do {
    u8g2.setFont(u8g2_font_ncenB12_tr);// affichage sur écran lcd humidité, température et température demandé 
      u8g2.drawStr(25, 20,"%");
      u8g2.drawStr(30, 40,"C");
      u8g2.drawStr(25, 30,".");
      u8g2.drawStr(45, 40,"/");
      u8g2.drawStr(80, 40,"C");
      u8g2.drawStr(75, 30,".");
    sprintf(hum, "%d",h);
      u8g2.drawStr(5, 20, hum);
    sprintf(temp, "%d",t);
    u8g2.drawStr(5, 40, temp);
    sprintf(val, "%d",valeur);
    u8g2.drawStr(55, 40, val);
  } while ( u8g2.nextPage() );
 
  Serial.print(F("\nHumidity: "));// affichage dans moniteur série humidité, température et température demandé 
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C \n"));
  if (a>=95) {a=95 ; b=85;}
  if (a<=20) {a=20 ; b=10;}
 
  if (etatBouton1==HIGH){ //incrémentation valeur température souhaitée 
    a=a+5;
    b=b+5;
  }
  if (etatBouton2==HIGH){//décrémentation valeur température souhaitée
    a=a-5;
    b=b-5;
  }

  Serial.print(F("Vous avez choisi:"));
  Serial.print(((a+b)/2)); // affiche la température choisi (qui correspond à la moyenne de a et b)
  Serial.print(F(" °C"));
  if (t <= b){ // déclenchement pour une température inférieure à b, avec b = temperature choisie - 5°C
    digitalWrite(relais, HIGH);

  }
  else if (t >= a){ //arrêt pour une température supérieure ou égale à a avec a = température choisie + 5°C
    digitalWrite(relais, LOW);
    //delay(1000); //tempo minimale entre chaque réactivation
  }
  delay(180);
}