#include <Arduino.h>
#include "Motor.h"

Motor::Motor()
{
	//Setup Channel A
	pinMode(12, OUTPUT); //Initiates Motor Channel A pin

	//Setup Channel B
	pinMode(13, OUTPUT); //Initiates Motor Channel B pin
}

void Motor::drive(float position)
{
	if (direction == none)
	{
		moveBothMotors(0.0f, 0.0f);
		return;
	}

	float motorSpeed = kp * position + kd * (position - lastError);
	lastError = position;

	moveBothMotors(normalSpeed - motorSpeed, normalSpeed + motorSpeed);
}

void Motor::moveBothMotors(float speedLeft, float speedRight)
{
	moveMotorOnSide(left, speedLeft);
	Serial.print(' ');
	moveMotorOnSide(right, speedRight);
	Serial.println();
}

void Motor::moveMotorOnSide(Direction side, float speed)
{
	speed = max(min(speed, maxSpeed), 0);
	Serial.print("Motor ");
	Serial.print(side == left ? "left " : "right ");
	Serial.print(' ');
	Serial.print(speed);
	digitalWrite(side == left ? 13 : 12, HIGH);
	analogWrite(side == left ? 11 : 3, speed * 255);
}
