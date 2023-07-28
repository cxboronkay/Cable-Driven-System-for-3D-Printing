//Controls one stepper motor using encoder input

// Include encoder and AccelStepper libraries:
#include <AS5600.h>
#include <AccelStepper.h>

// Initialize encoder and position variables:
AS5600 encoder;
int speed;
int requestedPosition = 360;  // in degrees
int requestedLength = 100; // in millimeters
float currentPosition;
float currentLength;
float startLength = 0;
float pulleyRadius = 26.25; //in mm
float positionDiff;
float lengthDiff;
float output;
float lastOutput;
float initPosition;
float position;
float revolutions = 0;

// Define stepper motor connections and motor interface type (1 when using driver):
#define dirPin 5
#define stepPin 2
#define motorInterfaceType 1

// Create new instance of AccelStepper class:
AccelStepper stepper = AccelStepper(motorInterfaceType, stepPin, dirPin);

// Store time:
unsigned long previousMillis = 0;

// Store constant interval at which to read encoder and step:
const long interval = 50;   //in milliseconds


//------------------------------------------------------------------------------------------------
void setup() {
  // Begin serial communication:
  Serial.begin(9600);
    
  // Set the maximum speed in steps per second:
  stepper.setMaxSpeed(2000);

  // Find initial encoder values for zeroing
  output = encoder.getPosition();
  initPosition = output;
  lastOutput = output;
  position = output;
}

//------------------------------------------------------------------------------------------------
void loop() {

  // check if it is time to read encoder
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval){
    // save the last time we read encoder
    previousMillis = currentMillis;
  
    
    // Monitor encoder position
    output = encoder.getPosition();           // get the raw value of the encoder
     
    if ((lastOutput - output) > 2047 )        // check if a full rotation has been made
      revolutions++;
    if ((lastOutput - output) < -2047 )
      revolutions--;
    
    currentPosition = (revolutions * 4096 + output - initPosition)* 0.087890625;   // calculate the position *IN DEGREES* the the encoder is at based off of the number of revolutions
    currentLength = startLength + (currentPosition/360)*2*M_PI*pulleyRadius;

    Serial.println("Length:");
    Serial.println(currentLength);
    Serial.println("Position:");
    Serial.println(currentPosition);
    
    lastOutput = output;                      // save the last raw value for the next loop 
    
    // Set the speed in steps per second:
    speed = 1000;
    stepper.setSpeed(speed);
    // Run stepper until position reached
    positionDiff = currentPosition - requestedPosition;   // check rotation
    lengthDiff = currentLength - requestedLength;      // check length
//    if (lengthDiff < 0) {
//      stepper.setSpeed(-speed);
//    }
// 
//    if (currentLength < (requestedLength + 1) && currentLength > (requestedLength - 1)){    //stop if position is within requestedPosition bounds
//      stepper.setSpeed(0);
//    }
    
    if (positionDiff < 0) {
      stepper.setSpeed(-speed);
    }
 
    if (currentPosition < (requestedPosition + 1) && currentPosition > (requestedPosition - 1)){    //stop if position is within requestedPosition bounds
      stepper.setSpeed(0);
    }
    Serial.println("Current speed:");
    Serial.println(speed);
    stepper.runSpeed(); //run stepper at set speed
  }
}
