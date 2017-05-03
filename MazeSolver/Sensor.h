#ifndef _SENSOR_h
#define _SENSOR_h

#include <QTRSensors.h>

class Sensor
{
private:
	const unsigned char calibrationSeconds = 4;
	const unsigned char numberOfSensors = 6;			// number of sensors used
	unsigned char pins[6] = { 0,1,2,3,4,5 };
	unsigned int sensorValues[6];

	QTRSensorsAnalog qtra;

public:
	float position;

	void init();
	void updateSensorValues();
	void printSensorValues();
};

#endif