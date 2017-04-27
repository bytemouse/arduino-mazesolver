#include <QTRSensors.h>

int lastError = 0;

#define NUM_SENSORS             6  // number of sensors used
#define NUM_SAMPLES_PER_SENSOR  4  // average 4 analog samples per sensor reading
#define EMITTER_PIN             2  // emitter is controlled by digital pin 2

// sensors 0 through 5 are connected to analog inputs 0 through 5, respectively
QTRSensorsAnalog qtra((unsigned char[]) {
  0, 1, 2, 3, 4, 5
},
NUM_SENSORS, NUM_SAMPLES_PER_SENSOR, EMITTER_PIN);
unsigned int sensorValues[NUM_SENSORS];


void setup() {
  //Setup Channel A
  pinMode(12, OUTPUT); //Initiates Motor Channel A pin

  //Setup Channel B
  pinMode(13, OUTPUT); //Initiates Motor Channel B pin

  delay(500);
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);    // turn on Arduino's LED to indicate we are in calibration mode
  for (int i = 0; i < 400; i++)  // make the calibration take about 10 seconds
  {
    qtra.calibrate();       // reads all sensors 10 times at 2.5 ms per six sensors (i.e. ~25 ms per call)
  }
  digitalWrite(13, LOW);     // turn off Arduino's LED to indicate we are through with calibration

  // print the calibration minimum values measured when emitters were on
  Serial.begin(9600);
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(qtra.calibratedMinimumOn[i]);
    Serial.print(' ');
  }
  Serial.println();

  // print the calibration maximum values measured when emitters were on
  for (int i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(qtra.calibratedMaximumOn[i]);
    Serial.print(' ');
  }
  Serial.println();
  Serial.println();
  delay(1000);
}


void loop() {
  
  int position = qtra.readLine(sensorValues);
  int M1, M2;
  float KP = 0.1; //floating-point proportional constant
  float KD = 5;   //floating-point derivative constant
  int error = position - 3000;

  int motorSpeed =  error + KD * (error - lastError);
  lastError = error;

  int m1Speed = M1 + motorSpeed;
  int m2Speed = M2 - motorSpeed;

  if (m1Speed < 0)
    m1Speed = 0;
  if (m2Speed < 0)
    m2Speed = 0;

  if (m1Speed > 255)
    m1Speed = 255;
  if (m2Speed > 255)
    m2Speed = 255;

  motorA(m1Speed);
  motorB(m2Speed);

   Serial.print(motorSpeed);
   Serial.print('\t');
    Serial.print(m1Speed);
    Serial.print('\t');
    Serial.print(m2Speed);
    Serial.print('\t');
    Serial.println();
    delay(500);
    
    
}

void motorA(int speed) {
  //Motor A forward
  digitalWrite(12, HIGH); //Establishes forward direction of Channel A
  analogWrite(3, speed);   //Spins the motor on Channel A
}

void motorB(int speed) {
  //Motor B forward 
  digitalWrite(13, HIGH); //Establishes forward direction of Channel B
  analogWrite(11, speed);   //Spins the motor on Channel B

}

void sensorOutput()
{
  // read calibrated sensor values and obtain a measure of the line position from 0 to 5000
  // To get raw sensor values, call:
  //  qtra.read(sensorValues); instead of unsigned int position = qtra.readLine(sensorValues);
  unsigned int position = qtra.readLine(sensorValues);
  
  // print the sensor values as numbers from 0 to 1000, where 0 means maximum reflectance and
  // 1000 means minimum reflectance, followed by the line position
  for (unsigned char i = 0; i < NUM_SENSORS; i++)
  {
    Serial.print(sensorValues[i]);
   
  }
  //Serial.println(); // uncomment this line if you are using raw values
  Serial.println(position); // comment this line out if you are using raw values
  
  delay(250);
}

