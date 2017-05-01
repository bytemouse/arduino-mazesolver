#ifndef _SENSOR_h
#define _SENSOR_h

#include <QTRSensors.h>

class Sensor
{
private:
	const unsigned char calibrationSeconds = 4;
	const unsigned char numberOfSensors = 6;			// number of sensors used
	const unsigned char numberOfSamplesPerSensor = 4;	// average 4 analog samples per sensor reading
	const unsigned char emitterPin = 2;					// emitter is controlled by digital pin 2
	unsigned char pins[6] = { 0,1,2,3,4,5 };
	unsigned int sensorValues[6];

	QTRSensorsAnalog qtra;
	unsigned int position;

public:
	void init();
	int getSensorValues();
};

#endif