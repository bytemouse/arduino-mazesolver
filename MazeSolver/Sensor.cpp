#include "Sensor.h"
#include <Arduino.h>
#include <QTRSensors.h>

void Sensor::init() {
	qtra = QTRSensorsAnalog(pins, NUM_SENSORS, NUM_SAMPLES_PER_SENSOR, EMITTER_PIN);

	digitalWrite(13, HIGH);    // turn on Arduino's LED to indicate we are in calibration mode

	for (int i = 0; i < calibrationSeconds / 0.025; i++) {
		qtra.calibrate();       // reads all sensors 10 times at 2.5 ms per six sensors (i.e. ~25 ms per call)
	}

	digitalWrite(13, LOW);     // turn off Arduino's LED to indicate we are through with calibration


	// print the calibration minimum values measured when emitters were on
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
}

int Sensor::getSensorValues() {
	// read calibrated sensor values and obtain a measure of the line position from 0 to 5000
	// To get raw sensor values, call:
	//  qtra.read(sensorValues); instead of unsigned int position = qtra.readLine(sensorValues);
	position = qtra.readLine(sensorValues);

	// print the sensor values as numbers from 0 to 1000, where 0 means maximum reflectance and
	// 1000 means minimum reflectance, followed by the line position
	for (unsigned char i = 0; i < NUM_SENSORS; i++)
	{
		Serial.print(sensorValues[i]);
		Serial.print('\t');

	}
	//Serial.println(); // uncomment this line if you are using raw values
	Serial.println((int)position - 2500); // comment this line out if you are using raw values

	return (int)position - 2500;
}
