// Takes converted G-Code coordinates and moves through all coordinates.

// Include I2C, encoder, math, and AccelStepper libraries:
#include <Wire.h>
#include <AS5600.h>
#include <math.h>
#include <AccelStepper.h>

// Create encoder instances:
AS5600 encoder0;
AS5600 encoder1;
//AS5600 encoder2;

// Define I2C address:
#define TCAADDR 0x70
// Initialize variables for position monitoring:

void tcaselect (uint8_t i) {
  if (i > 7) return;

  Wire.beginTransmission(TCAADDR);
  Wire.write(1 << i);
  Wire.endTransmission();
}

int speed0;
int speed1;
//int speed2;
int requestedLocation[2]; // in millimeters
float currentPosition[2];
float currentLocation[2];
float startLength[2] = {250, 250}; // need to manually move end effector here before starting Serial!!
float pulleyRadius = 26.25; //in mm
float positionDiff[2];
float locationDiff[2];
float output[2];
float lastOutput[2];
float initPosition[2];
float position[2];
float revolutions[2] = {0,0};
float printLength[2];
int counter = 0;
int numcoords = 147;

// Define stepper motor connections and motor interface type (1 when using driver):
#define dirPin0 3
#define stepPin0 2
#define motorInterfaceType0 1

#define dirPin1 5
#define stepPin1 4
#define motorInterfaceType1 1
//
//#define dirPin2 7
//#define stepPin2 6
//#define motorInterfaceType2 1

// Create new instances of AccelStepper class:
AccelStepper stepper0 = AccelStepper(motorInterfaceType0, stepPin0, dirPin0);
AccelStepper stepper1 = AccelStepper(motorInterfaceType1, stepPin1, dirPin1);
//AccelStepper stepper2 = AccelStepper(motorInterfaceType2, stepPin2, dirPin2);

// Copy translated coordinates here:
float coordinate_list[147][2] = 
  {
    {167.55,252.177},
    {167.478,252.365},
    {166.505,255.08},
    {165.968,257.154},
    {165.926,257.331},
    {165.462,259.627},
    {165.286,261.755},
    {165.278,261.917},
    {165.252,263.858},
    {165.528,265.943},
    {165.554,266.089},
    {165.883,267.656},
    {166.683,269.693},
    {166.74,269.824},
    {167.334,271.045},
    {168.721,272.977},
    {168.811,273.091},
    {169.568,274.005},
    {171.589,275.776},
    {171.704,275.874},
    {172.532,276.518},
    {175.208,278.071},
    {175.352,278.153},
    {176.159,278.573},
    {179.499,279.85},
    {179.53,279.863},
    {180.371,280.153},
    {184.361,281.102},
    {185.086,281.256},
    {189.703,281.816},
    {190.189,281.869},
    {195.42,281.991},
    {195.698,281.994},
    {201.337,281.63},
    {201.701,281.581},
    {207.223,280.772},
    {207.889,280.631},
    {213.208,279.431},
    {214.211,279.139},
    {219.22,277.613},
    {220.523,277.13},
    {225.196,275.327},
    {226.784,274.608},
    {231.08,272.589},
    {232.926,271.592},
    {236.817,269.413},
    {238.888,268.097},
    {242.361,265.818},
    {244.411,264.306},
    {244.618,264.151},
    {247.665,261.825},
    {249.87,259.944},
    {250.065,259.776},
    {252.759,257.401},
    {255.024,255.236},
    {255.236,255.024},
    {257.401,252.759},
    {259.776,250.065},
    {259.944,249.87},
    {261.825,247.665},
    {264.151,244.618},
    {264.306,244.411},
    {265.818,242.361},
    {268.097,238.888},
    {268.237,238.673},
    {269.413,236.817},
    {271.592,232.926},
    {271.714,232.703},
    {272.589,231.08},
    {274.608,226.784},
    {274.715,226.556},
    {275.327,225.196},
    {277.13,220.523},
    {277.148,220.477},
    {277.613,219.22},
    {279.139,214.211},
    {279.431,213.208},
    {280.624,207.914},
    {280.768,207.256},
    {281.579,201.715},
    {281.628,201.369},
    {281.994,195.698},
    {281.991,195.429},
    {281.87,190.218},
    {281.819,189.721},
    {281.256,185.086},
    {281.108,184.389},
    {280.153,180.371},
    {279.85,179.499},
    {278.552,176.116},
    {278.071,175.208},
    {276.518,172.532},
    {275.776,171.589},
    {274.005,169.568},
    {272.977,168.721},
    {271.045,167.334},
    {269.693,166.683},
    {267.656,165.883},
    {266.089,165.554},
    {265.943,165.528},
    {263.858,165.252},
    {261.917,165.278},
    {261.755,165.286},
    {259.627,165.462},
    {257.331,165.926},
    {257.154,165.968},
    {255.08,166.505},
    {252.365,167.478},
    {252.177,167.55},
    {250.203,168.351},
    {247.056,169.89},
    {246.856,169.991},
    {245.031,170.957},
    {241.446,173.1},
    {241.236,173.229},
    {239.603,174.258},
    {235.581,177.027},
    {235.361,177.181},
    {233.956,178.187},
    {229.511,181.583},
    {229.286,181.758},
    {228.139,182.66},
    {223.295,186.669},
    {222.2,187.594},
    {216.996,192.187},
    {216.232,192.87},
    {210.681,198.041},
    {210.224,198.47},
    {204.429,204.133},
    {204.14,204.422},
    {198.501,210.193},
    {198.055,210.665},
    {192.902,216.198},
    {192.211,216.968},
    {187.594,222.2},
    {186.669,223.295},
    {182.66,228.139},
    {181.583,229.511},
    {178.187,233.956},
    {177.027,235.581},
    {174.258,239.603},
    {173.1,241.446},
    {170.957,245.031},
    {169.89,247.056},
    {168.393,250.118},
    {167.301,250.851},
    {250,250}               //Can be convenient to add homing coordinate at end of program to return cables to original lengths (Don't forget to add 1 to the numcoords).
  };

//------------------------------------------------------------------------------------------------
void setup() {
  // Begin serial communication:
  Serial.begin(115200);
  
  // Set the maximum speed in steps per second:
  stepper0.setMaxSpeed(2000);
  stepper1.setMaxSpeed(2000);
//  stepper2.setMaxSpeed(2000);
  
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

//  tcaselect(2);
//  output[2] = encoder2.getPosition();
//  initPosition[2] = output[2];
//  lastOutput[2] = output[2];
//  position[2] = output[2];
}

//------------------------------------------------------------------------------------------------
void loop() {
  Serial.println("Requested Location");.
  for(int i=0; i<2; i++)  {
    requestedLocation[i] = coordinate_list[counter][i];
    Serial.println(requestedLocation[i]);  
  }
  if (counter == 0){
    delay(10000);
  }
  delay(5000);
  boolean flag0 = false;
  boolean flag1 = false;
//  flag2 = true;
  while (flag0 == false || flag1 == false){  
    // Monitor encoder position
    tcaselect(0);                             // get the raw values of the encoders
    output[0] = encoder0.getPosition();           
    tcaselect(1);
    output[1] = encoder1.getPosition();
//    tcaselect(2);
//    output[2] = encoder2.getPosition();

    if ((lastOutput[0] - output[0]) > 2047 )       // check if a full rotation has been made
      revolutions[0]++;
    if ((lastOutput[0] - output[0]) < -2047 )
      revolutions[0]--;
    if ((lastOutput[1] - output[1]) > 2047 )        
      revolutions[1]++;
    if ((lastOutput[1] - output[1]) < -2047 )
      revolutions[1]--;
//    if ((lastOutput[2] - output[2]) > 2047 )        
//      revolutions[2]++;
//    if ((lastOutput[2] - output[2]) < -2047 )
//      revolutions[2]--;
      
    currentPosition[0] = (-revolutions[0] * 4096 - output[0] + initPosition[0])* 0.087890625;   // calculate the position *IN DEGREES* the the encoder is at based off of the number of revolutions
    currentLocation[0] = startLength[0] + (currentPosition[0]/360)*2*M_PI*pulleyRadius;
    printLength[0] = currentLocation[0];
    currentPosition[1] = (-revolutions[1] * 4096 - output[1] + initPosition[1])* 0.087890625;   // calculate the position *IN DEGREES* the the encoder is at based off of the number of revolutions
    currentLocation[1] = startLength[1] + (currentPosition[1]/360)*2*M_PI*pulleyRadius;
    printLength[1] = currentLocation[1];
//    currentPosition[2] = (-revolutions[2] * 4096 - output[1] + initPosition[2])* 0.087890625;   // calculate the position *IN DEGREES* the the encoder is at based off of the number of revolutions
//    currentLocation[2] = startLength[2] + (currentPosition[2]/360)*2*M_PI*pulleyRadius;
//    printLength[2] = currentLocation[2];
        
    Serial.println("Location:");
    Serial.println(printLength[0]);
    Serial.println(printLength[1]);
//    Serial.println(printLength[2]);
    
    lastOutput[0] = output[0];                      // save the last raw value for the next loop 
    lastOutput[1] = output[1];
//    lastOutput[2] = output[2];
    
    // Set the speed in steps per second:
    speed0 = -100;
    speed1 = -100;
//    speed2 = -100;
    stepper0.setSpeed(speed0);
    stepper1.setSpeed(speed1);
//    stepper2.setSpeed(speed2);
    // Run steppers until position reached
    locationDiff[0] = currentLocation[0] - requestedLocation[0];      // check length
    if (locationDiff[0] < 0) {
      speed0 = -speed0;
      stepper0.setSpeed(speed0);
    }
    locationDiff[1] = currentLocation[1] - requestedLocation[1];      
    if (locationDiff[1] < 0) {
      speed1 = -speed1;
      stepper1.setSpeed(speed1);
    }
//    locationDiff[2] = currentLocation[2] - requestedLocation[2];      
//    if (locationDiff[2] < 0) {
//      speed2 = -speed2;
//      stepper2.setSpeed(speed2);
//    }
    
    if (currentLocation[0] < (requestedLocation[0] + 1) && currentLocation[0] > (requestedLocation[0] - 1)){    //stop if position is within requestedPosition bounds
      speed0 = 0;
      stepper0.setSpeed(speed0);
      flag0 = true;
    }
    if (currentLocation[1] < (requestedLocation[1] + 1) && currentLocation[1] > (requestedLocation[1] - 1)){    
      speed1 = 0;
      stepper1.setSpeed(speed1);
      flag1 = true;
    }    
//    if (currentLocation[2] < (requestedLocation[2] + 1) && currentLocation[2] > (requestedLocation[2] - 1)){    
//      speed2 = 0;
//      stepper2.setSpeed(speed2);
//      flag2 = true;
//    }    
    stepper0.runSpeed(); //run stepper at set speed
    stepper1.runSpeed();
//    stepper2.runSpeed();
    Serial.println("Speed0:");
    Serial.println(speed0);
    Serial.println("Speed1:");
    Serial.println(speed1);
//    Serial.println("Speed2:");
//    Serial.println(speed2);
  }
  counter += 1;
  delay(3000);
  if (counter == numcoords){
    Serial.println("DONE!!!!!");
    delay(1000000);
  }
}
