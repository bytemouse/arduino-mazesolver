#include "Motor.h"
#include <QTRSensors.h>
#include "Direction.h"

#pragma region "Pin declerations"
const unsigned char ledPins[] = { 4, 5, 6, 7 };
unsigned char sensorPins[] = { 0, 1, 2, 3, 4, 5 };
const unsigned char pushButtonPin = 2;
const unsigned char bluetoothRxPin = 7;
const unsigned char bluetoothTxPin = 10;
#pragma endregion

const int threshold = 400;

// pid loop vars
const float proportionalConst = 0.2f;
const float derivateConst = 1.0f;

const int maxMotorSpeed = 150;

// data type change in order to save memory
// (I don't really like it, because it makes the code less readable IMHO, but ya)
unsigned char path[300];

unsigned int pathLength;
unsigned int pathPositionInLaterRun;

int pastDiversionTurnDelayMs = 300;

QTRSensorsAnalog qtra(sensorPins, sizeof(sensorPins), 4, 2);
unsigned int sensorValues[sizeof(sensorPins)];

unsigned char direction = forward;

unsigned int position;
int lastError;

bool isEachDiversionOnCrossing[3];

long diversionCheckingStartTime;
bool isDiversionCheckRunning;

bool isFirstRun = true;


#pragma region "Initialization"
void setup()
{
	//init motor pins
	pinMode(12, OUTPUT);
	pinMode(13, OUTPUT);

	//init LEDs
	for (unsigned char i = 0; i < sizeof(ledPins); i++)
	{
		pinMode(ledPins[i], OUTPUT);
	}

	pinMode(pushButtonPin, INPUT);

	Serial.begin(9600);

	delay(500);

	calibrate();

	Serial.println('\n');
	delay(500);
}


void calibrate()
{
	lightLed(0);
	lightLed(1);
	lightLed(2);
	lightLed(3);

	// make half-turns to have values for black and white without holding it
	for (int i = 0; i <= 100; i++)
	{
		if (i == 0 || i == 60)
		{
			moveBothMotors(maxMotorSpeed, backward, maxMotorSpeed, forward);
		}

		else if (i == 20 || i == 100)
		{
			moveBothMotors(maxMotorSpeed, forward, maxMotorSpeed, backward);
		}

		qtra.calibrate();
	}

	while (sensorValues[2] < threshold)
	{
		position = qtra.readLine(sensorValues);
	}

	moveBothMotors(0, forward, 0, forward);

	// print the calibration minimum values measured when emitters were on
	for (int i = 0; i < sizeof(sensorPins); i++)
	{
		Serial.print(qtra.calibratedMinimumOn[i]);
		Serial.print(' ');
	}
	Serial.println();

	// print the calibration maximum values measured when emitters were on
	for (int i = 0; i < sizeof(sensorPins); i++)
	{
		Serial.print(qtra.calibratedMaximumOn[i]);
		Serial.print(' ');
	}

	delay(300);
}

#pragma endregion

void loop()
{
	// stop driving when 1 is received over serial port
	// and restart when 2 is received
	switch (Serial.read())
	{
	case '1':
		direction = none;
		break;
	case '2':
		direction = forward;
		break;
	}

	// if button pressed start the simplified path
	// unused digital pins are 2, 7, 10 and two of them are for the bluetooth module
	if (digitalRead(pushButtonPin) == HIGH)
	{
		onRestartButtonKlicked();
	}

	drive();
}

#pragma region "DrivingPart"

void drive()
{
	// update position and sensorValues
	position = qtra.readLine(sensorValues);

	// if the time for the last step before turn is over
	if (direction == diversionChecking
		&& millis() > diversionCheckingStartTime + pastDiversionTurnDelayMs)
	{
		decideWhatDirection();
	}

	turnOffAllLeds();

	int motorSpeed;
	int posPropotionalToMid;

	switch (direction)
	{
	case diversionChecking:
		lightLed(1);

		moveBothMotors(maxMotorSpeed, forward, maxMotorSpeed, forward);
		checkForDiversions();
		break;

	case none:
		lightLed(0);
		lightLed(3);

		moveBothMotors(0, forward, 0, forward);


		// TODO remove this
		// for now supposed to replace the not existing push button
		// you just have to quickly pick the vehicle up and place it at the start position
		delay(4000);
		onRestartButtonKlicked();

		break;

	case backward:
		lightLed(2);
		lightLed(3);

		moveBothMotors(maxMotorSpeed, forward, maxMotorSpeed, backward);
		checkForNewLineOnSide(right);
		break;

	case left:
		lightLed(2);

		moveBothMotors(maxMotorSpeed, backward, maxMotorSpeed, forward);
		checkForNewLineOnSide(left);
		break;

	case forward:
		lightLed(0);

		posPropotionalToMid = position - 2500;

		motorSpeed = proportionalConst * posPropotionalToMid + derivateConst * (posPropotionalToMid - lastError);
		lastError = posPropotionalToMid;

		moveBothMotors(maxMotorSpeed - motorSpeed, forward, maxMotorSpeed + motorSpeed, forward);

		checkForDiversions();
		if (isEachDiversionOnCrossing[left] || isEachDiversionOnCrossing[right])
		{
			direction = diversionChecking;
			startFurtherDiversionCheckingTime();
		}
		else if (isDeadEnd())
		{
			direction = backward;
			if (isFirstRun)
			{
				path[pathLength] = backward;
				simplifyMaze();
				pathLength++;
			}
			else
			{
				pathPositionInLaterRun++;
			}
		}
		break;

	case right:
		lightLed(3);

		moveBothMotors(maxMotorSpeed, forward, maxMotorSpeed, backward);
		checkForNewLineOnSide(right);
		break;
	}
}

void lightLed(unsigned char index)
{
	digitalWrite(ledPins[index], HIGH);
}

void turnOffAllLeds()
{
	for (unsigned char i = 0; i < sizeof(ledPins); i++)
	{
		digitalWrite(ledPins[i], LOW);
	}
}

void onRestartButtonKlicked()
{
	pathPositionInLaterRun = 0;
	isFirstRun = false;
	direction = forward;
}

bool isDeadEnd()
{
	for (unsigned char i = 0; i < sizeof(sensorPins); i++)
	{
		if (sensorValues[i] > threshold)
		{
			return false;
		}
	}
	return true;
}

void checkForNewLineOnSide(Direction side)
{
	if (sensorValues[side == left ? 0 : sizeof(sensorPins) - 1] > threshold)
	{
		while (sensorValues[side == left ? 2 : sizeof(sensorPins) - 3] < threshold)
		{
			position = qtra.readLine(sensorValues);

			moveBothMotors(maxMotorSpeed, side == left ? backward : forward, maxMotorSpeed, side == left ? forward : backward);
		}

		direction = forward;
	}
}

void checkForDiversions()
{
	if (sensorValues[sizeof(sensorPins) - 1] > threshold)
	{
		isEachDiversionOnCrossing[right] = true;
	}
	if (sensorValues[0] > threshold)
	{
		isEachDiversionOnCrossing[left] = true;
	}
}

void decideWhatDirection()
{
	if (!isFirstRun)
	{
		direction = path[pathPositionInLaterRun];
		pathPositionInLaterRun++;
		return;
	}

	// Check if there is a way up front
	for (unsigned char i = 1; i < sizeof(sensorPins) - 1; i++)
	{
		if (sensorValues[i] > threshold)
		{
			isEachDiversionOnCrossing[forward] = true;
			break;
		}
	}

	// check if the destination was reached
	bool destinationWasReached = true;
	for (int i = 0; i < sizeof(sensorPins); i++)
	{
		if (sensorValues[i] < threshold)
		{
			destinationWasReached = false;
		}
	}

	// Stop if destination was reached
	// Else go left preferably
	if (destinationWasReached)
	{
		printPath();
		direction = none;
	}
	else
	{
		if (isEachDiversionOnCrossing[left])
		{
			direction = left;
		}
		else if (isEachDiversionOnCrossing[forward])
		{
			direction = forward;
		}
		else
		{
			direction = right;
		}

		path[pathLength] = direction;
		pathLength++;
		simplifyMaze();
	}

	// Reset for next crossing
	for (unsigned char i = 0; i < sizeof(isEachDiversionOnCrossing); i++)
	{
		isEachDiversionOnCrossing[i] = false;
	}
}

void startFurtherDiversionCheckingTime()
{
	diversionCheckingStartTime = millis();
}

// LBR = B
// LBS = R
// RBL = B
// SBL = R
// SBS = B
// LBL = S
void simplifyMaze()
{
	if (pathLength < 3 || path[pathLength - 2] != backward)
	{
		return;
	}

	int totalAngle = 0;

	for (unsigned char i = 1; i <= 3; i++)
	{
		switch (path[pathLength - i])
		{
		case right:
			totalAngle += 90;
			break;
		case left:
			totalAngle += 270;
			break;
		case backward:
			totalAngle += 180;
			break;
		}
	}

	// Get the angle as a number between 0 and 360 degrees.
	totalAngle = totalAngle % 360;

	// Replace all of those turns with a single one.
	switch (totalAngle)
	{
	case 0:
		path[pathLength - 3] = forward;
		break;
	case 90:
		path[pathLength - 3] = right;
		break;
	case 180:
		path[pathLength - 3] = backward;
		break;
	case 270:
		path[pathLength - 3] = left;
		break;
	}

	// The path is now two steps shorter.
	pathLength -= 2;
}

#pragma endregion

void printSensorValues()
{
	for (unsigned char i = 0; i < sizeof(sensorPins); i++)
	{
		Serial.print(sensorValues[i]);
		Serial.print('\t');
	}

	Serial.print(" linePosition: ");
	Serial.println((int)position - 2500);
}

void printPath()
{
	Serial.println("+++++++++++++++++");
	for (unsigned int i = 0; i < pathLength; i++)
	{
		Serial.println(path[i]);
	}
	Serial.println("+++++++++++++++++");
}
