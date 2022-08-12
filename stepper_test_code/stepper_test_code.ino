//test code for steppers using acceleration

#include <AccelStepper.h>

const int STEPPER1_STEP = 2;
const int STEPPER1_DIR = 3;
const int LIMIT_SWITCH_1 = 4;

AccelStepper stepper1(1, STEPPER1_STEP, STEPPER1_DIR);

void setup() {

  stepper1.setMaxSpeed(1000);
  pinMode(LIMIT_SWITCH_1,INPUT_PULLUP);

  if(LIMIT_SWITCH_1==HIGH)
    stepper1.setSpeed(0);
  
  //homing
  while (digitalRead(LIMIT_SWITCH_1) == LOW)
  {
    stepper1.setSpeed(-400);
    s
    stepper1.runSpeed();
    stepper1.setCurrentPosition(0);
  }
  delay(2000);
  
  //move to starting position
  while (stepper1.currentPosition() <= 200)
  {
    stepper1.setSpeed(400);
    stepper1.setAcceleration(100);
    stepper1.runSpeed();
  }
  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:

  {
    stepper1.setSpeed(500);
    stepper1.moveTo(200);
    stepper1.runSpeed();
 
 stepper1.setSpeed(200);
  }

 
  
 
}
