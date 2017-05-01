#include "Sensor.h"
#include "Motor.h"

Sensor sensor;
Motor motor;

void setup()
{
	Serial.begin(9600);
	sensor.init();
	motor.direction = forward;
}


void loop()
{
	motor.drive(sensor.getSensorValues());
}
