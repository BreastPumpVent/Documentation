
// constants won't change. Used here to set a pin number:
const int pump =  28; // the number of the pin connection on the arduino to the pump
const int inhale = 22; // the number of the pin connection on the arduino to the inhale solenoid
const int exhale = 24; // the number of the pin connection on the arduino to the exhale solenoid
const int emergency = 26; // the number of the pin connection on the arduino to the emergency relefi solenoid
const int sensor = A10; // the number of the pin connection on the arduino to the pressure sensor
const int threshold = 800; // highest pressure  that would cause an emergency to occur (artifical for now)
// Variables will change:
int sensorvalue; // stored pressure value from the analog readout of the sensor 
bool emergencystate = false; //saves emergency state of the device

// constants won't change:
unsigned long inhaletime = 1667; // respiratory rate in milliseconds for inhale based off 12 breaths per minute: 5/3
unsigned long exhaletime = (long)(inhaletime * 2); // control inhale:exhale 1:2 
unsigned long currentTime; // timing variables
unsigned long newTime; // timing variables
void setup() {
  // set the digital pin as output:
  pinMode(pump, OUTPUT); // set the digital pump pin as output:
  pinMode(inhale, OUTPUT); // set the digital pin that controls the inhale as output:
  pinMode(exhale, OUTPUT); // set the digital pin exhale as output:
  pinMode(emergency, OUTPUT); // set the digital pin that powers the emergency sensor as output:
  Serial.begin(9600); // begin USB serial communication
}
void inhalefunct()        // set up software for inhale state
  {
      digitalWrite(pump, LOW);  // Pump Power Applied
      digitalWrite(inhale, LOW);  // Open Inhale solenoid
      digitalWrite(exhale, HIGH);  //close exhale solenoid
      digitalWrite(emergency, HIGH); //turn off emergency
      newTime = millis(); // used for timing
    while((newTime - currentTime)<inhaletime) // while the pump is pushing air into the lungs for a specified amount of time, monitor the emergency pressure sensor
    {
      sensorvalue=analogRead(sensor); // read the pressure
      //Serial.print("inhale pressure:"); // print to nurse the inhale pressure with number in the next line of code
      //Serial.println(sensorvalue); // this is the pressure value that will be read off in the print to nurse
     if (sensorvalue >= threshold) // working on code to disrupt the pump operation for in the case of over pressure
     {
      Serial.println("Inhale emergency");
      emergencystate = true; // sets code to emergency state
     }
      newTime = millis(); // used for timing
    }
  }

void exhalefunct()      // set up software for exhale state 
  {
      digitalWrite(pump, HIGH); //cut power to pump
      digitalWrite(inhale, HIGH); //close inhale solenoid
      digitalWrite(exhale, LOW);  //open exhale solenoid
      digitalWrite(emergency, HIGH); //turn off emergency
      newTime = millis(); // used for timing 
    while((newTime-currentTime)<exhaletime) // while the pump is paused for patient to exhale for a specified amount of time, monitor the emergency pressure sensor
    {
     sensorvalue=analogRead(sensor); // read the pressure
    //Serial.print("exhale pressure:"); // print to nurse the exhale pressure with number in the next line of code
    //Serial.println(sensorvalue); // this is the pressure value that will be read off in the print to nurse
     if (sensorvalue >= threshold) // working on code to disrupt the pump operation for in the case of over pressure
     {
      Serial.println("Exhale emergency");
      emergencystate = true; // sets code to emergency state
     }
      newTime = millis(); // used for timing
    }
  }
/*
 * TODO
 * return to proper period of breathing following emergency resolution
 * Institute delay to return to normal (500 ms?)
 * 
 * 
 * 
 * This only handles over-pressure situation
 */
void emergencyfunct()   // set up software for emergency state
  {
    digitalWrite(pump, HIGH);  //cut power to pump
    digitalWrite(inhale, HIGH);  //close inhale solenoid
    digitalWrite(exhale, HIGH);  //close exhale solenoid 
    digitalWrite(emergency, LOW); //open emergency solenoid, pressure releases from hoses. 
    while(emergencystate==true) //wait until emergency state over
    {
      sensorvalue=analogRead(sensor); //read sensor value
      Serial.println(sensorvalue);
      if (sensorvalue < threshold) //check if within threshold
        emergencystate = false; //if safe, emergency resolved
      
    }
  }
void loop() { // runs a continuous loop of code

  do{
    currentTime = millis(); // timimg
    Serial.println(emergencystate); 
    if(!emergencystate){ //check if emergency state has occurred
      Serial.println("inhale");// signal start of inhale
      inhalefunct(); // call inhale function to start the pump and being forced  while monitoring pressure
    }
    currentTime = millis();  // timing
    Serial.println(emergencystate);
    if(!emergencystate){//check if emergency state has occurred
      Serial.println("exhale"); // signal start of exhale
      exhalefunct(); // call exhale function to power off pump to close off the oxygen getting pushed into the lungs to allow for exhale
    }
  }while(!emergencystate); //loop only if safe

  do{
    emergencyfunct(); //emergency routine
  }while(emergencystate);
}
