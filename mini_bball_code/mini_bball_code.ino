#include <AccelStepper.h>
#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal.h>
#include<MultiStepper.h>

//constants
const int IR_PIN = 2;

const int STEPPER_X_STEP = 3;
const int STEPPER_X_DIR = 4;
const int STEPPER_Z_STEP = 5;
const int STEPPER_Z_DIR = 6;

const int LIMIT_SWITCH_X = 7;
const int LIMIT_SWITCH_Z = 8;

const int RS_PIN = 9;
const int EN_PIN = 10;
const int RD4_PIN = 11;
const int RD5_PIN = 12;
const int RD6_PIN = 13;
const int RD7_PIN = 14;

const int GAME_OVER_STATE = 6;
const int LEVEL_1_MAX_TIME = 60000;  
const int LEVEL_1_STATE = 1, LEVEL_2_STATE = 2;

//objects
AccelStepper stepperX(1, STEPPER_X_STEP, STEPPER_X_DIR);
AccelStepper stepperZ(1, STEPPER_Z_STEP, STEPPER_Z_DIR);
LiquidCrystal lcd(RS_PIN, EN_PIN, RD4_PIN, RD5_PIN, RD6_PIN, RD7_PIN); // need to change to I2C later

//global variables
int currentLevel = 0;
volatile int makes = 0;

void setup() {
 
  Serial.begin(9600);
  
  stepperX.setMaxSpeed(1000);
  stepperZ.setMaxSpeed(1000); 
  
  pinMode(LIMIT_SWITCH_X, INPUT_PULLUP);
  pinMode(LIMIT_SWITCH_Z, INPUT_PULLUP);
  
  pinMode(IR_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(IR_PIN), ISR_makes, FALLING);
  
  lcd.begin(16,2); // to be changed to (20,4)

}


void loop() {

  switch(currentLevel)
  {
    case 0:
      currentLevel = homingRoutine();
      break;

    case 1:
      currentLevel = level_1(); 
      break;

    case 2:
    //  level_2();
      break;
    
  }

}


void ISR_makes()
{
  makes++;
}

int homingRoutine()
{
  //moving to switch
  while (digitalRead(LIMIT_SWITCH_X) == LOW)
  {
    stepperX.setSpeed(-400);
    stepperX.runSpeed();
  }
  delay(1000);

  //moving till switch is not triggered
  while (digitalRead(LIMIT_SWITCH_X) == HIGH)
  {
    stepperX.setSpeed(200);
    stepperX.runSpeed();
  }

  stepperX.setCurrentPosition(0);

  while (digitalRead(LIMIT_SWITCH_Z) == LOW)
  {
    stepperZ.setSpeed(400);
    stepperZ.runSpeed();
  }
  delay(1000);

  while (digitalRead(LIMIT_SWITCH_Z) == HIGH)
  {
    stepperZ.setSpeed(200);
    stepperZ.runSpeed();
  }

  stepperZ.setCurrentPosition(0);

  
  return LEVEL_1_STATE;
}

int level_1()
{
  unsigned long levelStartTime = millis();
  unsigned long currentLevelTime = levelStartTime;

  lcd.setCursor(12,0); //need to update
  lcd.print(currentLevel);
  
  stepperX.setSpeed(500);
  stepperX.setAcceleration(1000);
  stepperX.moveTo(500); // get constants 
  stepperX.runToPosition(); // whats the difference between this and .runToNewPosition?

  //z-axis stays home for level 1

  //checking if 
  while (millis() <= (levelStartTime + LEVEL_1_MAX_TIME))
  {
    if (makes >= 5) 
    {
       
      makes = 0;
      return LEVEL_2_STATE;
      
    }

    if ((millis() - currentLevelTime) >= 1000)
    {
      currentLevelTime = millis();
      lcdUpdate(levelStartTime, currentLevelTime);
    }
  }

  return GAME_OVER_STATE;
}

void lcdUpdate(unsigned long levelStartTime, unsigned long currentLevelTime)
{
  lcd.setCursor(0,2);
  lcd.print(makes);

  int displayTime = 0;
  displayTime = (currentLevelTime - levelStartTime) / 1000;

  lcd.setCursor(10,0);
  lcd.print(displayTime);
  
}
