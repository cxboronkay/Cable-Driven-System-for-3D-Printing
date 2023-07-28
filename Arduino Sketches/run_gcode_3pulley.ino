// Takes converted G-Code coordinates and moves through all coordinates then stops.

// Include I2C, encoder, math, and AccelStepper libraries:
#include <Wire.h>
#include <AS5600.h>
#include <math.h>
#include <AccelStepper.h>

// Create encoder instances:
AS5600 encoderA;
AS5600 encoderB;
AS5600 encoderC;

// Define I2C address:
#define TCAADDR 0x70
// Initialize variables for position monitoring:

void tcaselect (uint8_t i) {
  if (i > 7) return;

  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

// Create variables:
int speedA;
int speedB;
int speedC;
int requestedLocation[3]; // in millimeters
float currentPosition[3];
float currentLocation[3];
float startLength[3] = {200, 350, 350}; // need to manually move end effector here before starting Serial!!
float pulleyRadius = 26.25; //in mm
float positionDiff[3];
float locationDiff[3];
float output[3];
float lastOutput[3];
float initPosition[3];
float position[3];
float revolutions[3] = {0,0,0};
float printLength[3];
int counter = 0;
int numcoords = 42;    //number of coordinates being inputted (can get from python program)  **Don't forget to change when changing coordinate list!!**

// Define stepper motor connections and motor interface type (1 when using driver):

#define dirPinA 3
#define stepPinA 2
#define motorInterfaceTypeA 1


#define dirPinB 5
#define stepPinB 4
#define motorInterfaceTypeB 1

#define dirPinC 7
#define stepPinC 6
#define motorInterfaceTypeC 1

// Create new instances of AccelStepper class:
AccelStepper stepperA = AccelStepper(motorInterfaceTypeA, stepPinA, dirPinA); //PULLEY A
AccelStepper stepperB = AccelStepper(motorInterfaceTypeB, stepPinB, dirPinB); //PULLEY B
AccelStepper stepperC = AccelStepper(motorInterfaceTypeC, stepPinC, dirPinC); //PULLEY C

// Copy translated coordinates here:
int coordinate_list[42][3] =         //**Don't forget to adjust first integer for number of coordinates**
 {
{273,435,313},
{265,437,320},
{258,437,328},
{250,434,337},
{247,432,342},
{244,430,347},
{241,427,352},
{239,424,357},
{235,416,366},
{234,412,371},
{233,407,375},
{233,401,381},
{234,392,387},
{236,382,392},
{241,371,397},
{250,358,399},
{256,351,399},
{260,348,398},
{263,345,396},
{267,343,395},
{271,342,393},
{274,341,390},
{278,340,387},
{281,340,384},
{287,342,378},
{290,344,373},
{295,349,365},
{301,361,350},
{303,370,340},
{304,380,330},
{303,389,323},
{303,394,320},
{302,399,316},
{300,404,313},
{299,408,311},
{297,413,309},
{294,417,308},
{292,421,307},
{287,426,307},
{283,430,308},
{276,433,311},
    {200,350,350}         //Can be convenient to add homing coordinate at end of program to return cables to original lengths (Don't forget to add 1 to the numcoords).
  };

//------------------------------------------------------------------------------------------------
void setup() {
  // Begin serial communication:
  Serial.begin(115200);
  
  // Set the maximum speed in steps per second:
  stepperA.setMaxSpeed(2000);
  stepperB.setMaxSpeed(2000);
  stepperC.setMaxSpeed(2000);
  
  // Find initial encoder values for zeroing:
  tcaselect(0);
  output[0] = encoderA.getPosition();
  initPosition[0] = output[0];
  lastOutput[0] = output[0];
  position[0] = output[0];

  tcaselect(1);
  output[1] = encoderB.getPosition();
  initPosition[1] = output[1];
  lastOutput[1] = output[1];
  position[1] = output[1];


  tcaselect(2);
  output[2] = encoderC.getPosition();
  initPosition[2] = output[2];
  lastOutput[2] = output[2];
  position[2] = output[2];

}

//------------------------------------------------------------------------------------------------
void loop() {
  for(int i=0; i<3; i++)  {
    requestedLocation[i] = coordinate_list[counter][i];  
    Serial.println(requestedLocation[i]);
  }
  if (counter == 0){
    delay(10000);         //  remove or adjust if you do not want delay at start
  }
  boolean flagA = false;
  boolean flagB = false;
  boolean flagC = false;
  while (flagA == false || flagB == false || flagC == false){  
    // Monitor encoder position
    tcaselect(0);                             // get the raw values of the encoders
    output[0] = encoderA.getPosition();           
    tcaselect(1);
    output[1] = encoderB.getPosition();
    tcaselect(2);
    output[2] = encoderC.getPosition();

    if ((lastOutput[0] - output[0]) > 2047 )       // check if a full rotation has been made
      revolutions[0]++;
    if ((lastOutput[0] - output[0]) < -2047 )
      revolutions[0]--;
    if ((lastOutput[1] - output[1]) > 2047 )        
      revolutions[1]++;
    if ((lastOutput[1] - output[1]) < -2047 )
      revolutions[1]--;
    if ((lastOutput[2] - output[2]) > 2047 )        
      revolutions[2]++;
    if ((lastOutput[2] - output[2]) < -2047 )
      revolutions[2]--;
      
    currentPosition[0] = (-revolutions[0] * 4096 - output[0] + initPosition[0])* 0.087890625;   // calculate the position *IN DEGREES* the the encoder is at based off of the number of revolutions
    currentLocation[0] = startLength[0] + (currentPosition[0]/360)*2*M_PI*pulleyRadius;
    currentPosition[1] = (-revolutions[1] * 4096 - output[1] + initPosition[1])* 0.087890625;   // calculate the position *IN DEGREES* the the encoder is at based off of the number of revolutions
    currentLocation[1] = startLength[1] + (currentPosition[1]/360)*2*M_PI*pulleyRadius;
    currentPosition[2] = (-revolutions[2] * 4096 - output[2] + initPosition[2])* 0.087890625;   // calculate the position *IN DEGREES* the the encoder is at based off of the number of revolutions
    currentLocation[2] = startLength[2] + (currentPosition[2]/360)*2*M_PI*pulleyRadius;
    
    Serial.println("Location:");
    Serial.println(currentLocation[0]);
    Serial.println(currentLocation[1]);
    Serial.println(currentLocation[2]);
     
    lastOutput[0] = output[0];                      // save the last raw value for the next loop 
    lastOutput[1] = output[1];
    lastOutput[2] = output[2];
    
    // Set the speed in steps per second:
    speedA = -150;
    speedB = -150;
    speedC = -150;
    stepperA.setSpeed(speedA);
    stepperB.setSpeed(speedB);
    stepperC.setSpeed(speedC);
    // Run steppers until position reached
    locationDiff[0] = currentLocation[0] - requestedLocation[0];      // check length
    if (locationDiff[0] < 0) {
      speedA = -speedA;
      stepperA.setSpeed(speedA);
    }
    locationDiff[1] = currentLocation[1] - requestedLocation[1];      
    if (locationDiff[1] < 0) {
      speedB = -speedB;
      stepperB.setSpeed(speedB);
    }
    locationDiff[2] = currentLocation[2] - requestedLocation[2];      
    if (locationDiff[2] < 0) {
      speedC = -speedC;
      stepperC.setSpeed(speedC);
    }
    
    if (currentLocation[0] < (requestedLocation[0] + 1) && currentLocation[0] > (requestedLocation[0] - 1)){    //Stop if position is within requestedPosition bounds
      speedA = 0;
      stepperA.setSpeed(speedA);
      flagA = true;
    }
    if (currentLocation[1] < (requestedLocation[1] + 1) && currentLocation[1] > (requestedLocation[1] - 1)){    
      speedB = 0;
      stepperB.setSpeed(speedB);
      flagB = true;
    }    
    if (currentLocation[2] < (requestedLocation[2] + 1) && currentLocation[2] > (requestedLocation[2] - 1)){    
      speedC = 0;
      stepperC.setSpeed(speedC);
      flagC = true;
    }    
    stepperA.runSpeed(); //run stepper at set speed
    stepperB.runSpeed();
    stepperC.runSpeed();
    Serial.println("speedA:");
    Serial.println(speedA);
    Serial.println("speedB:");
    Serial.println(speedB);
    Serial.println("speedC:");
    Serial.println(speedC);
  }
  counter += 1;
//  delay(500);               // Uncomment for delay between coordinates
  if (counter == numcoords){      //Stop once at final coordinate
    Serial.println("DONE!!!!");
    delay(100000);
  }
}
