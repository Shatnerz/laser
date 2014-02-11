//ArduinoStepperControlv8.0
//2012-04-14 jdreyer
//2013-01-31 dufourf
//Stepper motor control program 

int StartByte;
int i;
int j;
long userInput;
long steps;
long stepsToMove;
long StepperPosition=0;
long previous=0;
int deltime=3000; //the delay between stepper move commands in microseconds
int wavetime=10; //the on-time of the square wave in microseconds
float Version=8.1;

int DirectionPin=12;
int StepPin=13;
int VoltagePin=A0;

void setup() {
   Serial.begin(115200);
   pinMode(DirectionPin, OUTPUT);
   pinMode(StepPin, OUTPUT);
}

void loop()
{
  if (Serial.available() > 4) { //if there are at least 5 bytes in the serial buffer...
    StartByte = Serial.read(); //figure out what the function to be called is
    userInput=readULongFromBytes(); //read the unsigned long integer argument
    
    if (StartByte == 255) { //reset to 0
      previous=0;
      StepperPosition=0;
    }
    
    if (StartByte == 0) { //move to positive position
      stepsToMove = userInput-previous;
      if (stepsToMove > 0){ //forward
        TurnMotorClockWise(stepsToMove);
      }
      if (stepsToMove < 0){ //backward
        TurnMotorCounterClockWise(-stepsToMove);
      }
      //don't bother to call anything if we're already there
      previous = userInput;
    }

    if (StartByte == 1) { //move to negative position
      stepsToMove = -userInput-previous;
      if (stepsToMove > 0){
        TurnMotorClockWise(stepsToMove);
      }
      if (stepsToMove < 0){
        TurnMotorCounterClockWise(-stepsToMove);
      }
      previous = -userInput;
    }
    
    if (StartByte == 2) { //move to positive position while reporting the analog readout at each step
      stepsToMove = -userInput-previous;
      if (stepsToMove > 0){
        TurnMotorClockWiseAndReport(stepsToMove);
      }
      if (stepsToMove < 0){
        TurnMotorCounterClockWiseAndReport(-stepsToMove);
      }
      previous = -userInput;
    }
    
    if (StartByte == 3) { //move to negative position while reporting the analog readout at each step
      stepsToMove = userInput-previous;
      if (stepsToMove > 0){
        TurnMotorClockWiseAndReport(stepsToMove);
      }
      if (stepsToMove < 0){
        TurnMotorCounterClockWiseAndReport(-stepsToMove);
      }
      previous = userInput;
    }

    if (StartByte == 4) { //report position
      Serial.print(StepperPosition);
      //Serial.print('\n');
      delay(100);
    }

    if (StartByte == 5) { //read the analog pin
      Serial.print(analogRead(VoltagePin));
      //Serial.print('\n');
      delay(100);
    }
    
    if (StartByte == 6) { //report this sketch's version
      Serial.print(Version);
      //Serial.print('\n');
      delay(100);
    }
  }
}

void TurnMotorClockWise(int steps)
{
  //NOTE: LOW on the direction pin is clockwise
  //Set the direction.
  digitalWrite(DirectionPin, LOW);
  //make sure the step pin was left in the clean, low, position (the controller triggers on the rise)
  digitalWrite(StepPin, LOW);  
  // Iterate for [steps] microsteps
  for (i = 0; i<steps; i++)       
  {
    // This LOW to HIGH change is what creates the
    // "Rising Edge" so the easydriver knows to when to step.
    digitalWrite(StepPin, HIGH); 
    // This delay is there to ensure the controller picks up the rising wave
    delayMicroseconds(wavetime);      
    digitalWrite(StepPin, LOW);  
    // This delay time is close to top speed for the motor
    delayMicroseconds(deltime);      
  }
  //finally we update the position
  StepperPosition=userInput;
  delay(100);
}

void TurnMotorCounterClockWise(int steps)
{
  //NOTE: HIGH on the direction pin is clockwise
  digitalWrite(DirectionPin, HIGH);
  for (i = 0; i<steps; i++)       
  {
    digitalWrite(StepPin, LOW);
    digitalWrite(StepPin, HIGH);
    digitalWrite(StepPin, LOW);
    delayMicroseconds(deltime);
  }
  StepperPosition=userInput;
  delay(100);
}

void TurnMotorClockWiseAndReport(int steps)
{
  //NOTE: LOW is clockwise
  //Set the direction.
  digitalWrite(DirectionPin, LOW);
  //make sure the serial buffer is empty
  Serial.flush();
  digitalWrite(StepPin, LOW);  
  // Iterate for [steps] microsteps
  for (i = 0; i<steps; i++)       
  {
    // This LOW to HIGH change is what creates the
    // "Rising Edge" so the easydriver knows to when to step.
    digitalWrite(StepPin, HIGH); 
    delayMicroseconds(wavetime);      
    digitalWrite(StepPin, LOW);
    Serial.print(analogRead(VoltagePin));
    Serial.print('\n');
    // This delay time is close to top speed for the motor
    delayMicroseconds(deltime);      
  }
  StepperPosition=userInput;
  delay(100);
}

void TurnMotorCounterClockWiseAndReport(int steps)
{
  //Set the direction.
  digitalWrite(DirectionPin, HIGH);
  Serial.flush();
  digitalWrite(StepPin, LOW);
  for (i = 0; i<steps; i++)       
  {
    digitalWrite(StepPin, HIGH);
    delayMicroseconds(wavetime);
    digitalWrite(StepPin, LOW);
    Serial.print(analogRead(VoltagePin));
    Serial.print('\n');
    delayMicroseconds(deltime);
  }
  StepperPosition=userInput;
  delay(100);
}

unsigned long readULongFromBytes() {
//This function takes 4 bytes (read from the serial port) and converts them to an unsigned long int.
  union u_tag {
    byte b[4];
    long ulval;
  } u;
  u.b[0] = Serial.read();
  u.b[1] = Serial.read();
  u.b[2] = Serial.read();
  u.b[3] = Serial.read();
  return u.ulval;
}


