/* Test code for steppers using acceleration */

#include <AccelStepper.h>

const int STEPPER1_STEP = 2;
const int STEPPER1_DIR = 3;
const int LIMIT_SWITCH_1 = 4;

AccelStepper stepper1(1, STEPPER1_STEP, STEPPER1_DIR);

void setup() {
  pinMode(LIMIT_SWITCH_1,INPUT_PULLUP);

  Serial.begin(9600);

  stepper1.setMaxSpeed(1000); //step/sec
  stepper1.setAcceleration(6000);
 

  // Homing Sequence
  while (digitalRead(LIMIT_SWITCH_1) == LOW)
  {
    stepper1.setSpeed(500);
    stepper1.runSpeed();
  }
  stepper1.setCurrentPosition(0);
  delay(2000);

  // Move to starting position
  stepper1.moveTo(-500);
  stepper1.runToPosition();

  delay(2000);
}

void loop() {
  
  stepperMove();
  
}

void stepperMove()
{
  stepper1.moveTo(-150);
  stepper1.runToPosition();
  delay(100);

  stepper1.moveTo(-800);
  stepper1.runToPosition();
  delay(100);
}
