// avec deux boutons à la place du rotary encoder
#include <Arduino.h>
#include <U8g2lib.h> //libraire ecran

#define MAXTEMP0 80
#define MINTEMP0 0
#define MAXTEMP1 200

//I/O Pin setup
#define T0_PIN A13
#define T1_PIN A14
#define BUTTON_PLUS 2
#define BUTTON_MINUS 3
#define HEATER_PIN 10
#define FAN1_PIN 7
#define FAN2_PIN 9

//U8g display definition
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
char temp0[10];
char target0[10];
char target1[10];

//Temperature variables
int rawTemp0,currentTemp0, targetTemp0; //Chamber temperature
int rawTemp1,currentTemp1, targetTemp1; //Resistor temperature


int supTarget0 = 22; // variable pour selection de température
int lowTarget0 = 18; // variable pour selection de température
int supTarget1 = 240; // variable pour selection de température
int lowTarget1 = 180; // variable pour selection de température

//int counter, state, lastState;

// ATC Semitec 204GT-2 (4.7k pullup) Dagoma.Fr - MKS_Base_DKU001327 - version (measured/tested/approved)
#define temp0TableSize 33
int temp0Table[temp0TableSize][temp0TableSize]  = {
  {    1, 713 },
  {   17, 284 },
  {   20, 275 },
  {   23, 267 },
  {   27, 257 },
  {   31, 250 },
  {   37, 240 },
  {   43, 232 },
  {   51, 222 },
  {   61, 213 },
  {   73, 204 },
  {   87, 195 },
  {  106, 185 },
  {  128, 175 },
  {  155, 166 },
  {  189, 156 },
  {  230, 146 },
  {  278, 137 },
  {  336, 127 },
  {  402, 117 },
  {  476, 107 },
  {  554,  97 },
  {  635,  87 },
  {  713,  78 },
  {  784,  68 },
  {  846,  58 },
  {  897,  49 },
  {  937,  39 },
  {  966,  30 },
  {  986,  20 },
  { 1000,  10 },
  { 1010,   0 },
  { 1024,-273 } // for safety
};

int interpolate0(int i){
  int deltaByte,deltaTemp,result;

  deltaByte=temp0Table[i][0]-temp0Table[i-1][0]; 
  deltaTemp=temp0Table[i][1]-temp0Table[i-1][1];

  result= (deltaTemp*(rawTemp0-temp0Table[i-1][0])) / deltaByte +temp0Table[i-1][1];

  return result;
}

int processTemp0(){
  for(int i=1;i<temp0TableSize;i++){
    if(rawTemp0==temp0Table[i][0])return temp0Table[i][1];
    else if(rawTemp0<temp0Table[i][0]&&rawTemp0>temp0Table[i-1][0])
       return interpolate0(i);
  }
  return -273;
}

int interpolate1(int i){
  int deltaByte,deltaTemp,result;

  deltaByte=temp0Table[i][0]-temp0Table[i-1][0]; 
  deltaTemp=temp0Table[i][1]-temp0Table[i-1][1];

  result= (deltaTemp*(rawTemp1-temp0Table[i-1][0])) / deltaByte +temp0Table[i-1][1];

  return result;
}

int processTemp1(){
  for(int i=1;i<temp0TableSize;i++){
    if(rawTemp1==temp0Table[i][0])return temp0Table[i][1];
    else if(rawTemp1<temp0Table[i][0]&&rawTemp1>temp0Table[i-1][0])
       return interpolate1(i);
  }
  return -273;
}



void setup() {

  //Enable serial communication
  Serial.begin(115200);
  Serial.println( MSG_BOOTING );


  //I/O setup
  pinMode(BUTTON_PLUS, INPUT_PULLUP);
  pinMode(BUTTON_MINUS, INPUT_PULLUP);
  pinMode(T0_PIN, INPUT);
  pinMode(HEATER_PIN,OUTPUT);
  pinMode(FAN1_PIN,OUTPUT);
  pinMode(FAN2_PIN,OUTPUT);

  digitalWrite(FAN1_PIN,HIGH);
  digitalWrite(FAN2_PIN,HIGH);

  //Enable display
  u8g2.begin(); 
  
  Serial.println( MSG_READY );
      do {
    u8g2.setFont(u8g2_font_ncenB12_tr);
      u8g2.drawStr(12, 35,"Dagoma 3D");
  } while ( u8g2.nextPage() );
  delay(2000);

}

void loop() {
  //Start display
  u8g2.firstPage();

  //Reading I/O pins
  boolean plusState = digitalRead(BUTTON_PLUS);
  boolean minusState = digitalRead(BUTTON_MINUS);
  rawTemp0=analogRead(T0_PIN);
  rawTemp1=analogRead(T1_PIN);

  //Proccessing values
  currentTemp0=processTemp0(); //Update temperature value (in degree Celsius)
  currentTemp1=processTemp1();


  if ((!plusState) && minusState){ //incrémentation valeur température souhaitée 
    if(targetTemp0<MAXTEMP0){
      supTarget0+=5;
      lowTarget0+=5;
      targetTemp0+=5;
      delay(150);
    }
  }
  if (!minusState){//décrémentation valeur température souhaitée
    if(targetTemp0>MINTEMP0){
      supTarget0-=5;
      lowTarget0-=5;
      targetTemp0-=5;
      delay(150);
    }
  }
  


  //Temp regulation
  if(currentTemp1<MAXTEMP1){
    if (currentTemp0 <= (targetTemp0-5)){ // déclenchement pour une température inférieure à b, avec b = temperature choisie - 5°C
      digitalWrite(HEATER_PIN, HIGH);
    }
    else if (currentTemp0 >= (targetTemp0+5)){ //arrêt pour une température supérieure ou égale à a avec a = température choisie + 5°C
      digitalWrite(HEATER_PIN, LOW);
    }
  }
  else{
    digitalWrite(HEATER_PIN, LOW);
  }

  //Display

  do {
    u8g2.setFont(u8g2_font_ncenB12_tr);// affichage sur écran lcd humidité, température et température demandé 
    
    
    //Current Temperature
    sprintf(temp0, "%d",currentTemp0);
    u8g2.drawStr(5, 40, temp0);
    u8g2.drawStr(25, 30,".");
    u8g2.drawStr(30, 40,"C");
    
    u8g2.drawStr(45, 40,"/");

    //Target Temp
    sprintf(target0, "%d",targetTemp0);
    u8g2.drawStr(55, 40, target0);
    u8g2.drawStr(75, 30,".");
    u8g2.drawStr(80, 40,"C");
    
    
    //Resistor Temp
    sprintf(target1, "%d",currentTemp1);
    u8g2.drawStr(5, 60, target1);
    u8g2.drawStr(45, 50,".");
    u8g2.drawStr(50, 60,"C");
    u8g2.drawStr(60, 60," Res");

  } while ( u8g2.nextPage() );

  Serial.println(targetTemp0);
  delay(50);
}