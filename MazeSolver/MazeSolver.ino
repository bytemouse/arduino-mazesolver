#include <QTRSensors.h>
#include "Direction.h"

const int numberOfSensors = 6;
const int calibrationSeconds = 4;
const int treshold = 400;

// pid loop vars
const float proportionalConst = 0.1f;
const float derivateConst = 1.0f;

const int standardMotorSpeed = 180;

int drivePastDelay = 300; // tune value in mseconds motors will run past intersection to align wheels for turn NOT TESTED


unsigned char pins[6] = { 0, 1, 2, 3, 4, 5 };
QTRSensorsAnalog qtra(pins, numberOfSensors, 4, 2);
unsigned int sensorValues[numberOfSensors];

Direction direction = forward;

unsigned int position;
int lastError;
int loopIndex;

bool isEachDiversionOnCrossing[3];

long diversionCheckingStartTime;
bool isDiversionCheckRunning;


#pragma region "Initialization"
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


void calibrate()
{
	for (int i = 0; i <= 100; i++)
	{
		if (i == 0 || i == 60)
		{
			moveMotorOnSide(left, back, standardMotorSpeed);
			moveMotorOnSide(right, forward, standardMotorSpeed);
		}

		else if (i == 20 || i == 100)
		{
			moveMotorOnSide(left, forward, standardMotorSpeed);
			moveMotorOnSide(right, back, standardMotorSpeed);
		}

		qtra.calibrate();
	}

	while (sensorValues[2] < treshold)
	{
		position = qtra.readLine(sensorValues);
	}

	moveBothMotors(0, 0);

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
	delay(300);
}

#pragma endregion

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

	Serial.print(position);
	Serial.print("  ");
	Serial.print(sensorValues[0]);
	Serial.print("  ");
	Serial.println(sensorValues[5]);

	drive();

	loopIndex++;
}

#pragma region "DrivingPart"

void drive()
{
	position = qtra.readLine(sensorValues);

	if (direction == diversionChecking
		&& millis() > diversionCheckingStartTime + drivePastDelay)
	{
		endFurtherDiversionChecking();
	}

	int motorSpeed;
	int posPropotionalToMid;

	switch (direction)
	{
	case diversionChecking:
		moveBothMotors(standardMotorSpeed, standardMotorSpeed);
		checkForDiversions();
		break;
	case none:
		moveBothMotors(0, 0);
		break;
	case back:
		moveBothMotors(standardMotorSpeed, 0);
		checkForNewLineOnSide(right);
		break;
	case left:
		moveBothMotors(0, standardMotorSpeed);
		checkForNewLineOnSide(left);
		break;
	case forward:
		posPropotionalToMid = position - 2500;

		motorSpeed = proportionalConst * posPropotionalToMid + derivateConst * (posPropotionalToMid - lastError);
		lastError = posPropotionalToMid;

		moveBothMotors(standardMotorSpeed - motorSpeed, standardMotorSpeed + motorSpeed);

		checkForDiversions();
		if (isEachDiversionOnCrossing[left] || isEachDiversionOnCrossing[right])
		{
			direction = diversionChecking;
			startFurtherDiversionCheckingTime();
		}
		break;
	case right:
		moveBothMotors(standardMotorSpeed, 0);
		checkForNewLineOnSide(right);
		break;
	}
}

void checkForNewLineOnSide(Direction side)
{
	if (sensorValues[side == left ? 0 : numberOfSensors - 1] > treshold)
	{
		direction = forward;
	}
}

void moveMotorOnSide(Direction side, Direction orientation, int speed)
{
	speed = max(min(speed, 180), 0);
	
	digitalWrite(side == left ? 13 : 12, orientation == forward ? HIGH : LOW);
	analogWrite(side == left ? 3 : 11, speed);
}

void moveBothMotors(int speedLeft, int speedRight)
{
	moveMotorOnSide(left, forward, speedLeft);
	moveMotorOnSide(right, forward, speedRight);
}

void checkForDiversions()
{
	if (sensorValues[numberOfSensors - 1] > treshold)
	{
		isEachDiversionOnCrossing[right] = true;
	}
	if (sensorValues[0] > treshold)
	{
		isEachDiversionOnCrossing[left] = true;
	}
}

void endFurtherDiversionChecking()
{
	// Check if there is a way up front
	for (unsigned char i = 1; i < numberOfSensors - 1; i++)
	{
		if (sensorValues[i] > treshold)
		{
			isEachDiversionOnCrossing[forward] = true;
			break;
		}
	}

	// Go left preferably
	if (isEachDiversionOnCrossing[left] == true)
	{
		direction = left;
	}
	else if (isEachDiversionOnCrossing[forward] == true)
	{
		direction = forward;
	}
	else
	{
		direction = right;
	}

	// Reset for next crossing
	for (unsigned char i = 0; i < 3; i++)
	{
		isEachDiversionOnCrossing[i] = false;
	}
}

void startFurtherDiversionCheckingTime()
{
	diversionCheckingStartTime = millis();
}

#pragma endregion

void printSensorValues()
{
	for (unsigned char i = 0; i < numberOfSensors; i++)
	{
		Serial.print(sensorValues[i]);
		Serial.print('\t');
	}

	Serial.println((int)position - 2500);
}
