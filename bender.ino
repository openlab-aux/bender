
#define UNSIGNEDLONGRANGE 4294967295
#define MICROSINSECOND 1000000
#define NUMBERMOTORS 6


class Timer{

private:

  unsigned long lastMicros;


public:
  Timer()
  {
    lastMicros=micros();
  }

  unsigned long microsSinceLastCall()
  {
    unsigned long newMicros=micros();

    unsigned long microDifference;


    if(newMicros<lastMicros)//Overflow
    {
      microDifference=(UNSIGNEDLONGRANGE-lastMicros)+newMicros;
    }
    else
    {
      microDifference=newMicros-lastMicros;
    }

    lastMicros=newMicros;

    return microDifference;
  }
};




// Ramp-Shield with Pololu A4988
//#Slot E  ### Slot D ### Slot C ###
///# #######Slot B ### Slot A ### 

#define SLOT_C_STEP_PIN         54
#define SLOT_C_DIR_PIN          55
#define SLOT_C_ENABLE_PIN       38


#define SLOT_D_STEP_PIN         60
#define SLOT_D_DIR_PIN          61
#define SLOT_D_ENABLE_PIN       56

#define SLOT_E_STEP_PIN         46
#define SLOT_E_DIR_PIN          48
#define SLOT_E_ENABLE_PIN       62

#define SLOT_A_STEP_PIN         26
#define SLOT_A_DIR_PIN          28
#define SLOT_A_ENABLE_PIN       24

#define SLOT_B_STEP_PIN         36
#define SLOT_B_DIR_PIN          34
#define SLOT_B_ENABLE_PIN       30

#define SLOT_F_STEP_PIN        17
#define SLOT_F_DIR_PIN          23
#define SLOT_F_ENABLE_PIN      16

///#define SDPOWER            -1
//#define SDSS               53
#define LED_PIN            13
#define FAN_PIN            9

//#define PS_ON_PIN          12
//#define KILL_PIN           -1

#define HEATER_0_PIN       10
#define HEATER_1_PIN       8
//#define TEMP_0_PIN          13   // ANALOG NUMBERING
//#define TEMP_1_PIN          14   // ANALOG NUMBERING


#define NoError 0
#define Error 1
#define MotorStepped 2
#define TimingNotMet 3
#define StepAlreadySet 4



struct Pin {
protected: 

  int pinNumber;
  boolean pinState;

public :
  int setHigh()
  {
    digitalWrite(pinNumber, HIGH);
    return NoError;
  }

  int setLow()
  {
    digitalWrite(pinNumber, LOW);
    return NoError;
  }

  int set(int setting) //only LOW or HIGH
  {
    digitalWrite(pinNumber, setting);
    pinState=setting;
  }

  int flip()
  {
    if(pinState==HIGH)
    {
      digitalWrite(pinNumber, LOW);
      pinState=LOW;
    }
    else
    {
      digitalWrite(pinNumber, HIGH);
      pinState=HIGH; 
    }
    return NoError;
  }



  Pin(int pinNumber)
  {
    this->pinNumber=pinNumber;
    pinState=LOW;//i assume that default value is LOW but did not backcheck
    setLow(); //Now its definetly LOW
  }

};


struct StepPin : 
Pin
{
public:

  StepPin(int pinNumber2): 
  Pin(pinNumber2)
  {

  }

  int doStep()
  {
    return flip();
  }

};

struct EnablePin : 
Pin
{
public:

  EnablePin(int pinNumber2): 
  Pin(pinNumber2)
  {
    setLow();
  }

};

struct DirectionPin : 
Pin
{
public:

  static const int CLOCKWISE=HIGH;
  static const int COUNTERCLOCKWISE=LOW;


  DirectionPin(int pinNumber2): 
  Pin(pinNumber2)
  {
    //Pin(pinNumber2);
  }

  int setClockwise()
  {
    return set(CLOCKWISE); 
  }

  int setCounterClockwise()
  {
    return set(COUNTERCLOCKWISE); 
  }  

  boolean isClockwise()
  {
    return pinState==CLOCKWISE; 
  }

  boolean isCounterClockwise()
  {
    return pinState==COUNTERCLOCKWISE; 
  }

};




struct Pololu 
{
private: 
  StepPin* stepPin;
  DirectionPin* dirPin;
  EnablePin* enablePin;

public: 

  int doStep()
  {
    return stepPin->doStep();
  }

  int setDirCW()
  {
    return dirPin->setClockwise();
  }

  int setDirCounterCW()
  {
    return dirPin->setCounterClockwise();
  }


  Pololu(StepPin* stepPin,DirectionPin* dirPin,EnablePin* enablePin)
  {
    this->stepPin=stepPin;
    this->dirPin=dirPin;
    this->enablePin=enablePin;
  }

};


class EndSwitch
{

public:

  EndSwitch(Pin pin)
  {

  }


};

class Motor {

  /* see above:
   #define NoError 0
   #define Error 1
   #define MotorStepped 2
   #define TimingNotMet 3
   #define StepAlreadySet 4
   */
private: 


  //const int stepsInFullRound = 321;

  //Maximal rotation range of motor 


  //long int maxRotationInSteps; 
  //long int minRotationInSteps;

  //int stepsPerSecond;

  long int desiredRotationInSteps;
  long int rotationInSteps;
  Pololu* driver;
  unsigned long int period; 
  unsigned long int elapsedTime; 



public: 


  int addSteps(long int steps)
  {
    desiredRotationInSteps=steps;
    return NoError;
  }



  int setStepsPerSecond(unsigned long int stepsPerSecond)
  {
    //this->period=MICROSINSECOND / stepsPerSecond;
    period=(MICROSINSECOND) / stepsPerSecond;
    return NoError;
  }




  long int getRotationInSteps()
  {
    return rotationInSteps;
  }


  int runStepsInTime(long int steps, unsigned long int time)
  {
    period=time/abs(steps);
    addSteps(steps);
    elapsedTime=0;

  }


  Motor(Pololu* driver)
  {    
    this->driver=driver;
   // this->setStepsPerSecond(900);
    rotationInSteps=0;    
  }

  int run(unsigned long microSeconds)
  {
    elapsedTime+=microSeconds;


    if(elapsedTime>2*period)
    {
      // DEBUG: return TimingNotMet;
    }

    if (elapsedTime>period)
    {
      if(desiredRotationInSteps!=0)
      {

        if(desiredRotationInSteps>0)
        {
          driver->setDirCounterCW();
          desiredRotationInSteps--;
          rotationInSteps++;
        }
        else 
        {
          driver->setDirCW();
          desiredRotationInSteps++;
          rotationInSteps--;
        }
        elapsedTime-=period;
        driver->doStep();
        return  MotorStepped;
      }
    }


    return  NoError;

  }  

};


/*
struct CollisionDetector
 {
 
 public :
 
 CollisionDetector()
 {
 
 }
 
 int requestRotation(float rotation)
 {
 
 
 
 return NoError;
 }
 
 
 };
 
 */




/*
struct Axis {
 private: 
 int rotationInSteps;
 double rotationInDeg;
 
 public: 
 int getRotationInSteps()
 {
 return rotationInSteps;
 }
 
 
 
 
 };
 
 
 */





class Cobra{

private:

  boolean firstRun;
  unsigned long lastRunInstantOfTime;//instantOfTime= Zeitpunkt
  Timer* timer;

  Motor* motor[NUMBERMOTORS];


public:

  Cobra()
  {
    firstRun=true;
    timer=new Timer();

    motor[0]=new Motor(new Pololu(new StepPin(SLOT_A_STEP_PIN),new DirectionPin(SLOT_A_DIR_PIN),new EnablePin(SLOT_A_ENABLE_PIN)));
    motor[1]=new Motor(new Pololu(new StepPin(SLOT_B_STEP_PIN),new DirectionPin(SLOT_B_DIR_PIN),new EnablePin(SLOT_B_ENABLE_PIN)));
    motor[2]=new Motor(new Pololu(new StepPin(SLOT_C_STEP_PIN),new DirectionPin(SLOT_C_DIR_PIN),new EnablePin(SLOT_C_ENABLE_PIN)));
    motor[3]=new Motor(new Pololu(new StepPin(SLOT_D_STEP_PIN),new DirectionPin(SLOT_D_DIR_PIN),new EnablePin(SLOT_D_ENABLE_PIN)));
    motor[4]=new Motor(new Pololu(new StepPin(SLOT_E_STEP_PIN),new DirectionPin(SLOT_E_DIR_PIN),new EnablePin(SLOT_E_ENABLE_PIN)));
    motor[5]=new Motor(new Pololu(new StepPin(SLOT_F_STEP_PIN),new DirectionPin(SLOT_F_DIR_PIN),new EnablePin(SLOT_F_ENABLE_PIN)));
  }

  int run(unsigned long runtime)
  {


    if(firstRun)
    {
      timer->microsSinceLastCall();  
      firstRun=false;   
    }

    unsigned long elapsedTime=timer->microsSinceLastCall();

    for(int i=0;i<NUMBERMOTORS;i++)
    {
      motor[i]->run(elapsedTime);
    }

    return NoError;
  } 





  int addSteps(long int a,long int b, long int c, long int d, long int e,long int f)
  {
    motor[0]->addSteps(a);
    motor[1]->addSteps(b);
    motor[2]->addSteps(c);
    motor[3]->addSteps(d);
    motor[4]->addSteps(e);
    motor[5]->addSteps(f);

    return NoError;
  }





  int runStepsInTime(long int a,long int b, long int c, long int d, long int e,long int f,unsigned long timeInMicros)
  {
    unsigned long int totalRunTime=0;
    motor[0]->runStepsInTime(a,timeInMicros);
    motor[1]->runStepsInTime(b,timeInMicros);
    motor[2]->runStepsInTime(c,timeInMicros);
    motor[3]->runStepsInTime(d,timeInMicros);
    motor[4]->runStepsInTime(e,timeInMicros);
    motor[5]->runStepsInTime(f,timeInMicros);

   

  
    unsigned long elapsedTime=timer->microsSinceLastCall();

    while (totalRunTime<timeInMicros)
    {
      elapsedTime=timer->microsSinceLastCall();
      for(int i=0;i<NUMBERMOTORS;i++)
      {
        motor[i]->run(elapsedTime);
      }
      totalRunTime=totalRunTime+elapsedTime;
    }
    return NoError;
  }



  int runFast(long int a,long int b, long int c, long int d, long int e,long int f)
  {
    int stepperSpeed=50;
    unsigned long int timeInMicros=max(abs(f),max(abs(e),max(abs(d),max(abs(c),max(abs(a),abs(b))))))*stepperSpeed;
    runStepsInTime(a,b,c, d,  e, f,timeInMicros);
  
    
       return NoError;
  }

  int runSlow(long int a,long int b, long int c, long int d, long int e,long int f)
  {
    int stepperSpeed=200;
    unsigned long int timeInMicros=max(abs(f),max(abs(e),max(abs(d),max(abs(c),max(abs(a),abs(b))))))*stepperSpeed;
    runStepsInTime(a,b,c, d,  e, f,timeInMicros);
  
    
       return NoError;
  }




};

// The number of steps in one full motor rotation
const int stepsInFullRound = 321;
const int MOTOR_SECOND_LIMB = 1;
const int MOTOR_HEAD_RIGHT = 2;
const int MOTOR_HEAD_LEFT = 3;
const int MOTOR_FIRST_LIMB= 4;
const int MOTOR_ROTATE = 5;


Cobra* cobra;

// Set pins
void setup() {

  pinMode(FAN_PIN , OUTPUT);
  pinMode(HEATER_0_PIN , OUTPUT);
  pinMode(HEATER_1_PIN , OUTPUT);
  pinMode(LED_PIN  , OUTPUT);

  pinMode(SLOT_C_STEP_PIN  , OUTPUT);
  pinMode(SLOT_C_DIR_PIN    , OUTPUT);
  pinMode(SLOT_C_ENABLE_PIN    , OUTPUT);

  pinMode(SLOT_D_STEP_PIN  , OUTPUT);
  pinMode(SLOT_D_DIR_PIN    , OUTPUT);
  pinMode(SLOT_D_ENABLE_PIN    , OUTPUT);

  pinMode(SLOT_E_STEP_PIN  , OUTPUT);
  pinMode(SLOT_E_DIR_PIN    , OUTPUT);
  pinMode(SLOT_E_ENABLE_PIN    , OUTPUT);

  pinMode(SLOT_A_STEP_PIN  , OUTPUT);
  pinMode(SLOT_A_DIR_PIN    , OUTPUT);
  pinMode(SLOT_A_ENABLE_PIN    , OUTPUT);

  pinMode(SLOT_B_STEP_PIN  , OUTPUT);
  pinMode(SLOT_B_DIR_PIN    , OUTPUT);
  pinMode(SLOT_B_ENABLE_PIN    , OUTPUT);

  pinMode(SLOT_F_STEP_PIN  , OUTPUT);
  pinMode(SLOT_F_DIR_PIN    , OUTPUT);
  pinMode(SLOT_F_ENABLE_PIN    , OUTPUT);


  digitalWrite(SLOT_C_ENABLE_PIN    , LOW);
  digitalWrite(SLOT_D_ENABLE_PIN    , LOW);
  digitalWrite(SLOT_E_ENABLE_PIN    , LOW);
  digitalWrite(SLOT_A_ENABLE_PIN    , LOW);
  digitalWrite(SLOT_B_ENABLE_PIN    , LOW);
  digitalWrite(SLOT_F_ENABLE_PIN    , LOW);

  cobra=new Cobra();
  
  /*for(int i=0;i<1000;i++)
  {
   cobra->runStepsInTime(0,10,0,0,0,0,1000);
  }*/
 

cobra->runFast(0,25000,0,0,0,0);
cobra->runSlow(0,-25000,0,0,0,0);

cobra->runFast(0,0,25000,0,0,0);
cobra->runSlow(0,0,-25000,0,0,0);

cobra->runFast(0,0,0,25000,0,0);
cobra->runSlow(0,0,0,-25000,0,0);



/*
cobra->runFast(10000,10000,10000,10000,1000,10000);
cobra->runFast(-10000,0,0,0,0,0);
cobra->runFast(0,-10000,0,0,0,0);
cobra->runFast(0,0,-10000,0,0,0);
cobra->runFast(0,0,0,-10000,0,0);
cobra->runFast(0,0,0,0,-1000,0);
cobra->runFast(0,0,0,0,0,-10000);

cobra->runSlow(0,0,0,10000,0,0);
cobra->runSlow(0,0,0,-10000,0,0);
*/

}




// Tests various speeds for 10 full rotations
void loop(){

}



