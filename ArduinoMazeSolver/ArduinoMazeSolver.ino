#include "Motor.h"
#include <QTRSensors.h>
#include "Direction.h"
#include <SoftwareSerial.h>
#include "ProtocolBytes.h"
#include "Turn.h"

#pragma region "Pin declarations"
const byte ledPins[] = { 4, 5, 6, 8 };
unsigned char sensorPins[] = { 0, 1, 2, 3, 4, 5 };
SoftwareSerial bluetoothSerial(7, 10);
#pragma endregion

#pragma region "Variable declarations"
const int threshold = 400;

// pd loop constants
const float proportionalConst = 0.2f;
const float derivateConst = 1.0f;

const int maxMotorSpeed = 255;

Turn fullPath[100];
Turn simplePath[100];

byte pathLength;
byte fullPathLength;
byte pathPositionInLaterRun;

int pastDiversionTurnDelayMs = 150;

QTRSensorsAnalog qtra(sensorPins, sizeof(sensorPins), 4, 2);
unsigned int sensorValues[sizeof(sensorPins)];

Direction direction = forward;

unsigned int sensorPosition;
int lastError;

bool isEachDiversionOnCrossing[3];

unsigned long diversionCheckingStartTime;
bool isDiversionCheckRunning;

bool isFirstRun = true;

unsigned long lastTurnMs;

bool isNotPausing = false;

unsigned long lastBluetoothSendTryMs;
bool lastBluetoothPacketReceived;
#pragma endregion


#pragma region "Initialization"
void setup()
{
	//init motor pins
	pinMode(12, OUTPUT);
	pinMode(13, OUTPUT);

	//init LEDs
	for (byte i = 0; i < sizeof(ledPins); i++)
	{
		pinMode(ledPins[i], OUTPUT);
	}

	Serial.begin(9600);
	bluetoothSerial.begin(9600);


	delay(500);

	calibrate();

	delay(500);

	lastTurnMs = millis();

	bluetoothSerial.write(byteRequestClearAndroidMaze);
	Serial.print("Setup finished");
}


void calibrate()
{
	// make half-turns to have values for black and white without holding it
	for (byte i = 0; i <= 100; i++)
	{
		if (i == 0 || i == 60)
		{
			moveBothMotors(150, backward, 150, forward);
		}

		else if (i == 20 || i == 100)
		{
			moveBothMotors(150, forward, 150, backward);
		}

		qtra.calibrate();
	}

	while (sensorValues[2] < threshold)
	{
		sensorPosition = qtra.readLine(sensorValues);
	}

	moveBothMotors(0, forward, 0, forward);

	// print the calibration minimum values measured when emitters were on
	for (byte i = 0; i < sizeof(sensorPins); i++)
	{
		Serial.print(qtra.calibratedMinimumOn[i]);
		Serial.print(' ');
	}
	Serial.println();

	// print the calibration maximum values measured when emitters were on
	for (byte i = 0; i < sizeof(sensorPins); i++)
	{
		Serial.print(qtra.calibratedMaximumOn[i]);
		Serial.print(' ');
	}

	delay(300);
}

#pragma endregion

void loop()
{
	byte btReceivedByte;

	while (bluetoothSerial.available())
	{
		btReceivedByte = bluetoothSerial.read();

		switch (btReceivedByte)
		{
		case byteRequestStartDriving:
			isNotPausing = true;
			break;

		case byteRequestStopDriving:
			isNotPausing = false;
			break;

		case byteResponse:
			lastBluetoothPacketReceived = true;
			break;
		}
	}

	if (isNotPausing)
	{
		drive();
	}
	else
	{
		moveBothMotors(0, forward, 0, forward);
	}
}

#pragma region "DrivingPart"

void drive()
{
	// update position and sensorValues
	sensorPosition = qtra.readLine(sensorValues);

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
		moveBothMotors(maxMotorSpeed, forward, maxMotorSpeed, forward);
		checkForDiversions();
		break;

	case none:
		moveBothMotors(0, forward, 0, forward);

		// restart when vehicle is placed at the start position again
		if (getNumberOfCurrentlyWhiteSensors() > 0)
		{
			delay(1000);
			startNextRun();
		}
		break;

	case backward:
		moveBothMotors(maxMotorSpeed, forward, maxMotorSpeed, backward);
		checkForNewLineOnSide(right);
		break;

	case left:
		moveBothMotors(maxMotorSpeed, backward, maxMotorSpeed, forward);
		checkForNewLineOnSide(left);
		break;

	case forward:
		posPropotionalToMid = sensorPosition - 2500;

		motorSpeed = proportionalConst * posPropotionalToMid + derivateConst * (posPropotionalToMid - lastError);
		lastError = posPropotionalToMid;

		moveBothMotors(maxMotorSpeed - motorSpeed, forward, maxMotorSpeed + motorSpeed, forward);

		checkForDiversions();
		if (isEachDiversionOnCrossing[left] || isEachDiversionOnCrossing[right])
		{
			direction = diversionChecking;
			startFurtherDiversionCheckingTime();
		}
		// dead end
		else if (getNumberOfCurrentlyWhiteSensors() == sizeof(sensorPins))
		{
			direction = backward;
			storeTurnToPath();
		}
		break;

	case right:
		moveBothMotors(maxMotorSpeed, forward, maxMotorSpeed, backward);
		checkForNewLineOnSide(right);
		break;
	}
}

byte getNumberOfCurrentlyWhiteSensors()
{
	byte currentlyWhiteSensors = 0;
	for (byte i = 0; i < sizeof(sensorPins); i++)
	{
		if (sensorValues[i] < threshold)
		{
			currentlyWhiteSensors++;
		}
	}
	return currentlyWhiteSensors;
}

void checkForNewLineOnSide(Direction side)
{
	if (sensorValues[side == left ? 0 : sizeof(sensorPins) - 1] > threshold)
	{
		while (sensorValues[side == left ? 2 : sizeof(sensorPins) - 3] < threshold)
		{
			sensorPosition = qtra.readLine(sensorValues);

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

void startNextRun()
{
	//printPathLed();
	simplePath[pathLength + 1].direction = none;

	pathPositionInLaterRun = 0;
	isFirstRun = false;
	if (simplePath[pathPositionInLaterRun].direction == backward)
	{
		direction = backward;
		pathPositionInLaterRun++;
	}
	else
	{
		direction = forward;
	}
}

void decideWhatDirection()
{
	if (!isFirstRun)
	{
		direction = simplePath[pathPositionInLaterRun].direction;
		pathPositionInLaterRun++;
	}
	else
	{
		if (getNumberOfCurrentlyWhiteSensors() == 0)
		{
			direction = none;
			sendAllTurns();
		}
		else
		{
			// Check if there is a way up front
			if (getNumberOfCurrentlyWhiteSensors() < sizeof(sensorPins))
			{
				isEachDiversionOnCrossing[forward] = true;
			}

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

			storeTurnToPath();
		}
	}

	// Reset for next crossing
	for (byte i = 0; i < sizeof(isEachDiversionOnCrossing); i++)
	{
		isEachDiversionOnCrossing[i] = false;
	}

}

void storeTurnToPath()
{
	byte byteDirection = getDirectionByte(direction);

	byte bytesTurn[] =
	{
	   byteStarting,
	   fullPathLength,
	   byteDirection,

	   // send with a precision of 50 ms
	   (byte)((millis() - lastTurnMs) / 50),

	   byteFinished
	};

	lastTurnMs = millis();
	bluetoothSerial.write(bytesTurn, sizeof(bytesTurn));
	
	fullPath[fullPathLength].indexOfTurns = bytesTurn[1];
	fullPath[fullPathLength].time50ms = bytesTurn[3];
	fullPath[fullPathLength].direction = direction;

	simplePath[pathLength].direction = direction;

	pathLength++;
	fullPathLength++;
	simplifyMaze();

}

void sendTurn(Turn inputTurn)
{
	byte outputBytesTurn[] =
	{
	   byteStarting,
	   inputTurn.indexOfTurns,
	   getDirectionByte(inputTurn.direction),
	   inputTurn.time50ms,
	   byteFinished
	};

	bluetoothSerial.write(outputBytesTurn, sizeof(outputBytesTurn));
}

byte getDirectionByte(Direction dir)
{
	switch (dir)
	{
	case left:
		return byteLeft;
		break;
	case forward:
		return byteForward;
		break;
	case right:
		return byteRight;
		break;
	case backward:
		return byteBackward;
		break;
	default:
		return 0;
		break;
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
	if (pathLength < 3 || simplePath[pathLength - 2].direction != backward)
	{
		return;
	}

	int totalAngle = 0;

	for (byte i = 1; i <= 3; i++)
	{
		switch (simplePath[pathLength - i].direction)
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
		simplePath[pathLength - 3].direction = forward;
		break;
	case 90:
		simplePath[pathLength - 3].direction = right;
		break;
	case 180:
		simplePath[pathLength - 3].direction = backward;
		break;
	case 270:
		simplePath[pathLength - 3].direction = left;
		break;
	}

	// The path is now two steps shorter.
	simplePath[pathLength - 1].direction = none;
	simplePath[pathLength - 2].direction = none;
	pathLength -= 2;
}

#pragma endregion

#pragma region "Leds"
void lightLed(byte ledIndex)
{
	digitalWrite(ledPins[ledIndex], HIGH);
}

void turnOffAllLeds()
{
	for (byte i = 0; i < sizeof(ledPins); i++)
	{
		digitalWrite(ledPins[i], LOW);
	}
}

void ledDirection(byte ledDir)
{
	switch (ledDir)
	{
	case diversionChecking:
		lightLed(1);
		break;
	case none:
		lightLed(0);
		lightLed(3);
		break;
	case backward:
		lightLed(2);
		lightLed(3);
		break;
	case left:
		lightLed(2);
		break;
	case forward:
		lightLed(0);
		break;
	case right:
		lightLed(3);
		break;
	}
}
#pragma endregion

#pragma region "Diagnostics"
void printSensorValues()
{
	for (byte i = 0; i < sizeof(sensorPins); i++)
	{
		Serial.print(sensorValues[i]);
		Serial.print('\t');
	}

	Serial.print(" linePosition: ");
	Serial.println((int)sensorPosition - 2500);
}

void printPath()
{
	Serial.println("+++++++++++++++++");
	for (byte i = 0; i < pathLength; i++)
	{
		Serial.println(fullPath[i].direction);
	}
	Serial.println("+++++++++++++++++");
}

void printPathLed()
{
	turnOffAllLeds();
	moveBothMotors(0, forward, 0, forward);
	for (byte i = 0; i < pathLength; i++)
	{
		delay(500);
		ledDirection(fullPath[i].direction);
		delay(1000);
		turnOffAllLeds();
	}
}

void sendAllTurns()
{	
	moveBothMotors(0, forward, 0, forward);
	bluetoothSerial.write(byteRequestClearAndroidMaze);
	for(byte i=0; i<fullPathLength; i++)
	{
		sendTurn(fullPath[i]);
		delay(1000);
	}	
}
#pragma endregion

void shutDown()
{
	turnOffAllLeds();
	moveBothMotors(0, forward, 0, forward);
	exit(0);
}
