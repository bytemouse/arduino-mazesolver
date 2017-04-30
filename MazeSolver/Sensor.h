#ifndef _SENSOR_h
#define _SENSOR_h
#include "Motor.h"
#include <QTRSensors.h>

class Sensor {
private:
	const unsigned char calibrationSeconds = 4;
	const unsigned char NUM_SENSORS = 6;				// number of sensors used
	const unsigned char NUM_SAMPLES_PER_SENSOR = 4;		// average 4 analog samples per sensor reading
	const unsigned char EMITTER_PIN = 2;				// emitter is controlled by digital pin 2
	unsigned char pins[6] = { 0,1,2,3,4,5 };
	unsigned int sensorValues[6];

	QTRSensorsAnalog qtra;
	unsigned int position;

protected:

public:
	void init();
	int getSensorValues();
};

#endif