#pragma once
#include <Arduino.h>
#include "Direction.h"

void moveMotorOnSide(Direction side, Direction orientation, byte speed)
{
	speed = max(min(speed, 255), 0);

	digitalWrite(side == left ? 13 : 12, orientation == forward ? HIGH : LOW);
	analogWrite(side == left ? 3 : 11, speed);
}

void moveBothMotors(byte speedLeft, Direction orientationLeft, byte speedRight, Direction orientationRight)
{
	moveMotorOnSide(left, orientationLeft, speedLeft);
	moveMotorOnSide(right, orientationRight, speedRight);
}
