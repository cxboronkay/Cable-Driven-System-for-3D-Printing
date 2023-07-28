//Simple functional test for 2 motors

//Include AccelStepper libary:
#include <AccelStepper.h>

// Define stepper motor connections and motor interface type (1 when using driver):
#define dirPin0 3
#define stepPin0 2
#define motorInterfaceType0 1

#define dirPin1 5
#define stepPin1 4
#define motorInterfaceType1 1

// Create new instances of AccelStepper class:
AccelStepper stepper0 = AccelStepper(motorInterfaceType0, stepPin0, dirPin0);
AccelStepper stepper1 = AccelStepper(motorInterfaceType1, stepPin1, dirPin1);

int speed;

void setup() {
  // Begin serial communication
  Serial.begin(9600);

  // Set the maximum speed in steps per second:
  stepper0.setMaxSpeed(2000);
  stepper1.setMaxSpeed(2000);
  
}


void loop() { 
  speed = 200;
  stepper0.setSpeed(-speed);
  stepper1.setSpeed(-speed);

  stepper0.runSpeed();
  stepper1.runSpeed();
  
}
