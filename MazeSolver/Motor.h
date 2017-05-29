#pragma once
#include <Arduino.h>
#include "Direction.h"

void moveMotorOnSide(Direction side, Direction orientation, int speed)
{
	speed = max(min(speed, 180), 0);

	digitalWrite(side == left ? 13 : 12, orientation == forward ? HIGH : LOW);
	analogWrite(side == left ? 3 : 11, speed);
}

void moveBothMotors(int speedLeft, Direction orientationLeft, int speedRight, Direction orientationRight)
{
	moveMotorOnSide(left, orientationLeft, speedLeft);
	moveMotorOnSide(right, orientationRight, speedRight);
}
