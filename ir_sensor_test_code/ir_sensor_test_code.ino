#include <LiquidCrystal.h>

const int RS_PIN = 6;
const int EN_PIN = 7;
const int RD4_PIN = 8;
const int RD5_PIN = 9;
const int RD6_PIN = 10;
const int RD7_PIN = 11;
const int IR_SENSOR = 5;

int makes = 0;
int currentLevel = 1;
  
LiquidCrystal lcd(RS_PIN, EN_PIN, RD4_PIN, RD5_PIN, RD6_PIN, RD7_PIN);

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  pinMode(IR_SENSOR,INPUT);
  //pinMode(13,OUTPUT);//Onboard LED
  lcd.begin(16,2); // setting lcd dimensions
  
  
  lcd.print("Current Level:");
  lcd.print(currentLevel);
  lcd.setCursor(0,1);
  lcd.print("Makes: ");
  lcd.print(makes);
  lcd.print("/5");
 
}

void loop() {
  

  if (digitalRead(IR_SENSOR) == LOW)
  {
    makes++;
    lcd.setCursor(7,1);
    lcd.print(makes);
  }
  delay(500);

  if (makes == 5)
  {
    currentLevel++;
    makes= 0;
    lcd.setCursor(7,1);
    lcd.print(makes);
    lcd.setCursor(14,0);
    lcd.print(currentLevel);
  }

/*
 //this is for testing with arduino on board led
 
  Serial.println(digitalRead(IR_SENSOR));

  if (digitalRead(IR_SENSOR) == HIGH)
  {
    digitalWrite(13,HIGH);
  }

  else 
  {
    digitalWrite(13,LOW);
  }
 
*/

}
