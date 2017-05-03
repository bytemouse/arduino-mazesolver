#include <Arduino.h>
#include <QTRSensors.h>
#include "Sensor.h"

void Sensor::init()
{
	qtra = QTRSensorsAnalog(pins, numberOfSensors, 4, 2);

	for (int i = 0; i < calibrationSeconds / 0.025; i++)
	{
		qtra.calibrate();       // reads all sensors 10 times at 2.5 ms per six sensors (i.e. ~25 ms per call)
	}

	// print the calibration minimum values measured
	for (int i = 0; i < numberOfSensors; i++)
	{
		Serial.print(qtra.calibratedMinimumOn[i]);
		Serial.print(' ');
	}
	Serial.println();


	// print the calibration maximum values measured
	for (int i = 0; i < numberOfSensors; i++)
	{
		Serial.print(qtra.calibratedMaximumOn[i]);
		Serial.print(' ');
	}
}

void Sensor::updateSensorValues()
{
	position = qtra.readLine(sensorValues);
	position /= 2500;
	position -= 1.0f;
}

void Sensor::printSensorValues()
{
	for (unsigned char i = 0; i < numberOfSensors; i++)
	{
		Serial.print((float)sensorValues[i] / 1000);
		Serial.print('\t');
	}

	Serial.print("Linienposition:\t");
	Serial.println(position);
}
