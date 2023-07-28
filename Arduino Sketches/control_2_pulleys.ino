//Control 2 pulleys simultaneously. Takes desired user input location (in length of cables) and moves to spot before asking for new location.

// Include I2C, encoder, math, and AccelStepper libraries:
#include <Wire.h>
#include <AS5600.h>
#include <math.h>
#include <AccelStepper.h>

// Create encoder instances:
AS5600 encoder0;
AS5600 encoder1;

// Define I2C address:
#define TCAADDR 0x70
// Initialize variables for position monitoring:
int speed0;
void tcaselect (uint8_t i) {
  if (i > 7) return;

  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

int speed1;
int requestedLocation[2]; // in millimeters
float currentPosition[2];
float currentLocation[2];
float startLength[2] = {0, 0};
float pulleyRadius = 26.25; //in mm
float positionDiff[2];
float locationDiff[2];
float output[2];
float lastOutput[2];
float initPosition[2];
float position[2];
float revolutions[2] = {0,0};
float printLength[2];

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


//------------------------------------------------------------------------------------------------
void setup() {
  // Begin serial communication:
  Serial.begin(115200);
  
  // Set the maximum speed in steps per second:
  stepper0.setMaxSpeed(2000);
  stepper1.setMaxSpeed(2000);
  
  // Find initial encoder values for zeroing
  tcaselect(0);
  output[0] = encoder0.getPosition();
  initPosition[0] = output[0];
  lastOutput[0] = output[0];
  position[0] = output[0];

  tcaselect(1);
  output[1] = encoder1.getPosition();
  initPosition[1] = output[1];
  lastOutput[1] = output[1];
  position[1] = output[1];
}

//------------------------------------------------------------------------------------------------
void loop() {
  Serial.println("Enter desired location 1 in millimeters:");
  while (Serial.available() == 0){
  }
  requestedLocation[0] = Serial.parseFloat();
  Serial.read();
  boolean flag0 = false;
  Serial.println("Enter desired location 2 in millimeters:");
  while (Serial.available() == 0){
  }
  requestedLocation[1] = Serial.parseFloat();
  Serial.read();
  boolean flag1 = false;


  while (flag0 == false || flag1 == false){  
    // Monitor encoder position
    tcaselect(0);                             // get the raw values of the encoders
    output[0] = encoder0.getPosition();           
    tcaselect(1);
    output[1] = encoder1.getPosition();

    if ((lastOutput[0] - output[0]) > 2047 )       // check if a full rotation has been made
      revolutions[0]++;
    if ((lastOutput[0] - output[0]) < -2047 )
      revolutions[0]--;
    if ((lastOutput[1] - output[1]) > 2047 )        
      revolutions[1]++;
    if ((lastOutput[1] - output[1]) < -2047 )
      revolutions[1]--;

    currentPosition[0] = (revolutions[0] * 4096 + output[0] - initPosition[0])* 0.087890625;   // calculate the position *IN DEGREES* the the encoder is at based off of the number of revolutions
    currentLocation[0] = startLength[0] + (currentPosition[0]/360)*2*M_PI*pulleyRadius;
    printLength[0] = -currentLocation[0];
    currentPosition[1] = (revolutions[1] * 4096 + output[1] - initPosition[1])* 0.087890625;   // calculate the position *IN DEGREES* the the encoder is at based off of the number of revolutions
    currentLocation[1] = startLength[1] + (currentPosition[1]/360)*2*M_PI*pulleyRadius;
    printLength[1] = -currentLocation[1];
    
    Serial.println("Location:");
    Serial.println(printLength[0]);
    Serial.println(printLength[1]);

    
    lastOutput[0] = output[0];                      // save the last raw value for the next loop 
    lastOutput[1] = output[1];
    
    // Set the speed in steps per second:
    speed0 = -1000;
    speed1 = -1000;
    stepper0.setSpeed(speed0);
    stepper1.setSpeed(speed1);
    // Run steppers until position reached
    locationDiff[0] = -currentLocation[0] - requestedLocation[0];      // check length
    if (locationDiff[0] < 0) {
      speed0 = -speed0;
      stepper0.setSpeed(speed0);
    }
    locationDiff[1] = -currentLocation[1] - requestedLocation[1];      
    if (locationDiff[1] < 0) {
      speed1 = -speed1;
      stepper1.setSpeed(speed1);
    }
    
    if (-currentLocation[0] < (requestedLocation[0] + 1) && -currentLocation[0] > (requestedLocation[0] - 1)){    //stop if position is within requestedPosition bounds
      speed0 = 0;
      stepper0.setSpeed(speed0);
      flag0 = true;
    }
    if (-currentLocation[1] < (requestedLocation[1] + 1) && -currentLocation[1] > (requestedLocation[1] - 1)){    
      speed1 = 0;
      stepper1.setSpeed(speed1);
      flag1 = true;
    }    
    stepper0.runSpeed(); //run stepper at set speed
    stepper1.runSpeed();
    Serial.println("Speed0:");
    Serial.println(speed0);
    Serial.println("Speed1:");
    Serial.println(speed1);
    
  }
}
