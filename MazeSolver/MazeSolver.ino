#include "Sensor.h"
#include <QTRSensors.h>
#include "Motor.h"

int lastError = 0;

Sensor sensor;
Motor motor;

void setup() {
	Serial.begin(9600);
	sensor.init();
	motor.direction = forward;
}


void loop() {
	motor.drive(sensor.getSensorValues());
}
