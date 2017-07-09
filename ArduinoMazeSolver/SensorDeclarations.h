#pragma once

QTRSensorsAnalog qtra(sensorPins, sizeof(sensorPins), 4, 2); //intalizes QTR-8A sensor to use library
unsigned int sensorValues[sizeof(sensorPins)]; // array of all sensorValues
unsigned int sensorPosition; // sensorPosition relative to lines Valus range from 0(left) to 5000(right)