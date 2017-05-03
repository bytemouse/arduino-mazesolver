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
	switch (Serial.read())
	{
	case '1':
		motor.direction = none;
		break;
	case '2':
		motor.direction = forward;
		break;
	default:
		break;
	}

	sensor.updateSensorValues();
	sensor.printSensorValues();
	motor.drive(sensor.position);
}
