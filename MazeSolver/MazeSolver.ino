#include <QTRSensors.h>
#include "Direction.h"

const int numberOfSensors = 6;
const int calibrationSeconds = 4;
const int treshold = 400;

// pid loop vars
const float proportionalConst = 0.1f;
const float derivateConst = 1.0f;

const int standardMotorSpeed = 180;

#pragma region "OldCode"

// TODO nicht nötig, weil nur einmal verwendet und der Speicherbedarf klein bleiben soll
//const int speedA = 3;  //speed control for motor outputs 1 and 2 is on digital pin 10  (Right motor)
//const int speedB = 11; //speed control for motor outputs 3 and 4 is on digital pin 11  (Left motor)
//const int dirA = 12;  //direction control for motor outputs 1 and 2 is on digital pin 12  (Right motor)
//const int dirB = 13;  //direction control for motor outputs 3 and 4 is on digital pin 13  (Left motor)

// TODO nie benutzt
//int turnSpeed = 200;  // tune value motors will run while turning (0-255) NOT TESTED
//int turnSpeedSlow = 125;  // tune value motors will run as they slow down from turning cycle to avoid overrun (0-255) NOT TESTED

#pragma endregion

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

	if (side == right)
	{
		if (orientation == forward)
		{
			digitalWrite(12, HIGH);
			analogWrite(3, speed);
		}
		else
		{
			digitalWrite(12, LOW);
			analogWrite(3, speed);
		}

	}
	else
	{
		if (orientation == forward)
		{
			digitalWrite(13, HIGH);
			analogWrite(11, speed);
		}
		else
		{
			digitalWrite(13, LOW);
			analogWrite(11, speed);
		}

	}

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

#pragma region "OldCode"

//// Turns to the sent variable of
//// 'L' (left), 'R' (right), 'S' (straight), or 'B' (back)
//// TODO Tune 'turnSpeed'
//void turn(char dir)
//{
//	switch (dir)
//	{
//		// Turn left 90deg
//	case 'L':
//		moveBothMotors(forward, back, turnSpeed);
//
//
//		position = qtrra.readLine(sensorValues);
//
//
//		while (sensorValues[5] < treshold)  // wait for outer most sensor to find the line
//		{
//			position = qtrra.readLine(sensorValues);
//		}
//
//		// slow down speed
//		moveBothMotors(forward, back, turnSpeedSlow);
//
//		// find center
//		while (position > 3000)  // tune - wait for line position to find near center
//		{
//			position = qtrra.readLine(sensorValues);
//		}
//
//		// stop both motors
//		moveBothMotors(forward, back, 0);
//		break;
//
//		// Turn right 90deg
//	case 'R':
//		moveBothMotors(back, forward, turnSpeed);
//
//		position = qtrra.readLine(sensorValues);
//
//		while (sensorValues[1] < treshold)  // wait for outer most sensor to find the line
//		{
//			position = qtrra.readLine(sensorValues);
//		}
//
//		// slow down speed
//		moveBothMotors(back, forward, turnSpeedSlow);
//
//		// find center
//		while (position < 3000)  // tune - wait for line position to find near center
//		{
//			position = qtrra.readLine(sensorValues);
//		}
//
//		// stop both motors
//		moveBothMotors(back, forward, 0);
//		break;
//
//		// Turn right 180deg to go back
//	case 'B':
//		moveBothMotors(back, forward, turnSpeed);
//
//		position = qtrra.readLine(sensorValues);
//
//		while (sensorValues[1] < treshold)  // wait for outer most sensor to find the line
//		{
//			position = qtrra.readLine(sensorValues);
//		}
//
//		// slow down speed
//		moveBothMotors(back, forward, turnSpeedSlow);
//
//		// find center
//		while (position < 300)  // tune - wait for line position to find near center
//		{
//			position = qtrra.readLine(sensorValues);
//		}
//
//		// stop both motors
//		moveBothMotors(back, forward, 0);
//		break;
//
//		// Straight ahead
//	case 'S':
//		// do nothing
//		break;
//	}
//} // end turn

//void MazeSolve()
//{
//	while (1)
//	{
//
//
//
//		// Drive straight a bit.
//		moveBothMotors(forward, forward, 200);
//		delay(25);
//
//		// These variables record whether the robot has seen a line to the
//		// left, straight ahead, and right, while examining the current
//		// intersection.
//		unsigned char found_left = 0; //TODO make this boolean
//		unsigned char found_straight = 0;
//		unsigned char found_right = 0;
//
//		// Now read the sensors and check the intersection type.
//		position = qtrra.readLine(sensorValues);
//
//		// Check for left and right exits.
//		if (sensorValues[0] > 400)
//			found_right = 1;
//		if (sensorValues[5] > 400)
//			found_left = 1;
//
//		// Drive straight a bit more 
//		moveBothMotors(forward, forward, 200);
//		delay(drivePastDelay);
//
//		position = qtrra.readLine(sensorValues);
//		// if(sensorValues[1] > 200 || sensorValues[2] > 200 || sensorValues[3] > 200 || sensorValues[4] > 200) //To Do: Which sensor values indicate a straight
//		// found_straight = 1;
//
//		// unsigned char dir = select_turn(found_left, found_straight, found_right); //TODO this method
//
//		 // Make the turn indicated by the path.
//		turn(dir);
//		//TODO End of the maze
//	}

//char selectTurn(unsigned char found_left, unsigned char found_straight, unsigned char found_right) //TODO make this boolean
//{
//	// Make a decision about how to turn.  The following code
//	// implements a left-hand-on-the-wall strategy, where we always
//	// turn as far to the left as possible.
//	if (found_left)
//		return 'L';
//	else if (found_straight)
//		return 'S';
//	else if (found_right)
//		return 'R';
//	else
//		return 'B';
//} // end select_turn

#pragma endregion
