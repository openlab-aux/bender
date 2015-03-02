


#define X_STEP_PIN         54
#define X_DIR_PIN          55
#define X_ENABLE_PIN       38
#define X_MIN_PIN           3
#define X_MAX_PIN           2

#define Y_STEP_PIN         60
#define Y_DIR_PIN          61
#define Y_ENABLE_PIN       56
#define Y_MIN_PIN          14
#define Y_MAX_PIN          15

#define Z_STEP_PIN         46
#define Z_DIR_PIN          48
#define Z_ENABLE_PIN       62
#define Z_MIN_PIN          18
#define Z_MAX_PIN          19

#define E_STEP_PIN         26
#define E_DIR_PIN          28
#define E_ENABLE_PIN       24

#define Q_STEP_PIN         36
#define Q_DIR_PIN          34
#define Q_ENABLE_PIN       30

#define SDPOWER            -1
#define SDSS               53
#define LED_PIN            13

#define FAN_PIN            9

#define PS_ON_PIN          12
#define KILL_PIN           -1

#define HEATER_0_PIN       10
#define HEATER_1_PIN       8
#define TEMP_0_PIN          13   // ANALOG NUMBERING
#define TEMP_1_PIN          14   // ANALOG NUMBERING

/*
A simple test code for running a bipolar stepper motor with Pololu A4988 carrier.
*/

// The number of steps in one full motor rotation
const int stepsInFullRound = 321;
const int MOTOR_SECOND_LIMB = 1;
const int MOTOR_HEAD_RIGHT = 2;
const int MOTOR_HEAD_LEFT = 3;
const int MOTOR_FIRST_LIMB= 4;
const int MOTOR_ROTATE = 5;

// Set pins
void setup() {
  pinMode(FAN_PIN , OUTPUT);
  pinMode(HEATER_0_PIN , OUTPUT);
  pinMode(HEATER_1_PIN , OUTPUT);
  pinMode(LED_PIN  , OUTPUT);
  
  pinMode(X_STEP_PIN  , OUTPUT);
  pinMode(X_DIR_PIN    , OUTPUT);
  pinMode(X_ENABLE_PIN    , OUTPUT);
  
  pinMode(Y_STEP_PIN  , OUTPUT);
  pinMode(Y_DIR_PIN    , OUTPUT);
  pinMode(Y_ENABLE_PIN    , OUTPUT);
  
  pinMode(Z_STEP_PIN  , OUTPUT);
  pinMode(Z_DIR_PIN    , OUTPUT);
  pinMode(Z_ENABLE_PIN    , OUTPUT);
  
  pinMode(E_STEP_PIN  , OUTPUT);
  pinMode(E_DIR_PIN    , OUTPUT);
  pinMode(E_ENABLE_PIN    , OUTPUT);
  
  pinMode(Q_STEP_PIN  , OUTPUT);
  pinMode(Q_DIR_PIN    , OUTPUT);
  pinMode(Q_ENABLE_PIN    , OUTPUT);
  
   digitalWrite(X_ENABLE_PIN    , LOW);
   digitalWrite(Y_ENABLE_PIN    , LOW);
   digitalWrite(Z_ENABLE_PIN    , LOW);
   digitalWrite(E_ENABLE_PIN    , LOW);
   digitalWrite(Q_ENABLE_PIN    , LOW);
/*  pinMode(stepPin, OUTPUT);      
  pinMode(dirPin, OUTPUT);
  digitalWrite(stepPin, LOW);
  digitalWrite(dirPin, LOW);  */
}

// Runs the motor according to a chosen direction, speed (rounds per seconds) and the number of steps
void run(boolean runForward, double speedRPS, int stepCount, int motor_id) {
  int stepPin = 0;
  int dirPin = 0;
  switch (motor_id) {
     case MOTOR_SECOND_LIMB:
         stepPin = X_STEP_PIN;
         dirPin = X_DIR_PIN;
         break;
     case MOTOR_HEAD_RIGHT:
         stepPin = Y_STEP_PIN;
         dirPin = Y_DIR_PIN;
         break;
     case MOTOR_HEAD_LEFT:
         stepPin = Z_STEP_PIN;
         dirPin = Z_DIR_PIN;
         break;
     case MOTOR_FIRST_LIMB:
         stepPin = Q_STEP_PIN;
         dirPin = Q_DIR_PIN;
         break;
     case MOTOR_ROTATE:
         stepPin = E_STEP_PIN;
         dirPin = E_DIR_PIN;
         break;
     default:
         return;
  }
  digitalWrite(dirPin, runForward);
  for (int i = 0; i < stepCount; i++) {
    digitalWrite(stepPin, HIGH);
    holdHalfCylce(speedRPS);
    digitalWrite(stepPin, LOW);
    holdHalfCylce(speedRPS);
  }
}

// A custom delay function used in the run()-method
void holdHalfCylce(double speedRPS) {
  long holdTime_us = (long)(1.0 / (double) stepsInFullRound / speedRPS / 2.0 * 1E6);
  int overflowCount = holdTime_us / 65535;
  for (int i = 0; i < overflowCount; i++) {
    delayMicroseconds(65535);
  }
  delayMicroseconds((unsigned int) holdTime_us);
}

// Runs the motor once in forward direction and once to the opposite direction. 
// Holds the motor still for 1 sec after both movements. 
void runBackAndForth(double speedRPS, int rounds, int motor_id) {
  run(true, speedRPS, stepsInFullRound * rounds, motor_id);
  delay(1000);
  run(false, speedRPS, stepsInFullRound * rounds, motor_id);
  delay(1000);
}


// Tests various speeds for 10 full rotations
void loop(){
  //runBackAndForth(10, 10, MOTOR_FIRST_LIMB);
  runBackAndForth(10, 10, MOTOR_ROTATE);
//  runBackAndForth(7, 10, MOTOR_X);  
//  runBackAndForth(10, 10, MOTOR_Y);

}

