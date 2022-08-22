#include <AccelStepper.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include<MultiStepper.h>


//constants
const int IR_PIN = 2;

const unsigned long MAKE_INTERRUPT_DELAY = 1000000;

const int STEPPER_X_STEP = 3, STEPPER_X_DIR = 4;
const int STEPPER_Z_STEP = 5, STEPPER_Z_DIR = 6;

const int LIMIT_SWITCH_X = 7;
const int LIMIT_SWITCH_Z = 8;

// Define LCD pinout
const int  en = 2, rw = 1, rs = 0, d4 = 4, d5 = 5, d6 = 6, d7 = 7, bl = 3;

const int I2C_ADDRESS = 0x27;

const int LEVEL_1_MAX_TIME = 20000, LEVEL_2_MAX_TIME = 30000, LEVEL_3_MAX_TIME = 30000, LEVEL_4_MAX_TIME = 30000, LEVEL_5_MAX_TIME = 30000;  
const int HOMING_STATE = 0, LEVEL_1_STATE = 1, LEVEL_2_STATE = 2,LEVEL_3_STATE = 3, LEVEL_4_STATE = 4, LEVEL_5_STATE = 5, GAME_OVER_STATE = 6;


//objects
AccelStepper stepperX(1, STEPPER_X_STEP, STEPPER_X_DIR);
AccelStepper stepperZ(1, STEPPER_Z_STEP, STEPPER_Z_DIR);
LiquidCrystal_I2C lcd(0x27, 20,4); // need to change to I2C later

//global variables
int currentLevel = 0;
volatile int makes = 0;
volatile int levelTime = 0;
volatile bool canCount = true;
unsigned long firstMakeInterruptTime = 0.0;

void setup() {
 
  Serial.begin(9600);
  
  stepperX.setMaxSpeed(1000);
  stepperZ.setMaxSpeed(1000); 
  
  pinMode(LIMIT_SWITCH_X, INPUT_PULLUP);
  pinMode(LIMIT_SWITCH_Z, INPUT_PULLUP);
  
  pinMode(IR_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(IR_PIN), ISR_makes, FALLING);

 
  lcd.init();
  lcd.backlight();
  lcd.print("Current Level: ");
  lcd.setCursor(0,1);
  lcd.print("Makes: /5");
  lcd.setCursor(0,2);
  lcd.print("Time Remaining: ");
}


void loop() {

  switch(currentLevel)
  {
    case HOMING_STATE:
      currentLevel = homingRoutine();
      break;

    case LEVEL_1_STATE:
      currentLevel = level_1(); 
      break;

    case LEVEL_2_STATE:
      currentLevel = level_2();
      break;

    case GAME_OVER_STATE:
      currentLevel = game_over();
      break;
    
  }

}
/*
ISR(TIMER1_COMPA_vect) {
  TCNT1 = t1_load; // reset timer 1 to 0
  
}
*/
void ISR_makes()
{
  if (micros() >= (firstMakeInterruptTime + MAKE_INTERRUPT_DELAY))
  {
    canCount = true;
    firstMakeInterruptTime = micros();
  }
  if (canCount)
  {
    firstMakeInterruptTime = micros();
    makes++;
    canCount = false;
  }

  Serial.println(makes);
  
}

int homingRoutine()
{
  //moving to switch
  lcd.setCursor(0,3);
  lcd.print("homing...");
  
  while (digitalRead(LIMIT_SWITCH_X) == LOW)
  {
    stepperX.setSpeed(-400);
    stepperX.runSpeed();
  }
  delay(2000);

  //moving till switch is not triggered
  while (digitalRead(LIMIT_SWITCH_X) == HIGH)
  {
    stepperX.setSpeed(200);
    stepperX.runSpeed();
  }

  stepperX.setCurrentPosition(0);
  delay(1000);

  while (digitalRead(LIMIT_SWITCH_Z) == LOW)
  {
    stepperZ.setSpeed(400);
    stepperZ.runSpeed();
  }
  delay(1000);

  while (digitalRead(LIMIT_SWITCH_Z) == HIGH)
  {
    stepperZ.setSpeed(-200);
    stepperZ.runSpeed();
  }
  delay(1000);
  stepperZ.setCurrentPosition(0);

  lcd.setCursor(0,3);
  lcd.print("               ");
  return LEVEL_1_STATE;
}

int level_1()
{
  unsigned long levelStartTime = millis();
  unsigned long currentLevelTime = levelStartTime;

  lcd.setCursor(15,0); //need to update
  lcd.print(currentLevel);
  
  stepperX.setSpeed(500);
  stepperX.setAcceleration(1000);
  stepperX.moveTo(500); // get constants 
  stepperX.runToPosition(); // whats the difference between this and .runToNewPosition?
 // stepperZ.setAcceleration(1000);

  //z-axis stays home for level 1

  //checking if time for level has run out 
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
      lcdUpdate(levelStartTime, currentLevelTime, LEVEL_1_MAX_TIME);
    }
  }
  
  makes = 0;
  return GAME_OVER_STATE;
}

void lcdUpdate(unsigned long levelStartTime, unsigned long currentLevelTime, unsigned long levelMaxTime)
{
  lcd.setCursor(6,1);
  lcd.print(makes);
 
  int displayTimeElapsed = 0;
  displayTimeElapsed = (levelMaxTime - (currentLevelTime - levelStartTime)) / 1000;

  if (displayTimeElapsed < 10)
  {
    lcd.setCursor(17, 2);
    lcd.print(" ");
  }
  lcd.setCursor(16,2);
  lcd.print(displayTimeElapsed);
  
}
int level_2()
{
  unsigned long levelStartTime = millis();
  unsigned long currentLevelTime = levelStartTime;

  lcd.setCursor(15,0); //need to update
  lcd.print(currentLevel);
  
  stepperZ.setAcceleration(1000);
  stepperZ.moveTo(-700); //start point
  stepperZ.runToPosition();
  delay(2000);

  int nextPositionZ = -400;
  
  while (millis() <= (levelStartTime + LEVEL_2_MAX_TIME))
  {
    if (stepperZ.currentPosition() >= -400)
    {
      nextPositionZ = -900;
    }
    else if (stepperZ.currentPosition() <= -900)
    {
      nextPositionZ = -400;
    }
    
    stepperZ.moveTo(nextPositionZ);
    stepperZ.run();

    if (makes >= 5) 
    {
      makes = 0;
      return 0;
      
    }

    if ((millis() - currentLevelTime) >= 1000)
    {
      currentLevelTime = millis();
      lcdUpdate(levelStartTime, currentLevelTime, LEVEL_2_MAX_TIME);
    }
  }
  
  makes = 0;
  return GAME_OVER_STATE;
}

int game_over()
{
  static bool lcdCleared = false;
  if(!lcdCleared)
  {
    lcd.clear();
    lcdCleared = true;
  }
  lcd.setCursor(0,0);
  lcd.print("GAME OVER");
  
  return GAME_OVER_STATE;
}
