#include <QTRSensors.h>
#include "Direction.h"

const int numberOfSensors = 6;
const float proportionalConst = 0.1f;
const float derivateConst = 1.0f;
const int calibrationSeconds = 4;
const int twoHundred = 200;

unsigned char pins[6] = { 0, 1, 2, 3, 4, 5 };
QTRSensorsAnalog qtra(pins, numberOfSensors, 4, 2);
unsigned int sensorValues[numberOfSensors];

Direction direction = forward;

unsigned int position;
int lastError;
int loopIndex;

void setup()
{
	pinMode(12, OUTPUT);
	pinMode(13, OUTPUT);

	Serial.begin(9600);

	delay(500);

	calibrate();

	Serial.println('\n');
	delay(1000);
}

void loop()
{
	switch (Serial.read())
	{
	case '1':
		direction = none;
		break;
	case '2':
		direction = forward;
		break;
	default:
		break;
	}

	if (loopIndex % 100 == 0)
	{
		printSensorValues();
	}

	drive();

	loopIndex++;
}

void calibrate()
{
	for (int i = 0; i < calibrationSeconds / 0.025; i++)
	{
		qtra.calibrate();
	}

	// print the calibration minimum values measured when emitters were on
	for (int i = 0; i < numberOfSensors; i++)
	{
		Serial.print(qtra.calibratedMinimumOn[i]);
		Serial.print(' ');
	}
	Serial.println();

	// print the calibration maximum values measured when emitters were on
	for (int i = 0; i < numberOfSensors; i++)
	{
		Serial.print(qtra.calibratedMaximumOn[i]);
		Serial.print(' ');
	}
}

void drive()
{
	if (direction == none)
	{
		moveMotorOnSide(left, 0);
		moveMotorOnSide(right, 0);
		return;
	}

	position = qtra.readLine(sensorValues);
	int posPropotionalToMid = position - 2500;

	int motorSpeed = proportionalConst * posPropotionalToMid + derivateConst * (posPropotionalToMid - lastError);
	lastError = posPropotionalToMid;

	moveMotorOnSide(left, twoHundred - motorSpeed);
	//Serial.print(' ');
	moveMotorOnSide(right, twoHundred + motorSpeed);

}

void moveMotorOnSide(Direction side, int speed)
{
	speed = max(min(speed, 180), 0);
	//Serial.print("Motor ");
	//Serial.print(side == left ? "left " : "right ");
	//Serial.print(' ');
	//Serial.print(speed);
	digitalWrite(side == left ? 13 : 12, HIGH);
	analogWrite(side == left ? 3 : 11, speed);
}

void printSensorValues()
{
	for (unsigned char i = 0; i < numberOfSensors; i++)
	{
		Serial.print(sensorValues[i]);
		Serial.print('\t');
	}

	Serial.println((int)position - 2500);
}
